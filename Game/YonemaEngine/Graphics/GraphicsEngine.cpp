#include "GraphicsEngine.h"
#include "../GameWindow/GameWindow.h"
#include "../../Application.h"
#include "Dx12Wrappers/Texture.h"
#include "Renderers/Renderer.h"
#include "Fonts/FontEngine.h"
#include "../Effect/EffectEngine.h"
#include "../Memory/ResourceBankTable.h"


namespace nsYMEngine
{
	namespace nsGraphics
	{
		CGraphicsEngine* CGraphicsEngine::m_instance = nullptr;

		const wchar_t* CGraphicsEngine::m_kGPUAdapterNames[EnGPUAdapterType::NumGPUAdapterType] =
		{
			// 上から使用優先度順
			L"NVIDIA",
			L"AMD",
			L"Intel"
		};

		const D3D_FEATURE_LEVEL CGraphicsEngine::m_kFeatureLevels[m_kNumFeatureLevel]
		{
			D3D_FEATURE_LEVEL_12_1,
			D3D_FEATURE_LEVEL_12_0,
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0
		};

		using RendererType = nsRenderers::CRendererTable::EnRendererType;


		CGraphicsEngine::~CGraphicsEngine()
		{
			Terminate();
			m_instance = nullptr;

			return;
		}


