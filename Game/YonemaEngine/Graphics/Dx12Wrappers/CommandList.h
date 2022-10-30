#pragma once
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDx12Wrappers
		{
			class CCommandList : private nsUtils::SNoncopyable
			{
			private:
				static const wchar_t* const m_kNamePrefix;
				static const unsigned int m_kMaxNumRenderTargets = 8;
				static const unsigned int m_kMaxNumDescriptorHeaps = 4;

			public:
				constexpr CCommandList() = default;
				~CCommandList();
				
				bool Init(D3D12_COMMAND_LIST_TYPE commandListType);

				void Release();

				void SetName(const wchar_t* objectName);

				constexpr auto Get() noexcept
				{
					return m_commandList;
				}

				constexpr const auto& GetCurrentViewport() const noexcept
				{
					return m_currentViewport;
				}

				inline auto Reset(
					ID3D12CommandAllocator* commandAllorator,
					ID3D12PipelineState* pipelineState = nullptr
				);

				inline void TransitionFromShaderResourceToRenderTarget(
					ID3D12Resource* renderTarget);
				inline void TransitionFromShaderResourceToRenderTarget(
					const CRenderTarget& renderTarget);
				inline void TransitionFromRenderTargetToShaderResource(
					ID3D12Resource* renderTarget);
				inline void TransitionFromRenderTargetToShaderResource(
					const CRenderTarget& renderTarget);
				inline void TransitionFromPresentToRenderTarget(
					ID3D12Resource* renderTarget);
				inline void TransitionFromPresentToRenderTarget(
					const CRenderTarget& renderTarget);
				inline void TransitionFromPresentToRenderTarget(
					const CFrameBuffer& frameBuffer);
				inline void TransitionFromRenderTargetToPresent(
					ID3D12Resource* renderTarget);
				inline void TransitionFromRenderTargetToPresent(
					const CRenderTarget& renderTarget);
				inline void TransitionFromRenderTargetToPresent(
					const CFrameBuffer& frameBuffer);

				inline void SetRenderTargets(
					unsigned int numRt,
					D3D12_CPU_DESCRIPTOR_HANDLE* rtvHandle, 
					D3D12_CPU_DESCRIPTOR_HANDLE* dsvHandle);
				inline void SetRenderTarget(const CFrameBuffer& frameBuffer);
				inline void SetRenderTarget(const CRenderTarget& renderTarget);
				inline void SetRenderTargets(unsigned int numRt, const CRenderTarget* renderTargets[]);
				inline void SetRenderTargets(
					unsigned int numRt,
					const CRenderTarget* renderTargets[], 
					D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle
				);

				inline void ClearRenderTargetView(
					D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle, const nsMath::CVector4& clearColor);
				inline void ClearRenderTargetView(const CRenderTarget& renderTarget);
				inline void ClearRenderTargetViews(
					unsigned int numRt, const CRenderTarget* renderTargets[]);

				inline void ClearDepthStencilView(
					D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle, float clearValue = 1.0f);

				inline void ClearRenderTargetAndDepthStencilView(
					D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle,
					const nsMath::CVector4& clearColor,
					D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle,
					float clearValue
				);
				inline void ClearRenderTargetAndDepthStencilView(const CRenderTarget& renderTarget);
				inline void ClearRenderTargetAndDepthStencilView(const CFrameBuffer& frameBuffer);
				inline void ClearRenderTargetAndDepthStencilViews(
					unsigned int numRt, const CRenderTarget* renderTargets[]);

				inline void SetViewport(const D3D12_VIEWPORT& viewport);
				inline void SetScissorRect(const D3D12_RECT& scissorRect);
				inline void SetViewportAndScissorRect(
					const D3D12_VIEWPORT& viewport, const D3D12_RECT& scissorRect);
				inline void SetViewportAndScissorRect(const D3D12_VIEWPORT& viewport);

				inline void SetGraphicsRootSignature(ID3D12RootSignature* rootSignature);
				inline void SetGraphicsRootSignature(CRootSignature* rootSignature);
				inline void SetPipelineState(ID3D12PipelineState* pipelineState);
				inline void SetPipelineState(CPipelineState* pipelineState);
				inline void SetGraphicsRootSignatureAndPipelineState(
					ID3D12RootSignature* rootSignature, ID3D12PipelineState* pipelineState);
				inline void SetGraphicsRootSignatureAndPipelineState(
					CRootSignature* rootSignature, CPipelineState* pipelineState);

				inline void SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY topoloty);

				inline void SetDescriptorHeap(const CDescriptorHeap& descHeap);
				inline void SetDescriptorHeap(ID3D12DescriptorHeap* descHeap);
				inline void SetDescriptorHeaps(
					unsigned int numDescHeaps, const CDescriptorHeap* descHeaps[]);
				inline void SetDescriptorHeaps(
					unsigned int numDescHeaps, ID3D12DescriptorHeap* descHeaps[]);

				inline void SetGraphicsRootDescriptorTable(
					unsigned int rootParamerterIndex, D3D12_GPU_DESCRIPTOR_HANDLE baseDescriptor);
				inline void SetGraphicsRootDescriptorTable(
					unsigned int rootParamerterIndex, const CDescriptorHeap& descHeap);

				inline void SetVertexBuffer(const D3D12_VERTEX_BUFFER_VIEW& vertexBufferView);
				inline void SetVertexBuffer(const CVertexBuffer& vertexBuffer);
				inline void SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW& indexBufferView);
				inline void SetIndexBuffer(const CIndexBuffer& indexBuffer);

				inline void Draw(unsigned int vertexCount);
				inline void DrawInstanced(unsigned indexCount);
				inline void DrawInstanced(
					unsigned indexCount,
					unsigned int numInstances,
					unsigned int startIndexLocation = 0
					);

				inline void Close();


			private:

				void Terminate();

			private:
				ID3D12GraphicsCommandList4* m_commandList = nullptr;
				ID3D12DescriptorHeap* m_descriptorHeaps[m_kMaxNumDescriptorHeaps] = { nullptr };
				D3D12_VIEWPORT m_currentViewport = {};
				
			};





		}
	}
}