#include "RenderTarget.h"
#include "../GraphicsEngine.h"
#include "../GameWindow/MessageBox.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDx12Wrappers
		{
			const wchar_t* const CRenderTarget::m_kNamePrefix = L"RenderTarget: ";

			CRenderTarget::~CRenderTarget()
			{
				Terminate();
				return;
			}


			void CRenderTarget::Terminate()
			{
				Release();
				return;
			}

			void CRenderTarget::Release()
			{
				m_renderTargetTexture.Release();
				m_dsvDescriptorHeap.Release();
				m_rtvDescriptorHeap.Release();
				if (m_depthStencilBuffer)
				{
					m_depthStencilBuffer->Release();
					m_depthStencilBuffer = nullptr;
				}
				if (m_renderTarget)
				{
					m_renderTarget->Release();
					m_renderTarget = nullptr;
				}
				return;
			}

			bool CRenderTarget::Init(
				unsigned int width,
				unsigned int height,
				DXGI_FORMAT colorFormat,
				const nsMath::CVector4& clearColor,
				DXGI_FORMAT depthStencilFormat,
				const wchar_t* objectName
			)
			{
				m_rtvClearColor = clearColor;
				auto device = CGraphicsEngine::GetInstance()->GetDevice();

				if (CreateRenderTarget(device, width, height, colorFormat, clearColor) != true)
				{
					Release();
					return false;
				}
				m_renderTargetTexture.InitFromD3DResource(m_renderTarget);

				if (m_rtvDescriptorHeap.InitAsRTV() != true)
				{
					Release();
					return false;
				}

				CreateRenderTargetView(device, colorFormat);


				if (CreateDepthStencilBuffer(device, width, height, depthStencilFormat) != true)
				{
					Release();
					return false;
				}

				CreateDepthStencilView(device, depthStencilFormat);

				SetName(objectName);

				return true;
			}

			bool CRenderTarget::CreateRenderTarget(
				ID3D12Device5* device,
				unsigned int width,
				unsigned int height,
				DXGI_FORMAT colorFormat,
				const nsMath::CVector4& clearColor
			)
			{
				D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

				CD3DX12_RESOURCE_DESC resDesc(
					D3D12_RESOURCE_DIMENSION_TEXTURE2D,
					0,
					width,
					height,
					1,
					1,
					colorFormat,
					1,
					0,
					D3D12_TEXTURE_LAYOUT_UNKNOWN,
					D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
				);

				D3D12_CLEAR_VALUE clearValue =
					CD3DX12_CLEAR_VALUE(colorFormat, clearColor.m_fVec);

				auto result = device->CreateCommittedResource(
					&heapProp,
					D3D12_HEAP_FLAG_NONE,
					&resDesc,
					// D3D12_RESOURCE_STATE_RENDER_TARGETではない。
					// 後でステートを切り替えるまでは、テクスチャとして扱う。
					D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
					&clearValue,
					IID_PPV_ARGS(&m_renderTarget)
				);

				if (FAILED(result))
				{
					nsGameWindow::MessageBoxWarning(L"レンダーターゲットの生成に失敗しました。");
					return false;
				}

				return true;
			}

			void CRenderTarget::CreateRenderTargetView(
				ID3D12Device5* device, DXGI_FORMAT colorFormat)
			{
				D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
				rtvDesc.Format = colorFormat;
				rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

				D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle =
					m_rtvDescriptorHeap.GetCPUHandle();

				device->CreateRenderTargetView(m_renderTargetTexture.GetResource(), &rtvDesc, rtvHandle);

				return;
			}

			bool CRenderTarget::CreateDepthStencilBuffer(
				ID3D12Device5* device,
				unsigned int width,
				unsigned int height,
				DXGI_FORMAT depthStencilFormat
			)
			{
				if (depthStencilFormat == DXGI_FORMAT_UNKNOWN)
				{
					// フォーマットが指定されていなかったら、デプスステンシルバッファを作成しない。
					return true;
				}

				auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

				CD3DX12_RESOURCE_DESC resDesc(
					D3D12_RESOURCE_DIMENSION_TEXTURE2D,
					0,
					static_cast<UINT>(width),
					static_cast<UINT>(height),
					1,
					1,
					depthStencilFormat,
					1,
					0,
					D3D12_TEXTURE_LAYOUT_UNKNOWN,
					D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE
				);

				auto clearValue = CD3DX12_CLEAR_VALUE(depthStencilFormat, 1.0f, 0);

				auto result = device->CreateCommittedResource(
					&heapProp,
					D3D12_HEAP_FLAG_NONE,
					&resDesc,
					D3D12_RESOURCE_STATE_DEPTH_WRITE,
					&clearValue,
					IID_PPV_ARGS(&m_depthStencilBuffer)
				);

				if (FAILED(result))
				{
					nsGameWindow::MessageBoxError(L"デプスステンシルバッファの生成に失敗しました。");
					return false;
				}


				return true;
			}

			void CRenderTarget::CreateDepthStencilView(
				ID3D12Device5* device, DXGI_FORMAT depthStencilFormat)
			{
				if (depthStencilFormat == DXGI_FORMAT_UNKNOWN)
				{
					// フォーマットが指定されていなかったら、デプスステンシルバッファを作成しない。
					return;
				}
				D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
				dsvDesc.Format = depthStencilFormat;
				dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
				dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

				const auto descHeapHandle = m_dsvDescriptorHeap.GetCPUHandle();

				device->CreateDepthStencilView(m_depthStencilBuffer, &dsvDesc, descHeapHandle);

				return;
			}



			void CRenderTarget::SetName(const wchar_t* objectName)
			{
#ifdef _DEBUG
				if (m_renderTarget == nullptr || objectName == nullptr)
				{
					return;
				}
				std::wstring wstr(m_kNamePrefix);
				wstr += objectName;
				auto name = wstr.c_str();
				m_renderTarget->SetName(name);
				m_rtvDescriptorHeap.SetName(name);
				m_renderTargetTexture.SetName(name);
				if (m_depthStencilBuffer == nullptr)
				{
					return;
				}
				wstr = m_kNamePrefix;
				wstr += L"DepthStencil: ";
				wstr += objectName;
				name = wstr.c_str();
				m_depthStencilBuffer->SetName(name);
				m_dsvDescriptorHeap.SetName(name);
#endif
				return;
			}







		}

	}
}