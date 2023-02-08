#include "SkyCubeRenderer.h"
#include "../GraphicsEngine.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsEnvironment
		{
			const char* const CSkyCubeRenderer::m_kModelFilePath[static_cast<int>(EnSkyType::enNum)] =
			{
				"Assets/Models/Sky/SkyCube.fbx"
			};
			const float CSkyCubeRenderer::m_kDefaultSkyScale = 750.0f;
			const float CSkyCubeRenderer::m_kDefaultAutoRotateSpeed = 0.1f;


			bool CSkyCubeRenderer::Start()
			{

				return true;
			}

			void CSkyCubeRenderer::Update(float deltaTime)
			{
				if (m_modelRenderer == nullptr)
				{
					// ñ¢èâä˙âª
					return;
				}

				UpdateConstantBuffer();

				AutoFollowCamera();

				AutoRotate(deltaTime);

				return;
			}

			void CSkyCubeRenderer::OnDestroy()
			{
				DeleteGO(m_modelRenderer);
				m_modelRenderer = nullptr;

				m_expandConstantBuffer.Release();

				return;
			}

			void CSkyCubeRenderer::Init(EnSkyType skyType)
			{
				CreateExpandCB();

				m_modelRenderer = NewGO<nsRenderers::CModelRenderer>();
				nsRenderers::SModelInitData modelInitData = {};
				modelInitData.modelFilePath = m_kModelFilePath[static_cast<int>(skyType)];
				modelInitData.rendererType = 
					nsRenderers::CRendererTable::EnRendererType::enSkyCube;
				modelInitData.SetFlags(EnModelInitDataFlags::enShadowReceiver, false);
				modelInitData.pExpandConstantBuffer = &m_expandConstantBuffer;

				m_modelRenderer->SetScale(m_kDefaultSkyScale);
				m_modelRenderer->Init(modelInitData);

				return;
			}

			void CSkyCubeRenderer::CreateExpandCB()
			{
				m_expandConstantBuffer.Release();

				auto cbSize = sizeof(m_constantBufferData);

				m_expandConstantBuffer.Init(
					static_cast<unsigned int>(cbSize), L"SkyExpandCB", 1, &m_constantBufferData);

				return;
			}

			void CSkyCubeRenderer::UpdateConstantBuffer() noexcept
			{
				if (m_isDirtyFlag != true)
				{
					return;
				}

				m_expandConstantBuffer.CopyToMappedConstantBuffer(
					&m_constantBufferData, sizeof(m_constantBufferData));

				return;
			}


			void CSkyCubeRenderer::AutoFollowCamera() noexcept
			{
				if (m_autoFollowCameraFlag != true)
				{
					return;
				}

				const auto& camPos = 
					CGraphicsEngine::GetInstance()->GetMainCamera()->GetPosition();

				m_modelRenderer->SetPosition(camPos);


				return;
			}

			void CSkyCubeRenderer::AutoRotate(float deltaTime) noexcept
			{
				if (m_autoRotateFlag != true)
				{
					return;
				}

				m_rotateDegAngle += m_autoRotateSpeed * deltaTime;

				if (m_rotateDegAngle > 360.0f)
				{
					m_rotateDegAngle = 0.0f;
				}
				else if (m_rotateDegAngle < 0.0f)
				{
					m_rotateDegAngle = 360.0f;
				}

				nsMath::CQuaternion qRot;
				qRot.SetRotationYDeg(m_rotateDegAngle);

				m_modelRenderer->SetRotation(qRot);

				return;
			}


		}
	}
}