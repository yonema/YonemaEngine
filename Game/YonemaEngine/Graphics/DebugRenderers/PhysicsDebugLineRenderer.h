#pragma once
#include "../Renderers/Renderer.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDebugRenderers
		{
			class CPhysicsDebugLineRenderer : public nsRenderers::IRenderer
			{
			protected:
				void Draw(nsDx12Wrappers::CCommandList* commandList) override final;

			private:
				static const unsigned int m_kMaxVertices = 10000000;

				struct SVertex
				{
					constexpr SVertex() = default;
					~SVertex() = default;
					constexpr SVertex(const nsMath::CVector3& pos, const nsMath::CVector3& color)
						: position(pos), color(color) {};

					nsMath::CVector3 position = nsMath::CVector3::Zero();
					nsMath::CVector3 color = 
					{ 
						nsMath::CVector4::Red().r,
						nsMath::CVector4::Red().g,
						nsMath::CVector4::Red().b 
					};
				};

			public:
				CPhysicsDebugLineRenderer();
				~CPhysicsDebugLineRenderer();

				void Release();

				void AddVertex(const nsMath::CVector3& fromPos, const nsMath::CVector3& toPos, const nsMath::CVector3& fromColor, const nsMath::CVector3& toColor);

			private:

				bool Init();

				void Terminate();

				bool InitVertexBuffer();

				bool InitConstantBuffer();

			private:
				std::vector<SVertex> m_vertexArray;

				nsDx12Wrappers::CVertexBuffer m_vertexBuffer;
				nsDx12Wrappers::CConstantBuffer m_constantBuffer;
				nsDx12Wrappers::CDescriptorHeap m_descriptorHeap;
			};

		}
	}
}