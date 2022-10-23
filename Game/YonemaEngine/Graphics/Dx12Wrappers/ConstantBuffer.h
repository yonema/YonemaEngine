#pragma once
#include "DescriptorHeap.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDx12Wrappers
		{

			class CConstantBuffer : private nsUtils::SNoncopyable
			{
			public:
				constexpr CConstantBuffer() = default;
				~CConstantBuffer();

				/**
				 * @brief ���̊֐��Œ萔�o�b�t�@���쐬������A
				 * CreateConstantBufferView()�֐��ŁA�萔�o�b�t�@�r���[���쐬���Ă��������B
				 * �l��ύX����K�v���Ȃ��Ȃ�AUnmap()�֐��ŁA�A���}�b�v���Ă��������B
				 * @param size �f�[�^�̃T�C�Y
				 * @param numCBVs �萔�o�b�t�@�r���[�̐�
				 * @param srcData �f�[�^
				 * @return �萔�o�b�t�@���쐬�ł������H
				*/
				bool Init(
					unsigned int size,
					unsigned int numCBVs = 1,
					const void* srcData = nullptr
				);
				void Release();

				void CreateConstantBufferView(D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHeapHandle);

				void CopyToMappedConstantBuffer(const void* data);

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