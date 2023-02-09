#include "ShadowModelRenderer.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsShadow
		{
			void CShadowModelRenderer::Draw(nsDx12Wrappers::CCommandList* commandList)
			{
				if (m_drawingFlag != true)
				{
					return;
				}

				// ディスクリプタヒープと各種バッファをセット
				commandList->SetDescriptorHeap(m_descriptorHeap);

				// モデルごとの定数バッファをセット
				auto handle = m_descHandle.GetGpuHandle(
					static_cast<unsigned int>(EnDescHeapLayout::enModelCB));
				commandList->SetGraphicsRootDescriptorTable(0, handle);

				// スケルたタルアニメーションが有効なら、ボーン行列の配列をセット
				// インスタンシングが有効なら、ワールド行列の配列をセット
				// ボーン行列の配列とワールド行列の配列は、一緒のディスクリプタテーブルにある。
				if (m_isSkeltalAnimation || m_isInstancing)
				{
					handle = m_descHandle.GetGpuHandle(
						static_cast<unsigned int>(EnDescHeapLayout::enBoneMatrixArraySRV));
					commandList->SetGraphicsRootDescriptorTable(1, handle);
				}


				m_drawFunc(commandList);

				return;
			}

			CShadowModelRenderer::~CShadowModelRenderer()
			{
				Terminate();

				return;
			}

			void CShadowModelRenderer::Terminate()
			{
				Release();
				return;
			}

			void CShadowModelRenderer::Release()
			{
				DisableDrawing();

				m_worldMatrixArraySB.Release();
				m_boneMatrixArraySB.Release();
				m_modelCB.Release();

				return;
			}

			void CShadowModelRenderer::Init(
				const std::function<void(nsDx12Wrappers::CCommandList* commandList)>& drawFunc,
				nsRenderers::CRendererTable::EnRendererType baseModelType,
				const std::vector<nsMath::CMatrix>* pBoneMatrixArray,
				unsigned int maxInstance
			)
			{
				// シャドウマップ確認用スプライト
//#define NONE
#ifdef NONE
				static int a = 0;
				if (a == 0)
				{
					auto* shadowMapRenderer =
						CGraphicsEngine::GetInstance()->GetShadowMapRenderer();
					SSpriteInitData init;
					//init.texture = shadowMapRenderer->GetShadowMapSprite()->GetTexture();
					init.texture = shadowMapRenderer->GetShadowBokeTexture();
					init.spriteSize = { 300.0f,300.0f };
					//const auto& desc = shadowMapSprite->GetTexture()->GetResource()->GetDesc();
					//init.spriteSize = 
					//{ static_cast<float>(desc.Width) , static_cast<float>(desc.Height) };
					auto* spriteRenderer = NewGO<CSpriteRenderer>();
					spriteRenderer->Init(init);
					spriteRenderer->SetAnchor(EnAnchors::enTopRight);
					spriteRenderer->SetPivot({ 1.0f,0.0f });
					spriteRenderer->SetPosition({ 0.0f,0.0f });
				
					a++;
				}
#endif

				m_drawFunc = drawFunc;

				constexpr auto kNumDescHeaps = static_cast<unsigned int>(EnDescHeapLayout::enNum);
				m_descriptorHeap.InitAsCbvSrvUav(kNumDescHeaps, L"ShadowModelDescHeap");
				m_descHandle.Init(
					kNumDescHeaps,
					m_descriptorHeap.GetCPUHandle(),
					m_descriptorHeap.GetGPUHandle()
				);

				CreateModelCBV();
				CreateBoneMatrixArraySB(pBoneMatrixArray);
				CreateWorldMatrixArraySB(maxInstance);


				using EnRendererType = nsRenderers::CRendererTable::EnRendererType;

				switch (baseModelType)
				{
				case EnRendererType::enBasicModel:
					SetRenderType(EnRendererType::enShadowModel);
					break;
				case EnRendererType::enBasicNonCullingModel:
					SetRenderType(EnRendererType::enShadowModel);
					break;
				case EnRendererType::enSkinModel:
					SetRenderType(EnRendererType::enShadowSkinModel);
					break;
				case EnRendererType::enSkinNonCullingModel:
					SetRenderType(EnRendererType::enShadowSkinModel);
					break;
				case EnRendererType::enInstancingModel:
					SetRenderType(EnRendererType::enShadowInstancingModel);
					break;
				case EnRendererType::enInstancingNonCullingModel:
					SetRenderType(EnRendererType::enShadowInstancingModel);
					break;
				default:
					SetRenderType(EnRendererType::enShadowModel);
					break;
				}



				EnableDrawing();

				return;
			}

			void CShadowModelRenderer::CreateModelCBV()
			{
				// 定数バッファ作成

				auto cbSize = sizeof(SConstantBufferData);

				m_modelCB.Init(static_cast<unsigned int>(cbSize), L"ShadowModelCB");

				const auto& mWorld = nsMath::CMatrix::Identity();
				const auto& shadowCamera = 
					CGraphicsEngine::GetInstance()->GetShadowMapRenderer()->GetCamera();
				const auto& mLightViewProj = shadowCamera->GetViewProjectionMatirx();
				auto* mappedCB =
					static_cast<SConstantBufferData*>(m_modelCB.GetMappedConstantBuffer());
				mappedCB->mWorld = mWorld;
				mappedCB->mLightViewProj = mLightViewProj;
				mappedCB->lightPos = shadowCamera->GetPosition();

				auto handle = m_descHandle.GetCpuHandle(
					static_cast<unsigned int>(EnDescHeapLayout::enModelCB));
				// 定数バッファビュー作成
				m_modelCB.CreateConstantBufferView(handle);
			}

			void CShadowModelRenderer::CreateBoneMatrixArraySB(
				const std::vector<nsMath::CMatrix>* pBoneMatrixArray)
			{
				unsigned int size = 1;
				unsigned int num = 1;

				if (pBoneMatrixArray != nullptr && pBoneMatrixArray->empty() != true)
				{
					size = static_cast<unsigned int>(sizeof(nsMath::CMatrix));
					num = static_cast<unsigned int>(pBoneMatrixArray->size());
					m_isSkeltalAnimation = true;
				}

				bool res =
					m_boneMatrixArraySB.Init(size, num);

				if (res != true)
				{
					nsGameWindow::MessageBoxError(L"m_boneMatrixArraySBの生成に失敗しました。");
					return;
				}

				auto handle = m_descHandle.GetCpuHandle(
					static_cast<unsigned int>(EnDescHeapLayout::enBoneMatrixArraySRV));
				m_boneMatrixArraySB.RegistShaderResourceView(handle);


				return;
			}

			void CShadowModelRenderer::CreateWorldMatrixArraySB(unsigned int maxInstance)
			{
				unsigned int size = 1;
				unsigned int num = 1;

				if (maxInstance > 1)
				{
					size = static_cast<unsigned int>(sizeof(nsMath::CMatrix));
					num = maxInstance;
					m_isInstancing = true;
				}

				bool res =
					m_worldMatrixArraySB.Init(size, num);

				if (res != true)
				{
					nsGameWindow::MessageBoxError(L"m_worldMatrixArraySBの生成に失敗しました。");
				}

				auto handle = m_descHandle.GetCpuHandle(
					static_cast<unsigned int>(EnDescHeapLayout::enWorldMatrixArraySRV));
				m_worldMatrixArraySB.RegistShaderResourceView(handle);


				return;
			}


			void CShadowModelRenderer::Update(const nsMath::CMatrix& worldMatrix)
			{

				// 定数バッファにコピー。
				auto mappedCB =
					static_cast<SConstantBufferData*>(m_modelCB.GetMappedConstantBuffer());
				mappedCB->mWorld = worldMatrix;
				const auto& shadowCamera =
					CGraphicsEngine::GetInstance()->GetShadowMapRenderer()->GetCamera();
				const auto& mLightViewProj = shadowCamera->GetViewProjectionMatirx();
				mappedCB->mLightViewProj = mLightViewProj;
				mappedCB->lightPos = shadowCamera->GetPosition();

				return;
			}

			void CShadowModelRenderer::UpdateBoneMatrixArray(
				const std::vector<nsMath::CMatrix>* pBoneMatrixArray) noexcept
			{
				if (pBoneMatrixArray == nullptr || m_isSkeltalAnimation != true)
				{
					return;
				}

				m_boneMatrixArraySB.CopyToMappedStructuredBuffer(pBoneMatrixArray->data());

				return;
			}

			void CShadowModelRenderer::UpdateWorldMatrixArray(
				const std::vector<nsMath::CMatrix>* pWrldMatrixArray,
				unsigned int fixNumInstanceOnFrame
			) noexcept
			{
				if (pWrldMatrixArray == nullptr || m_isInstancing != true)
				{
					return;
				}

				m_worldMatrixArraySB.CopyToMappedStructuredBuffer(
					pWrldMatrixArray->data(),
					sizeof(nsMath::CMatrix) * fixNumInstanceOnFrame);

				return;
			}



		}
	}
}