		bool CGraphicsEngine::Init()
		{
			IDXGIFactory6* dxgiFactory = nullptr;
			if (CreateDXGIFactory(&dxgiFactory) != true)
			{
				return false;
			}
			
			if (CreateDevice(dxgiFactory) != true)
			{
				return false;
			}

			// デバイスを作ったらすぐに設定。
			m_descriptorSizeOfCbvSrvUav = m_device->GetDescriptorHandleIncrementSize(
				D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			m_descriptorSizeOfRtv = m_device->GetDescriptorHandleIncrementSize(
				D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

			if (CreateCommandAllocator() != true)
			{
				return false;
			}
			{
				// コマンドリストとコマンドキューで同じタイプを使う。
				constexpr D3D12_COMMAND_LIST_TYPE commandListType = D3D12_COMMAND_LIST_TYPE_DIRECT;
				if (CreateCommandList(commandListType) != true)
				{
					return false;
				}

				if (CreateCommandQueue(commandListType) != true)
				{
					return false;
				}
			}

			auto hwnd = nsGameWindow::CGameWindow::GetInstance()->GetHWND();
			const auto kWindowWidth = CApplication::GetInstance()->GetWindowWidth();
			const auto kWindowHeight = CApplication::GetInstance()->GetWindowHeight();

			if (m_frameBuffer.Init(
				m_device, dxgiFactory, m_commandQueue, hwnd, kWindowWidth, kWindowHeight)
				 != true)
			{
				return false;
			}

			if (CreateFence() != true)
			{
				return false;
			}

			m_rendererTable.Init();

			if (CreateMainRenderTarget() != true)
			{
				return false;
			}
			nsSprites::SSpriteInitData initData;
			initData.texture = m_mainRenderTarget.GetRenderTargetTexture();
			initData.spriteSize = initData.texture->GetTextureSize();
			m_mainRenderTargetSprite.Init(initData);

			auto frameBuffResDesc = m_frameBuffer.GetResourceDesc();
			auto res = m_simplePostEffectRenderTarget.Init(
				static_cast<unsigned int>(frameBuffResDesc.Width),
				frameBuffResDesc.Height,
				frameBuffResDesc.Format,
				m_frameBuffer.GetRtvClearColor(),
				DXGI_FORMAT_UNKNOWN,
				L"SimplePostEffectRenderTarget"
			);

			initData.texture = m_simplePostEffectRenderTarget.GetRenderTargetTexture();
			initData.spriteSize = initData.texture->GetTextureSize();
			m_simplePostEffectRenderTargetSprite.Init(initData);
			m_rendererTable.RegisterRenderer(RendererType::enSimplePostEffect, &m_mainRenderTargetSprite);

			//m_shadowMapRenderer.Init();

			// 初期化が終わったら、DXGIFactoryはもういらないため破棄する。
			dxgiFactory->Release();

			m_fontEngine = nsFonts::CFontEngine::CreateInstance();

			// 基本となるレンダーターゲットスプライトに設定する
			m_pBaseRenderTargetSprite = &m_simplePostEffectRenderTargetSprite;


			m_defaultTextures.Init();

			m_mainCamera.SetPosition({ 0.0f,10.0f,-25.0f });
			m_mainCamera.SetTargetPosition({ 0.0f,10.0f,0.0f });
			m_mainCamera.SetUpDirection(nsMath::CVector3::Up());
			m_mainCamera.UpdateCameraParam();

			CreateSeceneConstantBuff();

			m_gfxMemForDirectXTK = new DirectX::GraphicsMemory(m_device);

			return true;
		}

		void CGraphicsEngine::Terminate()
		{
			// 破棄する前に、コマンドの実行の完了を待つ。
			WaitForCommandExecutionToComplete();
			if (m_gfxMemForDirectXTK)
			{
				delete m_gfxMemForDirectXTK;
			}

			m_defaultTextures.Release();

			nsFonts::CFontEngine::DeleteInstance();
			m_sceneDataDH.Release();
			m_sceneDataCB.Release();
			//m_shadowMapRenderer.Release();
			m_simplePostEffectRenderTargetSprite.Release();
			m_simplePostEffectRenderTarget.Release();
			m_mainRenderTargetSprite.Release();
			m_mainRenderTarget.Release();
			if (m_fence)
			{
				m_fence->Release();
				m_fence = nullptr;
			}
			m_frameBuffer.Release();
			if (m_commandQueue)
			{
				m_commandQueue->Release();
				m_commandQueue = nullptr;
			}
			m_commandList.Release();
			if (m_commandAllocator)
			{
				m_commandAllocator->Release();
			}
			if (m_device)
			{
				m_device->Release();
				m_device = nullptr;
			}

			return;
		}

		void CGraphicsEngine::Update()
		{
			m_mainCamera.UpdateCameraParam();

			auto mappedSceneData = 
				static_cast<SSceneDataMatrix*>(m_sceneDataCB.GetMappedConstantBuffer());
			mappedSceneData->mView = m_mainCamera.GetViewMatirx();;
			mappedSceneData->mProj = m_mainCamera.GetProjectionMatirx();
			mappedSceneData->cameraPosWS = m_mainCamera.GetPosition();

			return;
		}

		void CGraphicsEngine::ExecuteDraw()
		{
			// 描画開始処理。更新処理より後、描画処理より前に呼ぶこと。
			BeginDraw();

			DrawToShadowMap();

			DrawToMainRenderTarget();

			DrawWithSimplePostEffect();

			DrawCollision();

			nsEffect::CEffectEngine::GetInstance()->Draw(&m_commandList);

			Draw2D();

			m_fontEngine->ExecuteDraw(&m_commandList);

			EndDraw();

			return;
		}


		void CGraphicsEngine::BeginDraw()
		{
			// ○リセット
			auto result = m_commandAllocator->Reset();
			result = m_commandList.Reset(m_commandAllocator, nullptr);

			return;
		}

		void CGraphicsEngine::DrawToShadowMap()
		{
			//m_shadowMapRenderer.Draw();

			return;
		}


		void CGraphicsEngine::DrawToMainRenderTarget()
		{
			// 深度バッファはフレームバッファと同じものを使用。
			auto dsvH = m_frameBuffer.GetDsvCpuDescriptorHandle();
			const nsDx12Wrappers::CRenderTarget* rtArray[] = { &m_mainRenderTarget };

			// 描画先を設定
			m_commandList.TransitionFromShaderResourceToRenderTarget(m_mainRenderTarget);
			m_commandList.SetRenderTargets(1, rtArray, dsvH);

			// 画面クリア
			m_commandList.ClearRenderTargetViews(1, rtArray);
			m_commandList.ClearDepthStencilView(dsvH);

			// ビューポートとシザリング矩形はフレームバッファと同じものを使用。
			m_commandList.SetViewportAndScissorRect(
				m_frameBuffer.GetViewport(), m_frameBuffer.GetScissorRect());

			m_commandList.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			using CRendererTable = nsRenderers::CRendererTable;
			for (
				unsigned int rendererType = CRendererTable::m_kFirstIndexOfModel;
				rendererType <= CRendererTable::m_kLastIndexOfModel;
				rendererType++
				)
			{
				m_commandList.SetGraphicsRootSignatureAndPipelineState(
					m_rendererTable.GetRootSignature(rendererType),
					m_rendererTable.GetPipelineState(rendererType)
				);

				for (auto renderer : m_rendererTable.GetRendererList(rendererType))
				{
					renderer->DrawWrapper(&m_commandList);
				}
			}



			// 描画終了
			m_commandList.TransitionFromRenderTargetToShaderResource(m_mainRenderTarget);


			return;
		}


		void CGraphicsEngine::DrawWithSimplePostEffect()
		{
			// 描画先を設定
			m_commandList.TransitionFromShaderResourceToRenderTarget(m_simplePostEffectRenderTarget);
			m_commandList.SetRenderTarget(m_simplePostEffectRenderTarget);

			// 画面クリア
			m_commandList.ClearRenderTargetAndDepthStencilView(m_simplePostEffectRenderTarget);

			// インデックスバッファを指定していないため、TRIANGLELISTではなく、
			// TRIANGLESTRIPを指定する。
			m_commandList.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

			// シンプルなポストエフェクトの描画設定
			m_commandList.SetGraphicsRootSignatureAndPipelineState(
				m_rendererTable.GetRootSignature(
					RendererType::enSimplePostEffect),
				m_rendererTable.GetPipelineState(
					RendererType::enSimplePostEffect)
			);
			// シンプルなポストエフェクトをかけて描画
			for (auto renderer : m_rendererTable.GetRendererList(RendererType::enSimplePostEffect))
			{
				renderer->DrawWrapper(&m_commandList);
			}

			// 描画終了
			m_commandList.TransitionFromRenderTargetToShaderResource(m_simplePostEffectRenderTarget);

			return;
		}

		void CGraphicsEngine::DrawCollision()
		{
#ifdef _DEBUG
			// 深度バッファはフレームバッファと同じものを使用。
			auto dsvH = m_frameBuffer.GetDsvCpuDescriptorHandle();
			const nsDx12Wrappers::CRenderTarget* rtArray[] = { &m_simplePostEffectRenderTarget };

			// 描画先を設定
			m_commandList.TransitionFromShaderResourceToRenderTarget(m_simplePostEffectRenderTarget);
			m_commandList.SetRenderTargets(1, rtArray, dsvH);

			// 画面クリア
			//m_commandList.ClearRenderTargetViews(1, rtArray);

			// ビューポートとシザリング矩形はフレームバッファと同じものを使用。
			m_commandList.SetViewportAndScissorRect(
				m_frameBuffer.GetViewport(), m_frameBuffer.GetScissorRect());

			m_commandList.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);


			m_commandList.SetGraphicsRootSignatureAndPipelineState(
				m_rendererTable.GetRootSignature(
					RendererType::enCollisionRenderer),
				m_rendererTable.GetPipelineState(
					RendererType::enCollisionRenderer)
			);

			for (auto renderer : m_rendererTable.GetRendererList(RendererType::enCollisionRenderer))
			{
				renderer->DrawWrapper(&m_commandList);
			}

			// 描画終了
			m_commandList.TransitionFromRenderTargetToPresent(m_simplePostEffectRenderTarget);
#endif // _DEBUG
			return;
		}


		void CGraphicsEngine::Draw2D()
		{
			// フレームバッファに書き込む前に、バックバッファとフロントバッファを入れ替える。
			m_frameBuffer.SwapBackBuffer();

			// 描画先を設定
			m_commandList.TransitionFromPresentToRenderTarget(m_frameBuffer);
			m_commandList.SetRenderTarget(m_frameBuffer);

			// 画面クリア
			m_commandList.ClearRenderTargetAndDepthStencilView(m_frameBuffer);

			// インデックスバッファを指定していないため、TRIANGLELISTではなく、
			// TRIANGLESTRIPを指定する。
			m_commandList.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);


			// 不透明スプライトの描画設定
			m_commandList.SetGraphicsRootSignatureAndPipelineState(
				m_rendererTable.GetRootSignature(
					RendererType::enSprite),
				m_rendererTable.GetPipelineState(
					RendererType::enSprite)
			);

			// まず基本のレンダーターゲットのスプライト描画
			m_pBaseRenderTargetSprite->Draw(&m_commandList);

			// 不透明スプライト描画
			for (auto renderer : m_rendererTable.GetRendererList(RendererType::enSprite))
			{
				renderer->DrawWrapper(&m_commandList);
			}

			// 半透明スプライトの描画設定
			m_commandList.SetGraphicsRootSignatureAndPipelineState(
				m_rendererTable.GetRootSignature(
					RendererType::enTransSprite),
				m_rendererTable.GetPipelineState(
					RendererType::enTransSprite)
			);
			// 半透明スプライト描画
			for (auto renderer : m_rendererTable.GetRendererList(RendererType::enTransSprite))
			{
				renderer->DrawWrapper(&m_commandList);
			}

			// 描画終了
			m_commandList.TransitionFromRenderTargetToPresent(m_frameBuffer);

			return;
		}

