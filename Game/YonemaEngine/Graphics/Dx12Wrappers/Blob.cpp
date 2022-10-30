#include "Blob.h"
#include "../GameWindow/MessageBox.h"
#include "../Utils/StringManipulation.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDx12Wrappers
		{
			CBlob::~CBlob()
			{
				Terminate();
				return;
			}

			void CBlob::Terminate()
			{
				Release();
				return;
			}

			void CBlob::Release()
			{
				if (m_blob)
				{
					m_blob->Release();
					m_blob = nullptr;
				}
				return;
			}


			bool CheckErrorBlob(HRESULT result, CBlob* errorBlob)
			{
				if (SUCCEEDED(result))
				{
					errorBlob->Release();
					return true;
				}

				auto error = errorBlob->Get();

				if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
				{
					::OutputDebugStringA("ƒtƒ@ƒCƒ‹‚ªŒ©“–‚½‚è‚Ü‚¹‚ñ");
				}
				else
				{
					std::string errstr;
					errstr.resize(error->GetBufferSize());
					std::copy_n((char*)error->GetBufferPointer(), error->GetBufferSize(), errstr.begin());
					errstr += "\n";
					::OutputDebugStringA(errstr.c_str());
					
					std::wstring errwstr = nsUtils::GetWideStringFromString(errstr);
					nsGameWindow::MessageBoxWarning(errwstr.c_str());

				}

				errorBlob->Release();

				return false;


			}


		}
	}
}