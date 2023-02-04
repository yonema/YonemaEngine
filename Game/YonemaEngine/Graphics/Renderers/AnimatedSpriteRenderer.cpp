#include "YonemaEnginePreCompile.h"
#include "AnimatedSpriteRenderer.h"
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsRenderers
		{
			bool CAnimatedSpriteRenderer::Start()
			{
				return true;
			}

			void CAnimatedSpriteRenderer::Update(float deltaTime)
			{
				if (m_animations2D.count(m_playingAnimationName) == 1 && m_animations2D.at(m_playingAnimationName).IsPlaying())
				{
					m_animations2D.at(m_playingAnimationName).Update();
					nsGraphics::nsAnimations2D::SAnimation2DFrameData frameData = m_animations2D.at(m_playingAnimationName).GetFrameData();

					AnimFrameDataToSprite(frameData);

					//TODO:���Έړ��A��Έړ��ł����͈Ⴄ�B
					m_position = frameData.Position;
				}

				if (m_sprite)
				{
					m_sprite->UpdateWorldMatrix(
						m_position, m_rotation, m_scale, m_pivot, m_anchor);
				}
				return;
			}

			void CAnimatedSpriteRenderer::OnDestroy()
			{
				Terminate();
				return;
			}
			void CAnimatedSpriteRenderer::Init(const nsSprites::SSpriteInitData& spriteInitData)
			{
				m_sprite = new nsSprites::CSprite();

				m_sprite->Init(spriteInitData);

				if (spriteInitData.alphaBlendMode == nsSprites::EnAlphaBlendMode::enTrans)
				{
					m_sprite->SetRenderType(CRendererTable::EnRendererType::enTransSprite);
				}
				else
				{
					nsGameWindow::MessageBoxWarning(L"CAnimatedSpriteRenderer���g�p���鎞�̓A���t�@�u�����h���[�h��enTrans�Ɏw�肵�Ă��������B");
				}

				m_sprite->EnableDrawing();

				return;
			}

			void CAnimatedSpriteRenderer::Terminate()
			{
				m_sprite->DisableDrawing();
				if (m_sprite)
				{
					delete m_sprite;
					m_sprite = nullptr;
				}
				return;
			}

			void CAnimatedSpriteRenderer::InitAnimation(const char* fileName)
			{
				nsGraphics::nsAnimations2D::CAnimation2D newAnimation;

				newAnimation.Init(fileName);

				m_animations2D.insert({ newAnimation.m_animationData->m_animtionName,newAnimation });

			}

			void CAnimatedSpriteRenderer::PlayAnimation(std::string playAnimName)
			{
				if (m_animations2D.count(playAnimName) != 1)
				{
					return;
				}

				if (m_animations2D.count(m_playingAnimationName) == 1)
				{
					nsGraphics::nsAnimations2D::SAnimation2DFrameData currentData = m_animations2D.at(m_playingAnimationName).GetFrameData();
				
					//���ݍĐ����̃A�j���[�V�������������ꍇ�A�����܂ł̈ړ��f�[�^��ۑ�����
					AnimFrameDataToSprite(currentData);
					m_animations2D.at(m_playingAnimationName).ResetFrame();
				}

				m_animations2D.at(playAnimName).SetDefaultPosition(GetPosition());
				m_animations2D.at(playAnimName).PlayAnimation();
				m_playingAnimationName = playAnimName;
			}
		}
	}
}
