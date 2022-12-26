#pragma once
#include "../../Game/SubmixType.h"

struct tWAVEFORMATEX;
typedef tWAVEFORMATEX WAVEFORMATEX;
struct XAUDIO2_BUFFER;

namespace nsYMEngine
{
	namespace nsSound
	{
		class CWaveFile : private nsUtils::SNoncopyable
		{
		private:
			static const DWORD m_kFourccRIFF;
			static const DWORD m_kFourccDATA;
			static const DWORD m_kFourccFMT;
			static const DWORD m_kFourccWAVE;
			static const DWORD m_kFourccXWMA;
			static const DWORD m_kFourccDPDS;

		public:
			constexpr CWaveFile() = default;
			~CWaveFile();

			bool Init(const wchar_t* filePath);

			void Release();

			constexpr bool IsValid() const noexcept
			{
				return m_waveData != nullptr ? true : false;
			}

			constexpr const auto* GetXAudioBuffer() const noexcept
			{
				return m_xAudioBuffer;
			}

			constexpr const auto* GetFormatex() const noexcept
			{
				return m_formatex;
			}

		private:
			void Terminate();

			bool OpenWaveFile(HANDLE* pHFile, const wchar_t* filePath);

			HRESULT FindChunk(
				HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition);

			HRESULT ReadChunkData(
				HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset);

		private:
			BYTE* m_waveData = nullptr;
			XAUDIO2_BUFFER* m_xAudioBuffer = nullptr;
			WAVEFORMATEX* m_formatex = nullptr;
		};

	}
}