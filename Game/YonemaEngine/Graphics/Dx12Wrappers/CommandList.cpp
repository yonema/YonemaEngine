#include "CommandList.h"
#include "../GraphicsEngine.h"
#include "../GameWindow/MessageBox.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDx12Wrappers
		{
			const wchar_t* const CCommandList::m_kNamePrefix = L"CommandList: ";


			CCommandList::~CCommandList()
			{
				Terminate();
				return;
			}

			void CCommandList::Terminate()
			{
				Release();
				return;
			}

			void CCommandList::Release()
			{
				if (m_commandList)
				{
					m_commandList->Release();
					m_commandList = nullptr;
				}
				return;
			}

			bool CCommandList::Init(D3D12_COMMAND_LIST_TYPE commandListType)
			{
				auto device = CGraphicsEngine::GetInstance()->GetDevice();
				auto commandAllocator = CGraphicsEngine::GetInstance()->GetCommandAllocator();

				auto result = device->CreateCommandList(
					0,
					commandListType,
					commandAllocator,
					nullptr,
					IID_PPV_ARGS(&m_commandList)
				);

				if (FAILED(result))
				{
					nsGameWindow::MessageBoxError(L"コマンドリストの生成に失敗しました。");
					return false;
				}

				// コマンドリストは、開かれている状態で生成されるため、閉じておく。
				m_commandList->Close();

				return true;
			}


			void CCommandList::SetName(const wchar_t* objectName)
			{
#ifdef _DEBUG
				if (m_commandList == nullptr || objectName == nullptr)
				{
					return;
				}
				std::wstring wstr(m_kNamePrefix);
				wstr += objectName;
				m_commandList->SetName(wstr.c_str());
#endif
				return;

			}

		}
	}
}