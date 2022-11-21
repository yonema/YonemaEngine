#include "SpriteFontWrapper.h"
#include <SpriteFont.h>
#include <ResourceUploadBatch.h>
#include "../GraphicsEngine.h"
#include "FontRenderer.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsFonts
		{
			nsMath::CVector2 CSpriteFontWrapper::m_frameBufferHalfSize = { -1.0f,-1.0f };

			CSpriteFontWrapper::~CSpriteFontWrapper()
			{
				Terminate();
				return;
			}

			void CSpriteFontWrapper::Terminate()
			{
				Release();
				return;
			}

			void CSpriteFontWrapper::Release()
			{
				m_srvDescHeap.Release();
				if (m_spriteFont)
				{
					delete m_spriteFont;
				}
				if (m_spriteBatch)
				{
					delete m_spriteBatch;
				}
				return;
			}


			bool CSpriteFontWrapper::Init(const wchar_t* filePath)
			{

				auto* device = CGraphicsEngine::GetInstance()->GetDevice();

				const auto& viewport = CGraphicsEngine::GetInstance()->GetFrameBuffer()->GetViewport();
				auto* commandQueue = CGraphicsEngine::GetInstance()->GetCommandQueue();

				m_srvDescHeap.InitAsCbvSrvUav(1, L"SpriteFontSRV");

				DirectX::ResourceUploadBatch resUploadBatch(device);
				resUploadBatch.Begin();

				DirectX::RenderTargetState sprBatchRTState(
					DXGI_FORMAT_R8G8B8A8_UNORM,
					DXGI_FORMAT_D32_FLOAT);
				DirectX::SpriteBatchPipelineStateDescription sprBatPiplineStateDesc(sprBatchRTState);

				m_spriteBatch = new DirectX::SpriteBatch(
					device,
					resUploadBatch,
					sprBatPiplineStateDesc,
					&viewport
				);

				m_spriteFont = new DirectX::SpriteFont(
					device,
					resUploadBatch,
					filePath,
					m_srvDescHeap.GetCPUHandle(),
					m_srvDescHeap.GetGPUHandle()
				);

				resUploadBatch.End(commandQueue);

				// 未初期化なら初期化する
				if (m_frameBufferHalfSize.x <= 0.0f)
				{
					float width = viewport.Width - viewport.TopLeftX;
					float height = viewport.Height - viewport.TopLeftY;
					m_frameBufferHalfSize = { width, height };
					m_frameBufferHalfSize.Scale(0.5f);
				}

				return true;
			}

			void CSpriteFontWrapper::BeginDraw(nsDx12Wrappers::CCommandList* commandList)
			{
				m_spriteBatch->Begin(
					commandList->Get(),
					DirectX::SpriteSortMode_Deferred,
					nsMath::CMatrix::Identity()
				);

				commandList->SetDescriptorHeap(m_srvDescHeap);

				return;
			}

			void CSpriteFontWrapper::Draw(const CFontRenderer::SFontParameter& fontParam) const
			{
				// positionをそのまま座標にすると、
				// 左上に表示。
				// 座標系が、YDown、XRightになる。これはあっているのでそのまま。

				nsMath::CVector2 finalPos;
				CalcPositionFromAnchar(fontParam, &finalPos);

				nsMath::CVector2 finalPivot;
				CalcPivot(fontParam, &finalPivot);


				m_spriteFont->DrawString(
					m_spriteBatch,
					fontParam.text,
					finalPos.m_xmf2Vec,
					fontParam.color,
					fontParam.rotation,
					finalPivot.m_xmf2Vec,
					fontParam.scale
				);

				return;
			}

			void CSpriteFontWrapper::EndDraw()
			{
				m_spriteBatch->End();

				return;
			}

			void CSpriteFontWrapper::CalcPositionFromAnchar(
				const CFontRenderer::SFontParameter& fontParam, nsMath::CVector2* pPos) const
			{
				// まずはY座標のアンカーに合わせる。
				switch (fontParam.anchor)
				{
				case EnAnchors::enTopLeft:
				case EnAnchors::enTopCenter:
				case EnAnchors::enTopRight:
					pPos->y = fontParam.position.y;
					break;
				case EnAnchors::enMiddleLeft:
				case EnAnchors::enMiddleCenter:
				case EnAnchors::enMiddleRight:
				default:
					pPos->y = fontParam.position.y + m_frameBufferHalfSize.y;
					break;
				case EnAnchors::enBottonLeft:
				case EnAnchors::enBottonCenter:
				case EnAnchors::enBottonRight:
					pPos->y = fontParam.position.y + m_frameBufferHalfSize.y * 2.0f;
					break;
				}
				//次にX座標のアンカーに合わせる。
				switch (fontParam.anchor)
				{
				case EnAnchors::enTopLeft:
				case EnAnchors::enMiddleLeft:
				case EnAnchors::enBottonLeft:
					pPos->x = fontParam.position.x;
					break;
				case EnAnchors::enTopCenter:
				case EnAnchors::enMiddleCenter:
				case EnAnchors::enBottonCenter:
				default:
					pPos->x = fontParam.position.x + m_frameBufferHalfSize.x;
					break;
				case EnAnchors::enTopRight:
				case EnAnchors::enMiddleRight:
				case EnAnchors::enBottonRight:
					pPos->x = fontParam.position.x + m_frameBufferHalfSize.x * 2.0f;
					break;
				}

				return;
			}

			void CSpriteFontWrapper::CalcPivot(
				const CFontRenderer::SFontParameter& fontParam, nsMath::CVector2* pPivot) const
			{
				auto stringSizeInFont = m_spriteFont->MeasureString(fontParam.text);
				pPivot->x = DirectX::XMVectorGetX(stringSizeInFont) * fontParam.pivot.x;
				pPivot->y = DirectX::XMVectorGetY(stringSizeInFont) * fontParam.pivot.y;

				return;
			}




		}
	}
}