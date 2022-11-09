#include "PipelineState.h"
#include "../GraphicsEngine.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDx12Wrappers
		{
			const wchar_t* const CPipelineState::m_kNamePrefix = L"GraphicsPipelineState: ";

			CPipelineState::~CPipelineState()
			{
				Terminate();
				return;
			}

			void CPipelineState::Terminate()
			{
				Release();
				return;
			}

			void CPipelineState::Release()
			{
				if (m_pipelineState)
				{
					m_pipelineState->Release();
					m_pipelineState = nullptr;
				}
				return;
			}

			bool CPipelineState::InitAsGraphicsPipelineState(
				const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc, const wchar_t* objectName)
			{
				auto device = CGraphicsEngine::GetInstance()->GetDevice();
				auto result = device->CreateGraphicsPipelineState(
					&desc,
					IID_PPV_ARGS(&m_pipelineState)
				);

				if (FAILED(result))
				{
					nsGameWindow::MessageBoxWarning(L"グラフィックスパイプラインステートの生成に失敗しました。");
					return false;
				}

				SetName(objectName);

				return true;
			}

			void CPipelineState::SetName(const wchar_t* objectName)
			{
#ifdef _DEBUG
				if (m_pipelineState == nullptr || objectName == nullptr)
				{
					return;
				}
				std::wstring wstr(m_kNamePrefix);
				wstr += objectName;
				m_pipelineState->SetName(wstr.c_str());
#endif
				return;

			}


		}
	}
}