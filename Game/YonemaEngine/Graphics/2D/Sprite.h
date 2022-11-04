#pragma once
#include "../Renderers/Renderer.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace ns2D
		{
			enum class EnAlphaBlendMode
			{
				enNone,
				enTrans,
				enAdd,
				enMultiply
			};
			struct SSpriteInitData
			{
				const char* filePath = nullptr;
				nsDx12Wrappers::CTexture* texture = nullptr;
				nsMath::CVector2 spriteSize = nsMath::CVector2::Zero();
				EnAlphaBlendMode alphaBlendMode = EnAlphaBlendMode::enNone;
				DXGI_FORMAT colorFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			};

			class CSprite : public nsRenderers::IRenderer
			{
			private:
				static const int m_kMaxNumTextures = 8;
				// テクスチャ用のSRV1個 + 定数用のCBV1個 = 2個
				static const int m_kNumDescriptors = 2;
				static const nsMath::CMatrix m_kViewMatrix;

				struct SConstantBufferCPU
				{
					nsMath::CMatrix mWorldViewProj;
					nsMath::CVector4 mulColor;
					nsMath::CVector4 screenParam;
				};

			public:
				void Draw(nsDx12Wrappers::CCommandList* commandList) override final;

			public:
				constexpr CSprite() = default;
				~CSprite();

				bool Init(const SSpriteInitData& initData);

				void Release();

				void UpdateWorldMatrix(
					const nsMath::CVector2 position,
					const nsMath::CQuaternion& rotation,
					const nsMath::CVector3& scale,
					const nsMath::CVector2& pivot
				);

				inline void SetMulColor(const nsMath::CVector4& mulColor) noexcept
				{
					m_constantBufferCPU.mulColor = mulColor;
				}

			private:
				void Terminate();

				bool InitTexture(const SSpriteInitData& initData);

				bool CreateDescriptorHeap();

				bool CreateVertexBuffer();

				bool CreateConstantBuffer();

				void CreateSrvCbv();

			private:
				nsDx12Wrappers::CTexture m_texture;
				nsDx12Wrappers::CDescriptorHeap m_srvDescriptorHeap;
				nsDx12Wrappers::CDescriptorHeap m_cbvDescriptorHeap;
				nsDx12Wrappers::CVertexBuffer m_vertexBuffer;
				nsDx12Wrappers::CConstantBuffer m_constantBuffer;
				SConstantBufferCPU m_constantBufferCPU = {};
				nsMath::CVector2 m_spriteSize = nsMath::CVector2::Zero();
				nsMath::CMatrix m_worldMatrix = nsMath::CMatrix::Identity();
			};
		}

	}
}