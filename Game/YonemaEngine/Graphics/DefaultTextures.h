#pragma once
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDx12Wrappers
		{
			class CTexture;
		}
	}
}

namespace nsYMEngine
{
	namespace nsGraphics
	{
		class CDefaultTextures : private nsUtils::SNoncopyable
		{
		public:
			enum EnTexType
			{
				enWhite,
				enBlack,
				enDiffuse,
				enNormal,
				enNum
			};
			static const unsigned int m_kNumTextures = static_cast<int>(EnTexType::enNum);
			static const char* const m_kTextureFilePath[m_kNumTextures];

		public:
			constexpr CDefaultTextures() = default;
			~CDefaultTextures();

			static constexpr const char* const GetTextureFilePath(EnTexType texType) noexcept
			{
				return m_kTextureFilePath[static_cast<int>(texType)];
			}

			void Init();

			void Release();

			constexpr auto* GetTexture(EnTexType texType) const noexcept
			{
				return m_textures[static_cast<int>(texType)];
			}

		private:

			void Terminate();

		private:
			nsDx12Wrappers::CTexture* m_textures[m_kNumTextures] = {};
		};

	}
}