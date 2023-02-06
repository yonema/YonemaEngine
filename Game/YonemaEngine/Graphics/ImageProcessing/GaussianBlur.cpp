#include "GaussianBlur.h"
#include "../GraphicsEngine.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsImageProcessing
		{
			// ガウシアンブラーをかけるとき、頂点シェーダーでもピクセルシェーダーでも
			// リソースにアクセスするためALLを指定。
			const D3D12_RESOURCE_STATES CGaussianBlur::m_kXBlurResourceState =
				D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;

			CGaussianBlur::~CGaussianBlur()
			{
				Terminate();

				return;
			}

			void CGaussianBlur::Terminate()
			{
				Release();

				return;
			}


			void CGaussianBlur::Release()
			{
				m_XBlurCB.Release();
				m_YBlurCB.Release();
				m_xBlurRenderTarget.Release();
				m_yBlurRenderTarget.Release();
				m_xBlurSprite.Release();
				m_yBlurSprite.Release();

				return;
			}


			void CGaussianBlur::Init(nsDx12Wrappers::CTexture* originalTexture, float blurPower)
			{
				m_originalTexture = originalTexture;

				UpdateWeightsTable(blurPower);

				InitRenderTargets();

				InitCBV();

				InitSprites();

				return;
			}

			void CGaussianBlur::ExecuteOnGPU(nsDx12Wrappers::CCommandList* commandList)
			{
				//// Xブラーを実行 ////

				// 描画先を設定
				commandList->TransitionResourceState(
					m_xBlurRenderTarget,
					m_kXBlurResourceState,
					D3D12_RESOURCE_STATE_RENDER_TARGET
				);
				commandList->SetRenderTarget(m_xBlurRenderTarget);

				// ビューポートとシザリング矩形を設定
				commandList->SetViewportAndScissorRect(m_xBlurRenderTarget);

				// インデックスバッファを指定していないため、TRIANGLELISTではなく、
				// TRIANGLESTRIPを指定する。
				commandList->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

				using EnRendererType = nsRenderers::CRendererTable::EnRendererType;
				// ルートシグネチャとパイプラインステートの設定
				auto* rendererTable = CGraphicsEngine::GetInstance()->GetRendererTable();
				commandList->SetGraphicsRootSignatureAndPipelineState(
					rendererTable->GetRootSignature(EnRendererType::enGaussianBlurXForShadowMap),
					rendererTable->GetPipelineState(EnRendererType::enGaussianBlurXForShadowMap)
				);

				// Xブラー実行
				m_xBlurSprite.Draw(commandList);

				// Xブラー終了
				commandList->TransitionResourceState(
					m_xBlurRenderTarget,
					D3D12_RESOURCE_STATE_RENDER_TARGET,
					m_kXBlurResourceState
				);


				//// Yブラー実行 ////

				// 描画先を設定
				commandList->TransitionFromShaderResourceToRenderTarget(m_yBlurRenderTarget);
				commandList->SetRenderTarget(m_yBlurRenderTarget);

				// ビューポートとシザリング矩形を設定
				commandList->SetViewportAndScissorRect(m_yBlurRenderTarget);

				// インデックスバッファを指定していないため、TRIANGLELISTではなく、
				// TRIANGLESTRIPを指定する。
				// さっき指定したからやらなくてもいいはず。
				//commandList->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

				// ルートシグネチャとパイプラインステートの設定
				rendererTable = CGraphicsEngine::GetInstance()->GetRendererTable();
				commandList->SetGraphicsRootSignatureAndPipelineState(
					rendererTable->GetRootSignature(EnRendererType::enGaussianBlurYForShadowMap),
					rendererTable->GetPipelineState(EnRendererType::enGaussianBlurYForShadowMap)
				);

				// Yブラー実行
				m_yBlurSprite.Draw(commandList);


				// Yブラー終了
				commandList->TransitionFromRenderTargetToShaderResource(m_yBlurRenderTarget);



				return;
			}

			void CGaussianBlur::InitRenderTargets()
			{
				const auto& texSize = m_originalTexture->GetTextureSize();
				//Xブラー用のレンダリングターゲットを作成する。
				m_xBlurRenderTarget.Init(
					static_cast<unsigned int>(texSize.x / 2.0f),	// x軸方向にダウンサンプリング
					static_cast<unsigned int>(texSize.y),
					m_originalTexture->GetFormat(),
					nsMath::CVector4::Black(),
					DXGI_FORMAT_UNKNOWN,
					m_kXBlurResourceState,
					L"XBlur"
				);

				//Yブラー用のレンダリングターゲットを作成する。
				m_yBlurRenderTarget.Init(
					static_cast<unsigned int>(texSize.x / 2.0f),	// x軸方向と
					static_cast<unsigned int>(texSize.y / 2.0f),	// y軸方向両方にダウンサンプリング
					m_originalTexture->GetFormat(),
					nsMath::CVector4::Black(),
					DXGI_FORMAT_UNKNOWN,
					D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
					L"YBlur"
				);

				return;
			}

			void CGaussianBlur::InitSprites()
			{
				//Xブラー用のスプライトを初期化する。
				{
					nsSprites::SSpriteInitData initData;
					//スプライトの解像度はm_xBlurRenderTargetと同じ。
					initData.spriteSize.x = static_cast<float>(m_xBlurRenderTarget.GetWidth());
					initData.spriteSize.y = static_cast<float>(m_xBlurRenderTarget.GetHeight());
					initData.texture = m_originalTexture;
					//描き込むレンダリングターゲットのフォーマットを指定する。
					initData.colorFormat = m_xBlurRenderTarget.Get()->GetDesc().Format;
					initData.pExpandConstantBuffer = &m_XBlurCB;

					m_xBlurSprite.Init(initData);
				}

				//Yブラー用のスプライトを初期化する。
				{
					nsSprites::SSpriteInitData initData;
					//スプライトの解像度はm_yBlurRenderTargetと同じ。
					initData.spriteSize.x = static_cast<float>(m_yBlurRenderTarget.GetWidth());
					initData.spriteSize.y = static_cast<float>(m_yBlurRenderTarget.GetHeight());
					//テクスチャは横ブラーをかけたもの。
					initData.texture = m_xBlurRenderTarget.GetRenderTargetTexture();
					//描き込むレンダリングターゲットのフォーマットを指定する。
					initData.colorFormat = m_yBlurRenderTarget.Get()->GetDesc().Format;
					initData.pExpandConstantBuffer = &m_YBlurCB;

					m_yBlurSprite.Init(initData);
				}

				return;
			}

			void CGaussianBlur::InitCBV()
			{
				m_XBlurCB.Init(static_cast<unsigned int>(sizeof(m_weights)), L"GaussianBlurCB");

				m_XBlurCB.CopyToMappedConstantBuffer(m_weights, sizeof(m_weights));

				m_YBlurCB.Init(static_cast<unsigned int>(sizeof(m_weights)), L"GaussianBlurCB");

				m_YBlurCB.CopyToMappedConstantBuffer(m_weights, sizeof(m_weights));

				return;
			}


			void CGaussianBlur::UpdateWeightsTable(float blurPower)
			{
				float total = 0;
				for (int i = 0; i < m_kNumWeights; i++)
				{
					m_weights[i] = expf(-0.5f * (float)(i * i) / blurPower);
					total += 2.0f * m_weights[i];

				}

				// 規格化
				for (int i = 0; i < m_kNumWeights; i++)
				{
					m_weights[i] /= total;
				}

				return;
			}



		}
	}
}