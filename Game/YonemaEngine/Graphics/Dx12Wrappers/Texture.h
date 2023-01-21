#pragma once
namespace nsYMEngine
{
	namespace nsGraphics
	{
		class CGraphicsEngine;
	}
}
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDx12Wrappers
		{
			class CTexture : private nsUtils::SNoncopyable
			{
			private:
				static const int m_kNumWICFileExtensions = 5;
				static const int m_kUpAndLowExtensions = 2;
				static const char* m_kWICFileExtensions[m_kUpAndLowExtensions][m_kNumWICFileExtensions];
				static const char* m_kTGAFileExtensions[m_kUpAndLowExtensions];
				static const char* m_kDDSFileExtensions[m_kUpAndLowExtensions];
				static const wchar_t* const m_kNamePrefix;

			public:
				constexpr CTexture() = default;
				~CTexture();

				void Init(const char* filePath);

				void InitFromD3DResource(ID3D12Resource* texture);

				void InitFromTexture(CTexture* texture);


				void Release();

				constexpr bool IsValid() const noexcept
				{
					return m_texture != nullptr;
				}

				constexpr bool IsCubemap() const noexcept
				{
					return m_isCubemap;
				}
				constexpr const auto& GetTextureSize() const noexcept
				{
					return m_textureSize;
				}

				constexpr auto GetMipLevels() const noexcept
				{
					return m_mipLevels;
				}

				constexpr auto GetFormat() const noexcept
				{
					return m_format;
				}

				constexpr ID3D12Resource* GetResource() noexcept
				{
					return m_texture;
				}

				inline void SetName(const wchar_t* objectName)
				{
#ifdef _DEBUG
					if (m_texture == nullptr || objectName == nullptr)
					{
						return;
					}
					std::wstring wstr(m_kNamePrefix);
					wstr += objectName;
					m_texture->SetName(wstr.c_str());
#endif
				}

				constexpr bool IsShared() const noexcept
				{
					return m_isShared;
				}

				constexpr void SetShared(bool isShared) noexcept
				{
					m_isShared = isShared;
				}

			private:

				void Terminate();

				ID3D12Resource* LoadTextureFromFile(const char* filePath);

				bool LoadMetadataAndScratchImage(
					const char* filePath,
					DirectX::TexMetadata* metadata,
					DirectX::ScratchImage* scratchImage
				);

				bool CreateUploadBuff(
					ID3D12Device5* device,
					ID3D12Resource** pUploadBuff,
					const size_t alignedRowPitch,
					const size_t imageHeight
				);

				bool CreateTextureBuff(
					ID3D12Device5* device,
					ID3D12Resource** pTextureBuff,
					const DirectX::TexMetadata& metadata
				);

				bool CopyToUploadBuffFromImage(
					ID3D12Resource* uploadBuff,
					const size_t alignedRowPitch,
					const DirectX::Image* image
				);

				void CopyToTextureBuffFromUploadBuff(
					ID3D12Resource* textureBuff,
					ID3D12Resource* uploadBuff,
					const DirectX::TexMetadata& metadata,
					const DirectX::Image* image,
					const size_t alignedRowPitch,
					CGraphicsEngine* graphicsEngine
				);

				void CopyTextureParam(ID3D12Resource* texture);

				void SetDefaultName(const char* filePath);


			private:
				ID3D12Resource* m_texture = nullptr;
				bool m_isCubemap = false;
				UINT16 m_mipLevels = 0;
				DXGI_FORMAT m_format = DXGI_FORMAT_UNKNOWN;
				nsMath::CVector2 m_textureSize = nsMath::CVector2::Zero();
				bool m_isShared = false;
			};
		}
	}
}