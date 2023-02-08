#include "ModelRenderer.h"
#include "../../Utils/StringManipulation.h"
#include "../GraphicsEngine.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsRenderers
		{
			const std::bitset<static_cast<int>(EnModelInitDataFlags::enNum)> 
				SModelInitData::kDefaultFlags(std::string("1000000"));



			bool CModelRenderer::Start()
			{
				if (m_renderer->GetLoadingState() != EnLoadingState::enAfterLoading)
				{
					return true;
				}

				UpdateWorldMatrix();
				m_renderer->PlayAnimation(0);

				return true;
			}

			void CModelRenderer::Update(float deltaTime)
			{
				if (UpdateLoadingAsynchronous() != true)
				{
					return;
				}

				UpdateDistanceFromCamera();

				SwitchingLOD();

				UpdateWorldMatrix();
				if (m_modelInitData.maxInstance > 1)
				{
					UpdateWorldMatrixArray();
				}

				m_updateAnimController.Update(m_distanceFromCamera);

				UpdateAnimation(deltaTime, m_updateAnimController.IsUpdatable());

				m_updateAnimController.NotUpdatable();

				return;
			}

			void CModelRenderer::OnDestroy()
			{
				Terminate();

				return;
			}


			void CModelRenderer::Init(const SModelInitData& modelInitData) noexcept
			{
				m_modelInitData = modelInitData;
				m_enableLoadingAsynchronous = 
					modelInitData.GetFlags(EnModelInitDataFlags::enLoadingAsynchronous);

				if (m_modelInitData.maxInstance > 1)
				{
					if (m_worldMatrixArray.empty() != true)
					{
						m_worldMatrixArray.clear();
					}
					m_worldMatrixArray.resize(m_modelInitData.maxInstance);
					for (auto& worldMatrix : m_worldMatrixArray)
					{
						worldMatrix = nsMath::CMatrix::Identity();
					}
				}

				CreateRenderer(m_modelInitData);

				if (m_enableLoadingAsynchronous != true)
				{
					UpdateWorldMatrix();
				}
				if (m_renderer && m_modelInitData.maxInstance > 1)
				{
					m_renderer->SetNumInstances(m_modelInitData.maxInstance);
				}
				if (m_lodRenderer && m_modelInitData.maxInstance > 1)
				{
					m_lodRenderer->SetNumInstances(m_modelInitData.maxInstance);
				}

				m_updateAnimController.Init(modelInitData.distanceToReducingUpdate);

				return;
			}

			void CModelRenderer::Terminate() noexcept
			{
				if (m_lodRenderer)
				{
					m_lodRenderer->DisableDrawing();
					delete m_lodRenderer;
					m_lodRenderer = nullptr;
				}
				if (m_renderer)
				{
					m_renderer->DisableDrawing();
					delete m_renderer;
					m_renderer = nullptr;
				}
				return;
			}

			void CModelRenderer::CreateRenderer(const SModelInitData& modelInitData) noexcept
			{
				std::string msg = {};

				switch (modelInitData.rendererType)
				{
				case CRendererTable::EnRendererType::enBasicModel:
				case CRendererTable::EnRendererType::enSkyCube:
					m_renderer = new nsModels::CBasicModelRenderer(modelInitData);
					if (IsEnableLOD() && m_enableLoadingAsynchronous != true)
					{
						m_lodRenderer = new nsModels::CBasicModelRenderer(
							modelInitData, true, m_renderer->GetAnimator());
					}
					break;
					break;
				default:
					msg = modelInitData.modelFilePath;
					msg += "のモデルのロードに失敗しました。SModelInitDataのrendererTypeが間違っています。";
					msg.erase(msg.end() - 1);
					nsGameWindow::MessageBoxError(nsUtils::GetWideStringFromString(msg).c_str());
					break;
				}

				return;
			}

			bool CModelRenderer::UpdateLoadingAsynchronous() noexcept
			{
				if (m_enableLoadingAsynchronous != true)
				{
					// 非同期ロードではない。または非同期ロードは終了している。
					return true;
				}

				m_renderer->CheckLoaded();

				if (m_renderer->GetLoadingState() != EnLoadingState::enAfterLoading)
				{
					return false;
				}

				if (m_renderer->IsEnableDrawing() != true)
				{
					// まだ描画機能が有効ではないとき、
					// つまりまだInitAfterImportScene()を呼んでないとき
					m_renderer->InitAfterImportScene();
				}

				if (IsEnableLOD())
				{
					if (m_lodRenderer == nullptr)
					{
						m_lodRenderer = new nsModels::CBasicModelRenderer(m_modelInitData, true);
					}

					m_lodRenderer->CheckLoaded();
					if (m_lodRenderer->GetLoadingState() != EnLoadingState::enAfterLoading)
					{
						return false;
					}

					m_lodRenderer->InitAfterImportScene();
				}

				m_enableLoadingAsynchronous = false;

				m_renderer->PlayAnimation(0);
				if (m_lodRenderer)
				{
					m_lodRenderer->PlayAnimation(0);
				}

				// 非同期ロード終了
				return true;
			}

			void CModelRenderer::UpdateDistanceFromCamera()
			{
				const auto& camPos = 
					CGraphicsEngine::GetInstance()->GetMainCamera()->GetPosition();

				auto toCamPos = camPos - m_position;

				m_distanceFromCamera = toCamPos.Length();

				return;
			}



			void CModelRenderer::UpdateWorldMatrix() noexcept
			{
				if (m_renderer)
				{
					m_renderer->UpdateWorldMatrix(m_position, m_rotation, m_scale);
				}

				if (m_lodRenderer)
				{
					m_lodRenderer->UpdateWorldMatrix(m_position, m_rotation, m_scale);
				}

				return;
			}

			void CModelRenderer::UpdateWorldMatrixArray() noexcept
			{
				if (m_renderer == nullptr)
				{
					return;
				}

				if (m_worldMatrixArray.empty())
				{
					return;
				}

				m_renderer->UpdateWorldMatrixArray(m_worldMatrixArray);

				if (m_lodRenderer == nullptr)
				{
					return;
				}

				m_lodRenderer->UpdateWorldMatrixArray(m_worldMatrixArray);

				return;
			}


			void CModelRenderer::SwitchingLOD() noexcept
			{
				if (m_renderer == nullptr || m_lodRenderer == nullptr)
				{
					return;
				}

				if (IsEnableLOD() != true)
				{
					return;
				}

				if (m_distanceFromCamera > m_modelInitData.distanceToLOD)
				{
					m_lodRenderer->SetDrawingFlag(true);
					m_renderer->SetDrawingFlag(false);
				}
				else
				{
					m_renderer->SetDrawingFlag(true);
					m_lodRenderer->SetDrawingFlag(false);
				}

				return;
			}

			void CModelRenderer::UpdateAnimation(float deltaTime, bool updateAnimMatrix) noexcept
			{
				m_renderer->UpdateAnimation(deltaTime, updateAnimMatrix);

				if (m_lodRenderer)
				{
					m_lodRenderer->UpdateAnimation(deltaTime, updateAnimMatrix);
				}


				return;
			}



		}
	}
}