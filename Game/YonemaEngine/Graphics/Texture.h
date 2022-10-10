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
		class CTexture : private nsUtil::SNoncopyable
		{
		private:
			static const int m_kNumWICFileExtensions = 5;
			static const char* m_kWICFileExtensions[m_kNumWICFileExtensions];
			static const char* m_kTGAFileExtension;
			static const char* m_kDDSFileExtension;

		public:
			CTexture() = default;
			~CTexture();

			void Init(const char* filePath);

			inline bool IsValid() const
			{
				return m_texture != nullptr;
			}

			ID3D12Resource* GetResource()
			{
				return m_texture;
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
				const size_t alignmentedRowPitch,
				const size_t imageHeight
			);

			bool CreateTextureBuff(
				ID3D12Device5* device,
				ID3D12Resource** pTextureBuff,
				const DirectX::TexMetadata& metadata
				);

			bool CopyToUploadBuffFromImage(
				ID3D12Resource* uploadBuff,
				const size_t alignmentedRowPitch,
				const DirectX::Image* image
			);

			void CopyToTextureBuffFromUploadBuff(
				ID3D12Resource* textureBuff,
				ID3D12Resource* uploadBuff,
				const DirectX::TexMetadata& metadata,
				const DirectX::Image* image,
				const size_t alignmentedRowPitch,
				CGraphicsEngine* graphicsEngine
			);

		private:
			ID3D12Resource* m_texture = nullptr;
		};

	}
}