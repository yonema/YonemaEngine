#include "ModelRenderer.h"
#include "../../Utils/StringManipulation.h"


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
				if (m_enableLoadingAsynchronous)
				{
					m_renderer->CheckLoaded();

					if (m_renderer->GetLoadingState() != EnLoadingState::enAfterLoading)
					{
						return;
					}
					m_renderer->InitAfterImportScene();
					m_renderer->PlayAnimation(0);
					m_enableLoadingAsynchronous = false;
				}

				UpdateWorldMatrix();
				if (m_modelInitData.maxInstance > 1)
				{
					UpdateWorldMatrixArray();
				}

				m_updateAnimController.Update(m_position);
				m_renderer->UpdateAnimation(deltaTime, m_updateAnimController.IsUpdatable());
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

				m_updateAnimController.Init(modelInitData.distanceToReducingUpdate);

				return;
			}

			void CModelRenderer::Terminate() noexcept
			{
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
					m_renderer = new nsModels::CBasicModelRenderer(modelInitData);
					break;
				case CRendererTable::EnRendererType::enSkyCube:
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

			void CModelRenderer::UpdateWorldMatrix() noexcept
			{
				if (m_renderer)
				{
					m_renderer->UpdateWorldMatrix(m_position, m_rotation, m_scale);
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

				return;
			}

		}
	}
}