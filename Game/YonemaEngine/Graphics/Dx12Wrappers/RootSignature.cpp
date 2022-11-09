#include "RootSignature.h"
#include "../GraphicsEngine.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDx12Wrappers
		{
			const wchar_t* const CRootSignature::m_kNamePrefix = L"RootSignature: ";

			CRootSignature::~CRootSignature()
			{
				Terminate();
				return;
			}

			void CRootSignature::Terminate()
			{
				Release();
				return;
			}

			void CRootSignature::Release()
			{
				if (m_rootSignature)
				{
					m_rootSignature->Release();
					m_rootSignature = nullptr;
				}
				return;
			}

			bool CRootSignature::Init(
				unsigned int numParameters,
				const D3D12_ROOT_PARAMETER1* pParameters,
				UINT numStaticSamplers,
				const D3D12_STATIC_SAMPLER_DESC* pStaticSamplers,
				D3D12_ROOT_SIGNATURE_FLAGS flags,
				const wchar_t* objectName
			)
			{
				Release();

				static auto device = CGraphicsEngine::GetInstance()->GetDevice();


				CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
				rootSignatureDesc.Init_1_1(
					numParameters,
					pParameters,
					numStaticSamplers,
					pStaticSamplers,
					flags
				);

				CBlob rootSignatureBlob;
				CBlob errorBlob;

				auto result = D3DX12SerializeVersionedRootSignature(
					&rootSignatureDesc,
					D3D_ROOT_SIGNATURE_VERSION_1_1,
					rootSignatureBlob.GetPp(),
					errorBlob.GetPp()
				);

				
				if (CheckErrorBlob(result, &errorBlob) != true)
				{
					nsGameWindow::MessageBoxWarning(L"ルートシグネチャのバイナリコードの生成に失敗しました。");
					return false;
				}

				result = device->CreateRootSignature(
					0,
					rootSignatureBlob.Get()->GetBufferPointer(),
					rootSignatureBlob.Get()->GetBufferSize(),
					IID_PPV_ARGS(&m_rootSignature)
				);

				if (FAILED(result))
				{
					nsGameWindow::MessageBoxWarning(L"ルートシグネチャの生成に失敗しました。");
					return false;
				}

				SetName(objectName);


				// 不要になったため解放。
				rootSignatureBlob.Release();


				return true;
			}

			void CRootSignature::SetName(const wchar_t* objectName)
			{
#ifdef _DEBUG
				if (m_rootSignature == nullptr || objectName == nullptr)
				{
					return;
				}
				std::wstring wstr(m_kNamePrefix);
				wstr += objectName;
				m_rootSignature->SetName(wstr.c_str());
#endif
				return;

			}

		}
	}
}