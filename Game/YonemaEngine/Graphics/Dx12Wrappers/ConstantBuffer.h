#pragma once

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDx12Wrappers
		{

			class CConstantBuffer : private nsUtils::SNoncopyable
			{
			private:
				static const wchar_t* const m_kNamePrefix;

			public:
				constexpr CConstantBuffer() = default;
				~CConstantBuffer();

				/**
				 * @brief ���̊֐��Œ萔�o�b�t�@���쐬������A
				 * CreateConstantBufferView()�֐��ŁA�萔�o�b�t�@�r���[���쐬���Ă��������B
				 * �l��ύX����K�v���Ȃ��Ȃ�AUnmap()�֐��ŁA�A���}�b�v���Ă��������B
				 * @param size �f�[�^�̃T�C�Y
				 * @param objectName �f�o�b�N�p�̃I�u�W�F�N�g�l�[��
				 * @param numCBVs �萔�o�b�t�@�r���[�̐�
				 * @param srcData �f�[�^
				 * @return �萔�o�b�t�@���쐬�ł������H
				*/
				bool Init(
					unsigned int size,
					const wchar_t* objectName = nullptr,
					unsigned int numCBVs = 1,
					const void* srcData = nullptr
				);
				void Release();

				void CreateConstantBufferView(D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHeapHandle);

				/**
				 * @brief �}�b�v���ꂽ�f�[�^�ɃR�s�[����B
				 * �}�b�v�A�A���}�b�v�͂��̊֐����ł͍s���܂���B
				 * @param data �R�s�[���̃f�[�^
				*/
				void CopyToMappedConstantBuffer(const void* data, size_t dataSize);

				constexpr auto GetMappedConstantBuffer() noexcept
				{
					return m_mappedConstantBuffer;
				}

				inline auto GetSizeInByte() noexcept
				{
					return static_cast<unsigned int>(m_constantBuffer->GetDesc().Width) / m_numCBVs;
				}

				inline void Unmap()
				{
					CD3DX12_RANGE readRange(0, 0);
					m_constantBuffer->Unmap(0, nullptr);
					return;
				}

				void SetName(const wchar_t* objectName);

			private:
				void Terminate();

			private:
				ID3D12Resource* m_constantBuffer = nullptr;
				void* m_mappedConstantBuffer = nullptr;
				unsigned int m_numIncrements = 0;
				unsigned int m_numCBVs = 0;
			};

		}
	}
}