		void CGraphicsEngine::EndDraw()
		{
			// 命令の実行前に、必ずコマンドリストのクローズを行う。
			m_commandList.Close();

			// コマンド実行
			ID3D12CommandList* commandLists[] = { m_commandList.Get() };
			m_commandQueue->ExecuteCommandLists(1, commandLists);

			// GPUとCPU同期
			WaitForCommandExecutionToComplete();

			// フリップ
			m_frameBuffer.Present();

			// フリップ後にコミット
			m_gfxMemForDirectXTK->Commit(m_commandQueue);

			m_gfxMemForDirectXTK->GarbageCollect();

			return;
		}

		bool CGraphicsEngine::CreateDXGIFactory(IDXGIFactory6** dxgiFactory)
		{
			HRESULT result;
			UINT dxgiFactoryFlags = 0;
#ifdef _DEBUG
			EnableDebugLayer();	
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
			result = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(dxgiFactory));

			if (FAILED(result))
			{
				nsGameWindow::MessageBoxError(L"DXGIFactoryオブジェクトの生成に失敗しました。");
				return false;
			}

			return true;
		}

		void CGraphicsEngine::EnableDebugLayer()
		{
			ID3D12Debug* debugLayer = nullptr;
			auto result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));

			// デバックレイヤーを有効にする。
			debugLayer->EnableDebugLayer();
			// 有効にしたら、インターフェースを開放する。
			debugLayer->Release();
			return;
		}

		bool CGraphicsEngine::CreateDevice(IDXGIFactory6* dxgiFactory)
		{
			IDXGIAdapter* adapter = nullptr;
			FindAdapter(dxgiFactory, &adapter);

			for (auto featureLevel : m_kFeatureLevels)
			{
				auto hr = D3D12CreateDevice(adapter, featureLevel, IID_PPV_ARGS(&m_device));

				if (SUCCEEDED(hr))
				{
					// 生成可能なバージョンを見つけたら、ループを打ち切り。
					break;
				}
			}

			// デバイス生成が終わったら、アダプタはもう使用しないため、破棄。
			if (adapter)
			{
				adapter->Release();
			}

			if (m_device == nullptr)
			{
				nsGameWindow::MessageBoxError(L"デバイスの生成に失敗しました。");
				return false;
			}

			return true;
		}

		void CGraphicsEngine::FindAdapter(IDXGIFactory6* dxgiFactory, IDXGIAdapter** adapterToUse)
		{
			// 最後にまとめて破棄するために、調べたアダプタすべてを保持しておくリスト。
			std::list<IDXGIAdapter*> adapters;
			// 調べる対象のアダプタ。
			IDXGIAdapter* tempAdapter = nullptr;
			// GPUの種類別のアダプタ。
			// 同じ種類のアダプタがあった場合は、ビデオメモリが大きい方を保持。
			IDXGIAdapter* adaptersByType[EnGPUAdapterType::NumGPUAdapterType] = { nullptr };
			// すべてのアダプタの中で、1番ビデオメモリが大きいアダプタを保持。
			IDXGIAdapter* maxVideoMemoryAdapter = nullptr;
			SIZE_T videoMomorySize = 0;

			// 1：利用可能なアダプタを探す

			// PCにあるアダプタをそれぞれ調べる
			for (int i = 0; dxgiFactory->EnumAdapters(i, &tempAdapter) != DXGI_ERROR_NOT_FOUND; i++)
			{
				adapters.emplace_back(tempAdapter);
				DXGI_ADAPTER_DESC tempAdapterDesc = {};
				tempAdapter->GetDesc(&tempAdapterDesc);

				// 優先度の高いアダプタがあるか調べる
				for (int adapterType = 0; adapterType < EnGPUAdapterType::NumGPUAdapterType; adapterType++)
				{
					std::wstring strDesc = tempAdapterDesc.Description;
					if (strDesc.find(m_kGPUAdapterNames[adapterType]) == std::string::npos)
					{
						// 種類の名前が違う、次の種類へ。
						continue;
					}

					if (adaptersByType[adapterType])
					{
						// 同じ種類のアダプタが複数見つかった。
						// ビデオメモリが大きいほうを使用する。

						DXGI_ADAPTER_DESC adaptersDesc;
						adaptersByType[adapterType]->GetDesc(&adaptersDesc);
						if (tempAdapterDesc.DedicatedVideoMemory <= adaptersDesc.DedicatedVideoMemory)
						{
							// ビデオメモリが少ないから、使用しない。
							break;
						}
					}

					adaptersByType[adapterType] = tempAdapter;
						
					break;
					
				}


				// ビデオメモリが一番大きいアダプタを探す
				if (tempAdapterDesc.DedicatedVideoMemory > videoMomorySize)
				{
					maxVideoMemoryAdapter = tempAdapter;
					videoMomorySize = tempAdapterDesc.DedicatedVideoMemory;
				}
			}


			// 2：探し出したアダプタから、どのアダプタを使用するか選ぶ

			bool decided = false;
			// 優先度の高いアダプタから順に調べる
			for (int i = 0; i < EnGPUAdapterType::NumGPUAdapterType; i++)
			{
				if (adaptersByType[i])
				{
					*adapterToUse = adaptersByType[i];
					// 使用するアダプタは、この関数の外で使われるため、
					// この関数の最後に破棄されてしまわないように、参照カウンタを上げておく。
					(*adapterToUse)->AddRef();
					decided = true;
					break;
				}
			}
			// 優先度が高いアダプタがなければ、ビデオメモリが一番大きいアダプタを使用する。
			if (decided != true)
			{
				if (maxVideoMemoryAdapter)
				{
					*adapterToUse = maxVideoMemoryAdapter;
					// 使用するアダプタは、この関数の外で使われるため、
					// この関数の最後に破棄されてしまわないように、参照カウンタを上げておく。
					(*adapterToUse)->AddRef();
				}
			}

			// 3：調べたアダプタすべてを破棄
			// 使用するアダプタは、事前に参照カウンタを上げてあるはず。
			// だから参照カウンタを下げても破棄されないはず。
			for (auto adapter : adapters)
			{
				adapter->Release();
			}

			return;
		}


		bool CGraphicsEngine::CreateCommandAllocator()
		{
			auto result = m_device->CreateCommandAllocator(
				D3D12_COMMAND_LIST_TYPE_DIRECT, 
				IID_PPV_ARGS(&m_commandAllocator)
			);

			if (FAILED(result))
			{
				nsGameWindow::MessageBoxError(L"コマンドアロケータの生成に失敗しました。");
				return false;
			}

			return true;
		}

		bool CGraphicsEngine::CreateCommandList(D3D12_COMMAND_LIST_TYPE commandListType)
		{
			return m_commandList.Init(commandListType);
		}

		bool CGraphicsEngine::CreateCommandQueue(D3D12_COMMAND_LIST_TYPE commandListType)
		{
			D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
			cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			cmdQueueDesc.NodeMask = 0;
			cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
			// コマンドリストと合わせる。
			cmdQueueDesc.Type = commandListType;

			auto result = m_device->CreateCommandQueue(
				&cmdQueueDesc,
				IID_PPV_ARGS(&m_commandQueue)
			);

			if (FAILED(result))
			{
				nsGameWindow::MessageBoxError(L"コマンドキューの生成に失敗しました。");
				return false;
			}

			return true;
		}


		bool CGraphicsEngine::CreateFence()
		{
			auto result = m_device->CreateFence(
				m_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)
			);
			if (FAILED(result))
			{
				nsGameWindow::MessageBoxError(L"フェンスの生成に失敗しました。");
				return false;
			}

			return true;
		}

		bool CGraphicsEngine::CreateMainRenderTarget()
		{
			auto frameBuffResDesc = m_frameBuffer.GetResourceDesc();
			auto res = m_mainRenderTarget.Init(
				static_cast<unsigned int>(frameBuffResDesc.Width),
				frameBuffResDesc.Height,
				frameBuffResDesc.Format,
				m_frameBuffer.GetRtvClearColor(),
				DXGI_FORMAT_UNKNOWN,
				L"MainRenderTarget"
			);

			return res;
		}

		bool CGraphicsEngine::CreateSeceneConstantBuff()
		{
			// 〇定数バッファの作成
			m_sceneDataCB.Init(sizeof(SSceneDataMatrix), L"SceneData");

			// 〇マップされたデータにデータをコピー
			auto mappedSceneData = 
				static_cast<SSceneDataMatrix*>(m_sceneDataCB.GetMappedConstantBuffer());
			mappedSceneData->mView = m_mainCamera.GetViewMatirx();
			mappedSceneData->mProj = m_mainCamera.GetViewMatirx();
			mappedSceneData->cameraPosWS = m_mainCamera.GetPosition();

			// 〇ディスクリプタヒープの作成
			constexpr unsigned int numDescHeaps = 1;
			m_sceneDataDH.InitAsCbvSrvUav(numDescHeaps, L"SceneData");

			// 〇定数バッファビューの作成
			auto heapHandle = m_sceneDataDH.GetCPUHandle();
			m_sceneDataCB.CreateConstantBufferView(heapHandle);

			return true;
		}


		void CGraphicsEngine::WaitForCommandExecutionToComplete()
		{
			m_commandQueue->Signal(m_fence, ++m_fenceVal);
			if (m_fence->GetCompletedValue() != m_fenceVal)
			{
				auto eventH = CreateEvent(nullptr, false, false, nullptr);
				m_fence->SetEventOnCompletion(m_fenceVal, eventH);
				WaitForSingleObject(eventH, INFINITE);
				CloseHandle(eventH);
			}

			return;
		}



	}

}