#include "ShadowMapRenderer.h"
#include "../Renderers/RendererTable.h"
#include "../Renderers/SpriteRenderer.h"
#include "ShadowMapGenericRenderer.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsShadow
		{
			const float CShadowMapRenderer::m_kShadowMapSize = 512.0f;
			const nsMath::CVector4 CShadowMapRenderer::m_kClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
			const DXGI_FORMAT CShadowMapRenderer::m_kDepthStencilFormat = DXGI_FORMAT_D32_FLOAT;
			// �K�E�V�A���u���[��������Ƃ��A���_�V�F�[�_�[�ł��s�N�Z���V�F�[�_�[�ł�
			// ���\�[�X�ɃA�N�Z�X���邽��ALL���w��B
			const D3D12_RESOURCE_STATES CShadowMapRenderer::m_kResourceState = 
				D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;

			const float CShadowMapRenderer::m_kShadowCameraOrthographicProjectionSize = 256.0f;
			const float CShadowMapRenderer::m_kShadowCameraNearClip = 10.0f;
			const float CShadowMapRenderer::m_kShadowCameraFarClip = 500.0f;
			const float CShadowMapRenderer::m_kShadowCameraOffset = 180.0f;
			const float CShadowMapRenderer::m_kShadowCameraOffsetByAngle = 90.0f;
			const float CShadowMapRenderer::m_kShadowCameraHeight = 150.0f;

			const float CShadowMapRenderer::m_kBlurPower = 5.0f;


			void CShadowMapRenderer::Init()
			{
				auto res = m_shadowMapRT.Init(
					static_cast<unsigned int>(m_kShadowMapSize),
					static_cast<unsigned int>(m_kShadowMapSize),
					CShadowMapGenericRenderer::m_kRTVFormat,
					m_kClearColor,
					m_kDepthStencilFormat,
					m_kResourceState,
					L"ShadowMapRenderTarget"
				);


				nsSprites::SSpriteInitData initData;
				initData.texture = m_shadowMapRT.GetRenderTargetTexture();
				initData.spriteSize = initData.texture->GetTextureSize();
				res = m_shadowMapSprite.Init(initData);

				m_blur.Init(m_shadowMapSprite.GetTexture(), m_kBlurPower);

				InitShadowCamera();

				return;
			}

			void CShadowMapRenderer::InitShadowCamera()
			{
				m_shadowCamera.SetProjectionType(CCamera::EnProjectionType::enOrthographic);
				m_shadowCamera.SetOrthographicProjectionSize(
					{
						m_kShadowCameraOrthographicProjectionSize,
						m_kShadowCameraOrthographicProjectionSize
					}
				);
				m_shadowCamera.SetAspectRatio(m_kShadowMapSize / m_kShadowMapSize);
				m_shadowCamera.SetPosition({ 10.0f,m_kShadowCameraHeight, 10.0f });
				m_shadowCamera.SetTargetPosition({ 0.0f,0.0f,0.0f });
				m_shadowCamera.SetUpDirection(nsMath::CVector3::Up());
				m_shadowCamera.SetNearClip(m_kShadowCameraNearClip);
				m_shadowCamera.SetFarClip(m_kShadowCameraFarClip);
				m_shadowCamera.UpdateCameraParam();

				return;
			}


			CShadowMapRenderer::~CShadowMapRenderer()
			{
				Terminate();

				return;
			}

			void CShadowMapRenderer::Terminate()
			{
				Release();

				return;
			}


			void CShadowMapRenderer::Release()
			{
				m_blur.Release();
				m_shadowMapSprite.Release();
				m_shadowMapRT.Release();

				return;
			}


			void CShadowMapRenderer::ExecuteDraw(
				nsDx12Wrappers::CCommandList* commandList,
				nsRenderers::CRendererTable* rendererTable
			)
			{
				// �`����ݒ�
				commandList->TransitionResourceState(
					m_shadowMapRT,
					m_kResourceState,
					D3D12_RESOURCE_STATE_RENDER_TARGET
				);
				commandList->SetRenderTarget(m_shadowMapRT);
				// �r���[�|�[�g�ƃV�U�����O��`��ݒ�
				commandList->SetViewportAndScissorRect(m_shadowMapRT);

				// ��ʃN���A
				commandList->ClearRenderTargetAndDepthStencilView(m_shadowMapRT);

				// �g�|���W��ݒ�
				commandList->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				using CRendererTable = nsRenderers::CRendererTable;
				using RendererType = CRendererTable::EnRendererType;
				for (
					unsigned int rendererType = CRendererTable::m_kFirstIndexOfShadowModel;
					rendererType <= CRendererTable::m_kLastIndexOfShadowModel;
					rendererType++
					)
				{
					// ���[�g�V�O�l�`���ƃp�C�v���C���X�e�[�g��ݒ�
					commandList->SetGraphicsRootSignatureAndPipelineState(
						rendererTable->GetRootSignature(rendererType),
						rendererTable->GetPipelineState(rendererType)
					);

					// �`��
					for (auto& rendererList : rendererTable->GetRendererListArray(rendererType))
					{
						for (auto& renderer : rendererList)
						{
							renderer->DrawWrapper(commandList);
						}
					}
				}

				// �`��I��
				commandList->TransitionResourceState(
					m_shadowMapRT,
					D3D12_RESOURCE_STATE_RENDER_TARGET,
					m_kResourceState
				);

				m_blur.ExecuteOnGPU(commandList);

				return;
			}

			void CShadowMapRenderer::Update(const nsMath::CVector3& lightDir)
			{
				const auto& mainCamera = CGraphicsEngine::GetInstance()->GetMainCamera();

				// CalcShadowOffset
				nsMath::CVector3 shadowCameraOffset = nsMath::CVector3::Zero();
				{
					auto camDirXZ = mainCamera->GetForwardDirection();
					camDirXZ.y = 0.0f;
					camDirXZ.Normalize();
					
					auto ligDirXZ = lightDir;
					ligDirXZ.y = 0.0f;
					ligDirXZ.Normalize();

					// �J�����ƃ��C�g�̌����̂Ȃ��p�ɉ����ăI�t�Z�b�g��ω�
					float dot = Dot(camDirXZ, ligDirXZ);
					float radAngle = acosf(dot);
					float degAngle = nsMath::RadToDeg(radAngle);
					float power = 1.0f - (degAngle / 180.0f);
					float offsetByAngle = m_kShadowCameraOffsetByAngle * power;

					shadowCameraOffset =
						camDirXZ * static_cast<float>(m_kShadowCameraOffset - offsetByAngle);
				}

				// CalcShadowCamTarget
				auto shadowCamPos = mainCamera->GetPosition();
				shadowCamPos += shadowCameraOffset;
				m_shadowCamera.SetTargetPosition(shadowCamPos);

				// CalcShadowCamPos
				const auto& origin = shadowCamPos;
				auto invDir = lightDir;
				invDir.Scale(-1.0f);
				const float cosTheta = Dot(nsMath::CVector3::Up(), invDir);
				float length = m_kShadowCameraHeight / cosTheta;
				auto toPos = invDir * length;
				shadowCamPos += toPos;
				m_shadowCamera.SetPosition(shadowCamPos);


				m_shadowCamera.UpdateCameraParam();

				return;
			}




		}
	}
}