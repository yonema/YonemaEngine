#pragma once

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDx12Wrappers
		{
			inline auto CCommandList::Reset(
				ID3D12CommandAllocator* commandAllorator,
				ID3D12PipelineState* pipelineState
			)
			{
				return m_commandList->Reset(commandAllorator, pipelineState);
			}

			inline void CCommandList::TransitionFromShaderResourceToRenderTarget(
				ID3D12Resource* renderTarget)
			{
				D3D12_RESOURCE_BARRIER barrier =
					CD3DX12_RESOURCE_BARRIER::Transition(
						renderTarget,
						D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
						D3D12_RESOURCE_STATE_RENDER_TARGET
					);

				m_commandList->ResourceBarrier(1, &barrier);
				return;
			}
			inline void CCommandList::TransitionFromShaderResourceToRenderTarget(
				const CRenderTarget& renderTarget)
			{
				TransitionFromShaderResourceToRenderTarget(renderTarget.Get());
				return;
			}
			inline void CCommandList::TransitionFromRenderTargetToShaderResource(
				ID3D12Resource* renderTarget)
			{
				D3D12_RESOURCE_BARRIER barrier =
					CD3DX12_RESOURCE_BARRIER::Transition(
						renderTarget,
						D3D12_RESOURCE_STATE_RENDER_TARGET,
						D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
					);

				m_commandList->ResourceBarrier(1, &barrier);
				return;
			}
			inline void CCommandList::TransitionFromRenderTargetToShaderResource(
				const CRenderTarget& renderTarget)
			{
				TransitionFromRenderTargetToShaderResource(renderTarget.Get());
				return;
			}
			inline void CCommandList::TransitionFromPresentToRenderTarget(
				ID3D12Resource* renderTarget)
			{
				D3D12_RESOURCE_BARRIER barrier =
					CD3DX12_RESOURCE_BARRIER::Transition(
						renderTarget,
						D3D12_RESOURCE_STATE_PRESENT,
						D3D12_RESOURCE_STATE_RENDER_TARGET
					);

				m_commandList->ResourceBarrier(1, &barrier);
				return;
			}
			inline void CCommandList::TransitionFromPresentToRenderTarget(
				const CRenderTarget& renderTarget)
			{
				TransitionFromPresentToRenderTarget(renderTarget.Get());
				return;
			}
			inline void CCommandList::TransitionFromPresentToRenderTarget(
				const CFrameBuffer& frameBuffer)
			{
				TransitionFromPresentToRenderTarget(frameBuffer.GetCurrentRenerTarget());
				return;
			}
			inline void CCommandList::TransitionFromRenderTargetToPresent(
				ID3D12Resource* renderTarget)
			{
				D3D12_RESOURCE_BARRIER barrier =
					CD3DX12_RESOURCE_BARRIER::Transition(
						renderTarget,
						D3D12_RESOURCE_STATE_RENDER_TARGET,
						D3D12_RESOURCE_STATE_PRESENT
					);

				m_commandList->ResourceBarrier(1, &barrier);
				return;
			}
			inline void CCommandList::TransitionFromRenderTargetToPresent(
				const CRenderTarget& renderTarget)
			{
				TransitionFromRenderTargetToPresent(renderTarget.Get());
				return;
			}
			inline void CCommandList::TransitionFromRenderTargetToPresent(
				const CFrameBuffer& frameBuffer)
			{
				TransitionFromRenderTargetToPresent(frameBuffer.GetCurrentRenerTarget());
				return;
			}

			inline void CCommandList::SetRenderTargets(
				unsigned int numRt, D3D12_CPU_DESCRIPTOR_HANDLE* rtvHandle, D3D12_CPU_DESCRIPTOR_HANDLE* dsvHandle)
			{
				m_commandList->OMSetRenderTargets(numRt, rtvHandle, false, dsvHandle);
				return;
			}
			inline void CCommandList::SetRenderTarget(const CFrameBuffer& frameBuffer)
			{
				auto rtvHandle = frameBuffer.GetCurrentRtvCpuDescriptorHandle();
				auto dsvHandle = frameBuffer.GetDsvCpuDescriptorHandle();
				SetRenderTargets(1, &rtvHandle, &dsvHandle);
			}

			inline void CCommandList::SetRenderTarget(const CRenderTarget& renderTarget)
			{
				auto rtvDescHandle = renderTarget.GetRtvDescriptorCpuHandle();
				if (renderTarget.IsValidDepthStencilBuffer())
				{
					auto dsvDescHandle = renderTarget.GetDsvDescriptorCpuHandle();
					SetRenderTargets(1, &rtvDescHandle, &dsvDescHandle);
				}
				else
				{
					SetRenderTargets(1, &rtvDescHandle, nullptr);
				}
			}

			inline void CCommandList::SetRenderTargets(
				unsigned int numRt, const CRenderTarget* renderTargets[])
			{
				D3D12_CPU_DESCRIPTOR_HANDLE rtDescHandles[m_kMaxNumRenderTargets];
				numRt = min(numRt, m_kMaxNumRenderTargets);

				for (unsigned int i = 0; i < numRt; i++)
				{
					rtDescHandles[i] = renderTargets[i]->GetRtvDescriptorCpuHandle();
				}

				if (renderTargets[0]->IsValidDepthStencilBuffer())
				{
					auto dsvDescHandle = renderTargets[0]->GetDsvDescriptorCpuHandle();
					SetRenderTargets(numRt, rtDescHandles, &dsvDescHandle);
				}
				else
				{
					SetRenderTargets(numRt, rtDescHandles, nullptr);
				}

				return;
			}

			inline void CCommandList::SetRenderTargets(
				unsigned int numRt,
				const CRenderTarget* renderTargets[],
				D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle
			)
			{
				D3D12_CPU_DESCRIPTOR_HANDLE rtDescHandles[m_kMaxNumRenderTargets];
				numRt = min(numRt, m_kMaxNumRenderTargets);

				for (unsigned int i = 0; i < numRt; i++)
				{
					rtDescHandles[i] = renderTargets[i]->GetRtvDescriptorCpuHandle();
				}

				m_commandList->OMSetRenderTargets(numRt, rtDescHandles, false, &dsvHandle);

				return;
			}

			inline void CCommandList::ClearRenderTargetView(
				D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle, const nsMath::CVector4& clearColor)
			{
				m_commandList->ClearRenderTargetView(rtvHandle, clearColor.m_fVec, 0, nullptr);
				return;
			}
			inline void CCommandList::ClearRenderTargetView(const CRenderTarget& renderTarget)
			{
				const CRenderTarget* rtArray[] = { &renderTarget };
				ClearRenderTargetViews(1, rtArray);
				return;
			}
			inline void CCommandList::ClearRenderTargetViews(
				unsigned int numRt, const CRenderTarget* renderTargets[])
			{
				numRt = min(numRt, m_kMaxNumRenderTargets);
				for (unsigned int i = 0; i < numRt; i++)
				{
					ClearRenderTargetView(
						renderTargets[i]->GetRtvDescriptorCpuHandle(),
						renderTargets[i]->GetRtvClearColor()
					);
				}
				return;
			}

			inline void CCommandList::ClearDepthStencilView(
				D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle, float clearValue)
			{
				m_commandList->ClearDepthStencilView(
					dsvHandle,
					D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
					clearValue,
					0,
					0,
					nullptr);
				return;
			}

			inline void CCommandList::ClearRenderTargetAndDepthStencilView(
				D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle,
				const nsMath::CVector4& clearColor,
				D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle,
				float clearValue
			)
			{
				ClearRenderTargetView(rtvHandle, clearColor);
				ClearDepthStencilView(dsvHandle, clearValue);
				return;
			}
			inline void CCommandList::ClearRenderTargetAndDepthStencilView(
				const CRenderTarget& renderTarget)
			{
				const CRenderTarget* rtArray[] = { &renderTarget };
				ClearRenderTargetAndDepthStencilViews(1, rtArray);
				return;
			}
			inline void CCommandList::ClearRenderTargetAndDepthStencilView(
				const CFrameBuffer& frameBuffer)
			{
				auto rtvHandle = frameBuffer.GetCurrentRtvCpuDescriptorHandle();
				auto& clearColor = frameBuffer.GetRtvClearColor();
				auto dsvHandle = frameBuffer.GetDsvCpuDescriptorHandle();
				auto clearValue = frameBuffer.GetDsvClearValue();
				ClearRenderTargetAndDepthStencilView(rtvHandle, clearColor, dsvHandle, clearValue);
			}
			inline void CCommandList::ClearRenderTargetAndDepthStencilViews(
				unsigned int numRt, const CRenderTarget* renderTargets[])
			{
				if (renderTargets[0]->IsValidDepthStencilBuffer()) {
					//深度バッファがある。
					ClearDepthStencilView(
						renderTargets[0]->GetDsvDescriptorCpuHandle(),
						renderTargets[0]->GetDsvClearValue());
				}
				ClearRenderTargetViews(numRt, renderTargets);
				return;
			}


			inline void CCommandList::SetViewport(const D3D12_VIEWPORT& viewport)
			{
				m_currentViewport = viewport;
				m_commandList->RSSetViewports(1, &viewport);
				return;
			}
			inline void CCommandList::SetScissorRect(const D3D12_RECT& scissorRect)
			{
				m_commandList->RSSetScissorRects(1, &scissorRect);
				return;
			}
			inline void CCommandList::SetViewportAndScissorRect(
				const D3D12_VIEWPORT& viewport, const D3D12_RECT& scissorRect)
			{
				SetViewport(viewport);
				SetScissorRect(scissorRect);
				return;
			}
			inline void CCommandList::SetViewportAndScissorRect(const D3D12_VIEWPORT& viewport)
			{
				D3D12_RECT scissorRect;
				scissorRect.top = 0;
				scissorRect.left = 0;
				scissorRect.bottom = static_cast<LONG>(viewport.Height);
				scissorRect.right = static_cast<LONG>(viewport.Width);

				SetViewportAndScissorRect(viewport, scissorRect);

				return;
			}

			inline void CCommandList::SetGraphicsRootSignature(ID3D12RootSignature* rootSignature)
			{
				m_commandList->SetGraphicsRootSignature(rootSignature);
				return;
			}

			inline void CCommandList::SetGraphicsRootSignature(CRootSignature* rootSignature)
			{
				SetGraphicsRootSignature(rootSignature->Get());
				return;
			}
			inline void CCommandList::SetPipelineState(ID3D12PipelineState* pipelineState)
			{
				m_commandList->SetPipelineState(pipelineState);
				return;
			}
			inline void CCommandList::SetPipelineState(CPipelineState* pipelineState)
			{
				SetPipelineState(pipelineState->Get());
				return;
			}
			inline void CCommandList::SetGraphicsRootSignatureAndPipelineState(
				ID3D12RootSignature* rootSignature, ID3D12PipelineState* pipelineState)
			{
				SetGraphicsRootSignature(rootSignature);
				SetPipelineState(pipelineState);
				return;
			}
			inline void CCommandList::SetGraphicsRootSignatureAndPipelineState(
				CRootSignature* rootSignature, CPipelineState* pipelineState)
			{
				SetGraphicsRootSignature(rootSignature);
				SetPipelineState(pipelineState);
				return;
			}

			inline void CCommandList::SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY topoloty)
			{
				m_commandList->IASetPrimitiveTopology(topoloty);
				return;
			}


			inline void CCommandList::SetDescriptorHeap(const CDescriptorHeap& descHeap)
			{
				const CDescriptorHeap* descHeaps[] = { &descHeap };
				SetDescriptorHeaps(1, descHeaps);
				return;
			}
			inline void CCommandList::SetDescriptorHeap(ID3D12DescriptorHeap* descHeap)
			{
				ID3D12DescriptorHeap* descHeaps[] = { descHeap };
				SetDescriptorHeaps(1, descHeaps);
				return;
			}

			inline void CCommandList::SetDescriptorHeaps(
				unsigned int numDescHeaps, const CDescriptorHeap* descHeaps[])
			{
				numDescHeaps = min(numDescHeaps, m_kMaxNumDescriptorHeaps);
				for (unsigned int i = 0; i < numDescHeaps; i++)
				{
					m_descriptorHeaps[i] = descHeaps[i]->Get();
				}
				m_commandList->SetDescriptorHeaps(numDescHeaps, m_descriptorHeaps);
				return;
			}
			inline void CCommandList::SetDescriptorHeaps(
				unsigned int numDescHeaps, ID3D12DescriptorHeap* descHeaps[])
			{
				numDescHeaps = min(numDescHeaps, m_kMaxNumDescriptorHeaps);
				for (unsigned int i = 0; i < numDescHeaps; i++)
				{
					m_descriptorHeaps[i] = descHeaps[i];
				}
				m_commandList->SetDescriptorHeaps(numDescHeaps, m_descriptorHeaps);
				return;
			}

			inline void CCommandList::SetGraphicsRootDescriptorTable(
				unsigned int rootParamerterIndex, D3D12_GPU_DESCRIPTOR_HANDLE baseDescriptor)
			{
				m_commandList->SetGraphicsRootDescriptorTable(rootParamerterIndex, baseDescriptor);
				return;
			}
			inline void CCommandList::SetGraphicsRootDescriptorTable(
				unsigned int rootParamerterIndex, const CDescriptorHeap& descHeap)
			{
				auto heapHandle = descHeap.GetGPUHandle();
				SetGraphicsRootDescriptorTable(rootParamerterIndex, heapHandle);
				return;
			}

			inline void CCommandList::SetVertexBuffer(
				const D3D12_VERTEX_BUFFER_VIEW& vertexBufferView)
			{
				m_commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
				return;
			}
			inline void CCommandList::SetVertexBuffer(const CVertexBuffer& vertexBuffer)
			{
				SetVertexBuffer(vertexBuffer.GetView());
				return;
			}
			inline void CCommandList::SetIndexBuffer(
				const D3D12_INDEX_BUFFER_VIEW& indexBufferView)
			{
				m_commandList->IASetIndexBuffer(&indexBufferView);
				return;
			}
			inline void CCommandList::SetIndexBuffer(const CIndexBuffer& indexBuffer)
			{
				SetIndexBuffer(indexBuffer.GetView());
				return;
			}

			inline void CCommandList::Draw(unsigned int vertexCount)
			{
				m_commandList->DrawInstanced(vertexCount, 1, 0, 0);
				return;
			}
			inline void CCommandList::DrawInstanced(unsigned indexCount)
			{
				m_commandList->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);
				return;
			}
			inline void CCommandList::DrawInstanced(
				unsigned indexCount, unsigned int numInstances, unsigned int startIndexLocation)
			{
				m_commandList->DrawIndexedInstanced(
					indexCount, numInstances, startIndexLocation, 0, 0);
				return;
			}

			inline void CCommandList::Close()
			{
				m_commandList->Close();
				return;
			}
		}
	}
}
