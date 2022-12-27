#include "WaveFile.h"
#include <xaudio2.h>
#include "SoundEngine.h"

namespace nsYMEngine
{
	namespace nsSound
	{
#ifdef _XBOX	//Big-Endian
		const DWORD CWaveFile::m_kFourccRIFF = 'RIFF';
		const DWORD CWaveFile::m_kFourccDATA = 'data';
		const DWORD CWaveFile::m_kFourccFMT = 'fmt ';
		const DWORD CWaveFile::m_kFourccWAVE = 'WAVE';
		const DWORD CWaveFile::m_kFourccXWMA = 'XWMA';
		const DWORD CWaveFile::m_kFourccDPDS = 'dpds';
#else			//Little-Endian
		const DWORD CWaveFile::m_kFourccRIFF = 'FFIR';
		const DWORD CWaveFile::m_kFourccDATA = 'atad';
		const DWORD CWaveFile::m_kFourccFMT = ' tmf';
		const DWORD CWaveFile::m_kFourccWAVE = 'EVAW';
		const DWORD CWaveFile::m_kFourccXWMA = 'AMWX';
		const DWORD CWaveFile::m_kFourccDPDS = 'sdpd';
#endif

		CWaveFile::~CWaveFile()
		{
			Terminate();

			return;
		}

		void CWaveFile::Terminate()
		{
			Release();
			return;
		}

		void CWaveFile::Release()
		{
			if (m_waveData != nullptr)
			{
				delete m_waveData;
				m_waveData = nullptr;
			}

			if (m_xAudioBuffer != nullptr)
			{
				delete m_xAudioBuffer;
				m_xAudioBuffer = nullptr;
			}

			if (m_formatex != nullptr)
			{
				delete m_formatex;
				m_formatex = nullptr;
			}

			return;
		}

		bool CWaveFile::Init(const wchar_t* filePath)
		{
			Release();

			HANDLE hFile = {};
			
			bool res = OpenWaveFile(&hFile, filePath);
			if (res != true)
			{
				std::wstring str(filePath);
				str.erase(str.end() - 1);
				str += L"のwaveファイルのオープンに失敗しました。";
				nsGameWindow::MessageBoxWarning(str.c_str());
				return false;
			}

			DWORD dwChunkSize;
			DWORD dwChunkPosition;

			// オーディオ ファイル内の 'RIFF' チャンクを見つけて、ファイルの種類を確認します。
			//check the file type, should be fourccWAVE or 'XWMA'
			auto hRes = FindChunk(hFile, m_kFourccRIFF, dwChunkSize, dwChunkPosition);
			if (FAILED(hRes))
			{
				return false;
			}
			DWORD filetype;
			hRes = ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition);
			if (FAILED(hRes) || filetype != m_kFourccWAVE)
			{
				return false;
			}

			// 'fmt' チャンクを見つけて、その内容を WAVEFORMATEXTENSIBLE 構造体にコピーします。
			hRes = FindChunk(hFile, m_kFourccFMT, dwChunkSize, dwChunkPosition);
			if (FAILED(hRes))
			{
				return false;
			}
			m_formatex = new WAVEFORMATEX{};
			hRes = ReadChunkData(hFile, m_formatex, dwChunkSize, dwChunkPosition);
			if (FAILED(hRes))
			{
				return false;
			}

			// 'data' チャンクを見つけて、その内容をバッファーに読み取ります。
			//fill out the audio data buffer with the contents of the fourccDATA chunk
			hRes = FindChunk(hFile, m_kFourccDATA, dwChunkSize, dwChunkPosition);
			if (FAILED(hRes))
			{
				return false;
			}
			m_waveData = new BYTE[dwChunkSize];
			hRes = ReadChunkData(hFile, m_waveData, dwChunkSize, dwChunkPosition);
			if (FAILED(hRes))
			{
				return false;
			}

			m_xAudioBuffer = new XAUDIO2_BUFFER{};
			// XAUDIO2_BUFFER構造体に値を設定します。
			m_xAudioBuffer->AudioBytes = dwChunkSize;  //size of the audio buffer in bytes
			m_xAudioBuffer->pAudioData = m_waveData;  //buffer containing audio data
			m_xAudioBuffer->Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer


			return true;
		}


		bool CWaveFile::OpenWaveFile(HANDLE* pHFile, const wchar_t* filePath)
		{
			*pHFile = CreateFile(
				filePath,
				GENERIC_READ,
				FILE_SHARE_READ,
				NULL,
				OPEN_EXISTING,
				0,
				NULL
			);

			if (*pHFile == INVALID_HANDLE_VALUE)
			{
				return static_cast<bool>(HRESULT_FROM_WIN32(GetLastError()));
			}

			if (SetFilePointer(*pHFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
			{
				return static_cast<bool>(HRESULT_FROM_WIN32(GetLastError()));
			}

			return true;
		}


		HRESULT CWaveFile::FindChunk(
			HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition)
		{
			HRESULT hr = S_OK;
			if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
			{
				return HRESULT_FROM_WIN32(GetLastError());
			}

			DWORD dwChunkType;
			DWORD dwChunkDataSize;
			DWORD dwRIFFDataSize = 0;
			DWORD dwFileType;
			DWORD bytesRead = 0;
			DWORD dwOffset = 0;

			while (hr == S_OK)
			{
				DWORD dwRead;
				if (ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL) == FALSE)
				{
					hr = HRESULT_FROM_WIN32(GetLastError());
				}

				if (ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL) == FALSE)
				{
					hr = HRESULT_FROM_WIN32(GetLastError());
				}

				switch (dwChunkType)
				{
				case m_kFourccRIFF:
					dwRIFFDataSize = dwChunkDataSize;
					dwChunkDataSize = 4;
					if (ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL) == FALSE)
					{
						hr = HRESULT_FROM_WIN32(GetLastError());
					}

					break;

				default:
					if (SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT) ==
						INVALID_SET_FILE_POINTER)
					{
						return HRESULT_FROM_WIN32(GetLastError());
					}
					break;
				}

				dwOffset += sizeof(DWORD) * 2;

				if (dwChunkType == fourcc)
				{
					// 目的のチャンクが見つかった。
					dwChunkSize = dwChunkDataSize;
					dwChunkDataPosition = dwOffset;
					return S_OK;
				}

				dwOffset += dwChunkDataSize;

				if (bytesRead >= dwRIFFDataSize)
				{
					return S_FALSE;
				}

			}

			return S_OK;

		}

		HRESULT CWaveFile::ReadChunkData(
			HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset)
		{
			HRESULT hr = S_OK;
			if (SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
			{
				return HRESULT_FROM_WIN32(GetLastError());
			}

			DWORD dwRead;
			if (ReadFile(hFile, buffer, buffersize, &dwRead, NULL) == FALSE)
			{
				hr = HRESULT_FROM_WIN32(GetLastError());
			}

			return hr;
		}


	}
}