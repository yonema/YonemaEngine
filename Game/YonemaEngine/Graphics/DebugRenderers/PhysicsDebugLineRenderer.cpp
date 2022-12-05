#include "PhysicsDebugLineRenderer.h"
#include "../../Utils/AlignSize.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDebugRenderers
		{
			void CPhysicsDebugLineRenderer::Draw(nsDx12Wrappers::CCommandList* commandList)
			{
				if (m_vertexArray.size() == 0)
				{
					return;
				}
				m_vertexBuffer.Copy(
					&m_vertexArray[0],
					static_cast<unsigned int>(sizeof(m_vertexArray[0]) * m_vertexArray.size())
				);
				auto mViewProj = CGraphicsEngine::GetInstance()->GetMatrixViewProj();
				m_constantBuffer.CopyToMappedConstantBuffer(&mViewProj, sizeof(mViewProj));


				// ○定数バッファのディスクリプタヒープをセット
				ID3D12DescriptorHeap* descHeaps[] = { m_descriptorHeap.Get() };
				commandList->SetDescriptorHeaps(1, descHeaps);
				auto descriptorHeapH = m_descriptorHeap.GetGPUHandle();
				commandList->SetGraphicsRootDescriptorTable(0, descriptorHeapH);


				commandList->SetVertexBuffer(m_vertexBuffer);

				commandList->Draw(static_cast<unsigned int>(m_vertexArray.size()));

				m_vertexArray.clear();

				return;
			}

			CPhysicsDebugLineRenderer::CPhysicsDebugLineRenderer()
			{
				Init();
				return;
			}
			CPhysicsDebugLineRenderer::~CPhysicsDebugLineRenderer()
			{
				Terminate();
				return;
			}

			void CPhysicsDebugLineRenderer::Terminate()
			{
				Release();
				return;
			}

			void CPhysicsDebugLineRenderer::Release()
			{
				m_descriptorHeap.Release();
				m_constantBuffer.Release();
				m_vertexBuffer.Release();
				m_vertexArray.clear();

				return;
			}


			bool CPhysicsDebugLineRenderer::Init()
			{
				m_vertexArray.reserve(m_kMaxVertices);

				if (InitVertexBuffer() != true)
				{
					return false;
				}

				if (InitConstantBuffer() != true)
				{
					return false;
				}

				SetRenderType(nsRenderers::CRendererTable::EnRendererType::enCollisionRenderer);
				EnableDrawing();

				return true;
			}

			bool CPhysicsDebugLineRenderer::InitVertexBuffer()
			{
				static constexpr unsigned int alignSize = 4;
				const auto alignedStrideSize = nsUtils::AlignSize(sizeof(SVertex), alignSize);

				auto res = m_vertexBuffer.Init(
					static_cast<unsigned int>(alignedStrideSize * m_kMaxVertices),
					alignedStrideSize
				);

				return res;
			}

			bool CPhysicsDebugLineRenderer::InitConstantBuffer()
			{
				auto mViewProj = CGraphicsEngine::GetInstance()->GetMatrixViewProj();

				auto res = m_constantBuffer.Init(
					sizeof(nsMath::CMatrix), L"CollisionRenderer", 1, &mViewProj);
				if (res != true)
				{
					return false;
				}

				constexpr unsigned int numDescHeaps = 1;
				res = m_descriptorHeap.InitAsCbvSrvUav(numDescHeaps, L"CollisionRenderer");
				if (res != true)
				{
					return false;
				}

				m_constantBuffer.CreateConstantBufferView(m_descriptorHeap.GetCPUHandle());

				return true;
			}

			void CPhysicsDebugLineRenderer::AddVertex(
				const nsMath::CVector3& fromPos,
				const nsMath::CVector3& toPos,
				const nsMath::CVector3& fromColor,
				const nsMath::CVector3& toColor
			)
			{
				m_vertexArray.emplace_back(fromPos, fromColor);
				m_vertexArray.emplace_back(toPos, toColor);

				if (m_vertexArray.size() > m_kMaxVertices) 
				{
					nsGameWindow::MessageBoxError(L"コリジョンの数が多すぎます。");
					std::abort();
				}

				return;
			}


		}
	}
}