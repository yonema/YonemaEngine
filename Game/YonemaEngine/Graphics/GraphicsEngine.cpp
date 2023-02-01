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
			// �ォ��g�p�D��x��
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

			// �f�o�C�X��������炷���ɐݒ�B
			m_descriptorSizeOfCbvSrvUav = m_device->GetDescriptorHandleIncrementSize(
				D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			m_descriptorSizeOfRtv = m_device->GetDescriptorHandleIncrementSize(
				D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

			if (CreateCommandAllocator() != true)
			{
				return false;
			}
			{
				// �R�}���h���X�g�ƃR�}���h�L���[�œ����^�C�v���g���B
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

			// ���������I�������ADXGIFactory�͂�������Ȃ����ߔj������B
			dxgiFactory->Release();

			m_fontEngine = nsFonts::CFontEngine::CreateInstance();

			// ��{�ƂȂ郌���_�[�^�[�Q�b�g�X�v���C�g�ɐݒ肷��
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
			// �j������O�ɁA�R�}���h�̎��s�̊�����҂B
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
			// �`��J�n�����B�X�V��������A�`�揈�����O�ɌĂԂ��ƁB
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
			// �����Z�b�g
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
			// �[�x�o�b�t�@�̓t���[���o�b�t�@�Ɠ������̂��g�p�B
			auto dsvH = m_frameBuffer.GetDsvCpuDescriptorHandle();
			const nsDx12Wrappers::CRenderTarget* rtArray[] = { &m_mainRenderTarget };

			// �`����ݒ�
			m_commandList.TransitionFromShaderResourceToRenderTarget(m_mainRenderTarget);
			m_commandList.SetRenderTargets(1, rtArray, dsvH);

			// ��ʃN���A
			m_commandList.ClearRenderTargetViews(1, rtArray);
			m_commandList.ClearDepthStencilView(dsvH);

			// �r���[�|�[�g�ƃV�U�����O��`�̓t���[���o�b�t�@�Ɠ������̂��g�p�B
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



			// �`��I��
			m_commandList.TransitionFromRenderTargetToShaderResource(m_mainRenderTarget);


			return;
		}


		void CGraphicsEngine::DrawWithSimplePostEffect()
		{
			// �`����ݒ�
			m_commandList.TransitionFromShaderResourceToRenderTarget(m_simplePostEffectRenderTarget);
			m_commandList.SetRenderTarget(m_simplePostEffectRenderTarget);

			// ��ʃN���A
			m_commandList.ClearRenderTargetAndDepthStencilView(m_simplePostEffectRenderTarget);

			// �C���f�b�N�X�o�b�t�@���w�肵�Ă��Ȃ����߁ATRIANGLELIST�ł͂Ȃ��A
			// TRIANGLESTRIP���w�肷��B
			m_commandList.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

			// �V���v���ȃ|�X�g�G�t�F�N�g�̕`��ݒ�
			m_commandList.SetGraphicsRootSignatureAndPipelineState(
				m_rendererTable.GetRootSignature(
					RendererType::enSimplePostEffect),
				m_rendererTable.GetPipelineState(
					RendererType::enSimplePostEffect)
			);
			// �V���v���ȃ|�X�g�G�t�F�N�g�������ĕ`��
			for (auto renderer : m_rendererTable.GetRendererList(RendererType::enSimplePostEffect))
			{
				renderer->DrawWrapper(&m_commandList);
			}

			// �`��I��
			m_commandList.TransitionFromRenderTargetToShaderResource(m_simplePostEffectRenderTarget);

			return;
		}

		void CGraphicsEngine::DrawCollision()
		{
#ifdef _DEBUG
			// �[�x�o�b�t�@�̓t���[���o�b�t�@�Ɠ������̂��g�p�B
			auto dsvH = m_frameBuffer.GetDsvCpuDescriptorHandle();
			const nsDx12Wrappers::CRenderTarget* rtArray[] = { &m_simplePostEffectRenderTarget };

			// �`����ݒ�
			m_commandList.TransitionFromShaderResourceToRenderTarget(m_simplePostEffectRenderTarget);
			m_commandList.SetRenderTargets(1, rtArray, dsvH);

			// ��ʃN���A
			//m_commandList.ClearRenderTargetViews(1, rtArray);

			// �r���[�|�[�g�ƃV�U�����O��`�̓t���[���o�b�t�@�Ɠ������̂��g�p�B
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

			// �`��I��
			m_commandList.TransitionFromRenderTargetToPresent(m_simplePostEffectRenderTarget);
#endif // _DEBUG
			return;
		}


		void CGraphicsEngine::Draw2D()
		{
			// �t���[���o�b�t�@�ɏ������ޑO�ɁA�o�b�N�o�b�t�@�ƃt�����g�o�b�t�@�����ւ���B
			m_frameBuffer.SwapBackBuffer();

			// �`����ݒ�
			m_commandList.TransitionFromPresentToRenderTarget(m_frameBuffer);
			m_commandList.SetRenderTarget(m_frameBuffer);

			// ��ʃN���A
			m_commandList.ClearRenderTargetAndDepthStencilView(m_frameBuffer);

			// �C���f�b�N�X�o�b�t�@���w�肵�Ă��Ȃ����߁ATRIANGLELIST�ł͂Ȃ��A
			// TRIANGLESTRIP���w�肷��B
			m_commandList.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);


			// �s�����X�v���C�g�̕`��ݒ�
			m_commandList.SetGraphicsRootSignatureAndPipelineState(
				m_rendererTable.GetRootSignature(
					RendererType::enSprite),
				m_rendererTable.GetPipelineState(
					RendererType::enSprite)
			);

			// �܂���{�̃����_�[�^�[�Q�b�g�̃X�v���C�g�`��
			m_pBaseRenderTargetSprite->Draw(&m_commandList);

			// �s�����X�v���C�g�`��
			for (auto renderer : m_rendererTable.GetRendererList(RendererType::enSprite))
			{
				renderer->DrawWrapper(&m_commandList);
			}

			// �������X�v���C�g�̕`��ݒ�
			m_commandList.SetGraphicsRootSignatureAndPipelineState(
				m_rendererTable.GetRootSignature(
					RendererType::enTransSprite),
				m_rendererTable.GetPipelineState(
					RendererType::enTransSprite)
			);
			// �������X�v���C�g�`��
			for (auto renderer : m_rendererTable.GetRendererList(RendererType::enTransSprite))
			{
				renderer->DrawWrapper(&m_commandList);
			}

			// �`��I��
			m_commandList.TransitionFromRenderTargetToPresent(m_frameBuffer);

			return;
		}

		void CGraphicsEngine::EndDraw()
		{
			// ���߂̎��s�O�ɁA�K���R�}���h���X�g�̃N���[�Y���s���B
			m_commandList.Close();

			// �R�}���h���s
			ID3D12CommandList* commandLists[] = { m_commandList.Get() };
			m_commandQueue->ExecuteCommandLists(1, commandLists);

			// GPU��CPU����
			WaitForCommandExecutionToComplete();

			// �t���b�v
			m_frameBuffer.Present();

			// �t���b�v��ɃR�~�b�g
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
				nsGameWindow::MessageBoxError(L"DXGIFactory�I�u�W�F�N�g�̐����Ɏ��s���܂����B");
				return false;
			}

			return true;
		}

		void CGraphicsEngine::EnableDebugLayer()
		{
			ID3D12Debug* debugLayer = nullptr;
			auto result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));

			// �f�o�b�N���C���[��L���ɂ���B
			debugLayer->EnableDebugLayer();
			// �L���ɂ�����A�C���^�[�t�F�[�X���J������B
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
					// �����\�ȃo�[�W��������������A���[�v��ł��؂�B
					break;
				}
			}

			// �f�o�C�X�������I�������A�A�_�v�^�͂����g�p���Ȃ����߁A�j���B
			if (adapter)
			{
				adapter->Release();
			}

			if (m_device == nullptr)
			{
				nsGameWindow::MessageBoxError(L"�f�o�C�X�̐����Ɏ��s���܂����B");
				return false;
			}

			return true;
		}

		void CGraphicsEngine::FindAdapter(IDXGIFactory6* dxgiFactory, IDXGIAdapter** adapterToUse)
		{
			// �Ō�ɂ܂Ƃ߂Ĕj�����邽�߂ɁA���ׂ��A�_�v�^���ׂĂ�ێ����Ă������X�g�B
			std::list<IDXGIAdapter*> adapters;
			// ���ׂ�Ώۂ̃A�_�v�^�B
			IDXGIAdapter* tempAdapter = nullptr;
			// GPU�̎�ޕʂ̃A�_�v�^�B
			// ������ނ̃A�_�v�^���������ꍇ�́A�r�f�I���������傫������ێ��B
			IDXGIAdapter* adaptersByType[EnGPUAdapterType::NumGPUAdapterType] = { nullptr };
			// ���ׂẴA�_�v�^�̒��ŁA1�ԃr�f�I���������傫���A�_�v�^��ێ��B
			IDXGIAdapter* maxVideoMemoryAdapter = nullptr;
			SIZE_T videoMomorySize = 0;

			// 1�F���p�\�ȃA�_�v�^��T��

			// PC�ɂ���A�_�v�^�����ꂼ�꒲�ׂ�
			for (int i = 0; dxgiFactory->EnumAdapters(i, &tempAdapter) != DXGI_ERROR_NOT_FOUND; i++)
			{
				adapters.emplace_back(tempAdapter);
				DXGI_ADAPTER_DESC tempAdapterDesc = {};
				tempAdapter->GetDesc(&tempAdapterDesc);

				// �D��x�̍����A�_�v�^�����邩���ׂ�
				for (int adapterType = 0; adapterType < EnGPUAdapterType::NumGPUAdapterType; adapterType++)
				{
					std::wstring strDesc = tempAdapterDesc.Description;
					if (strDesc.find(m_kGPUAdapterNames[adapterType]) == std::string::npos)
					{
						// ��ނ̖��O���Ⴄ�A���̎�ނցB
						continue;
					}

					if (adaptersByType[adapterType])
					{
						// ������ނ̃A�_�v�^���������������B
						// �r�f�I���������傫���ق����g�p����B

						DXGI_ADAPTER_DESC adaptersDesc;
						adaptersByType[adapterType]->GetDesc(&adaptersDesc);
						if (tempAdapterDesc.DedicatedVideoMemory <= adaptersDesc.DedicatedVideoMemory)
						{
							// �r�f�I�����������Ȃ�����A�g�p���Ȃ��B
							break;
						}
					}

					adaptersByType[adapterType] = tempAdapter;
						
					break;
					
				}


				// �r�f�I����������ԑ傫���A�_�v�^��T��
				if (tempAdapterDesc.DedicatedVideoMemory > videoMomorySize)
				{
					maxVideoMemoryAdapter = tempAdapter;
					videoMomorySize = tempAdapterDesc.DedicatedVideoMemory;
				}
			}


			// 2�F�T���o�����A�_�v�^����A�ǂ̃A�_�v�^���g�p���邩�I��

			bool decided = false;
			// �D��x�̍����A�_�v�^���珇�ɒ��ׂ�
			for (int i = 0; i < EnGPUAdapterType::NumGPUAdapterType; i++)
			{
				if (adaptersByType[i])
				{
					*adapterToUse = adaptersByType[i];
					// �g�p����A�_�v�^�́A���̊֐��̊O�Ŏg���邽�߁A
					// ���̊֐��̍Ō�ɔj������Ă��܂�Ȃ��悤�ɁA�Q�ƃJ�E���^���グ�Ă����B
					(*adapterToUse)->AddRef();
					decided = true;
					break;
				}
			}
			// �D��x�������A�_�v�^���Ȃ���΁A�r�f�I����������ԑ傫���A�_�v�^���g�p����B
			if (decided != true)
			{
				if (maxVideoMemoryAdapter)
				{
					*adapterToUse = maxVideoMemoryAdapter;
					// �g�p����A�_�v�^�́A���̊֐��̊O�Ŏg���邽�߁A
					// ���̊֐��̍Ō�ɔj������Ă��܂�Ȃ��悤�ɁA�Q�ƃJ�E���^���グ�Ă����B
					(*adapterToUse)->AddRef();
				}
			}

			// 3�F���ׂ��A�_�v�^���ׂĂ�j��
			// �g�p����A�_�v�^�́A���O�ɎQ�ƃJ�E���^���グ�Ă���͂��B
			// ������Q�ƃJ�E���^�������Ă��j������Ȃ��͂��B
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
				nsGameWindow::MessageBoxError(L"�R�}���h�A���P�[�^�̐����Ɏ��s���܂����B");
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
			// �R�}���h���X�g�ƍ��킹��B
			cmdQueueDesc.Type = commandListType;

			auto result = m_device->CreateCommandQueue(
				&cmdQueueDesc,
				IID_PPV_ARGS(&m_commandQueue)
			);

			if (FAILED(result))
			{
				nsGameWindow::MessageBoxError(L"�R�}���h�L���[�̐����Ɏ��s���܂����B");
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
				nsGameWindow::MessageBoxError(L"�t�F���X�̐����Ɏ��s���܂����B");
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
			// �Z�萔�o�b�t�@�̍쐬
			m_sceneDataCB.Init(sizeof(SSceneDataMatrix), L"SceneData");

			// �Z�}�b�v���ꂽ�f�[�^�Ƀf�[�^���R�s�[
			auto mappedSceneData = 
				static_cast<SSceneDataMatrix*>(m_sceneDataCB.GetMappedConstantBuffer());
			mappedSceneData->mView = m_mainCamera.GetViewMatirx();
			mappedSceneData->mProj = m_mainCamera.GetViewMatirx();
			mappedSceneData->cameraPosWS = m_mainCamera.GetPosition();

			// �Z�f�B�X�N���v�^�q�[�v�̍쐬
			constexpr unsigned int numDescHeaps = 1;
			m_sceneDataDH.InitAsCbvSrvUav(numDescHeaps, L"SceneData");

			// �Z�萔�o�b�t�@�r���[�̍쐬
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