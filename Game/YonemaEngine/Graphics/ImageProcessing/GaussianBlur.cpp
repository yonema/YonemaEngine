#include "GaussianBlur.h"
#include "../GraphicsEngine.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsImageProcessing
		{
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
				m_descHeap.Release();
				m_constantBuffer.Release();
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

				InitSprites();

				InitCBV();

				return;
			}

			void CGaussianBlur::ExecuteOnGPU(nsDx12Wrappers::CCommandList* commandList)
			{
				//// Xブラーを実行 ////

				// 描画先を設定
				commandList->TransitionFromShaderResourceToRenderTarget(m_xBlurRenderTarget);
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

				// ブラー用の定数バッファを設定
				commandList->SetDescriptorHeap(m_descHeap);
				commandList->SetGraphicsRootDescriptorTable(2, m_descHeap);

				// Xブラー実行
				m_xBlurSprite.Draw(commandList);

				// Xブラー終了
				commandList->TransitionFromRenderTargetToShaderResource(m_xBlurRenderTarget);


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

				// ブラー用の定数バッファを設定
				commandList->SetDescriptorHeap(m_descHeap);
				commandList->SetGraphicsRootDescriptorTable(2, m_descHeap);

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
					L"XBlur"
				);

				//Yブラー用のレンダリングターゲットを作成する。
				m_yBlurRenderTarget.Init(
					static_cast<unsigned int>(texSize.x / 2.0f),	// x軸方向と
					static_cast<unsigned int>(texSize.y / 2.0f),	// y軸方向両方にダウンサンプリング
					m_originalTexture->GetFormat(),
					nsMath::CVector4::Black(),
					DXGI_FORMAT_UNKNOWN,
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

					m_yBlurSprite.Init(initData);
				}

				return;
			}

			void CGaussianBlur::InitCBV()
			{
				m_constantBuffer.Init(static_cast<unsigned int>(sizeof(m_weights)), L"GaussianBlurCB");

				m_constantBuffer.CopyToMappedConstantBuffer(m_weights, sizeof(m_weights));

				m_descHeap.InitAsCbvSrvUav(1, L"GaussianBlueDH");

				m_constantBuffer.CreateConstantBufferView(m_descHeap.GetCPUHandle());

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