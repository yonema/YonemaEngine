#include "ShadowModelRenderer.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsShadow
		{
			void CShadowModelRenderer::Draw(nsDx12Wrappers::CCommandList* commandList)
			{
				// モデルごとの定数バッファのセット
				commandList->SetDescriptorHeap(m_modelDH);
				commandList->SetGraphicsRootDescriptorTable(0, m_modelDH);

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

				m_modelDH.Release();
				m_modelCB.Release();

				return;
			}

			void CShadowModelRenderer::Init(
				const std::function<void(nsDx12Wrappers::CCommandList* commandList)>& drawFunc,
				nsRenderers::CRendererTable::EnRendererType baseModelType
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

				CreateModelCBV();

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

				// ディスクリプタヒープ作成
				constexpr unsigned int kNumDescHeaps = 1;
				m_modelDH.InitAsCbvSrvUav(kNumDescHeaps, L"ShadowModelDH");

				// 定数バッファビュー作成
				m_modelCB.CreateConstantBufferView(m_modelDH.GetCPUHandle());
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



		}
	}
}