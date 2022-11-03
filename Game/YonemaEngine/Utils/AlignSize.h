#pragma once
namespace nsYMEngine
{
	namespace nsUtils
	{
		/**
		 * @brief �f�[�^�T�C�Y���A���C�����g����
		 * @param[in] size ���̃T�C�Y
		 * @param[in] alignment �A���C�����g�T�C�Y
		 * @return �A���C�����g�����f�[�^�T�C�Y
		*/
		constexpr size_t AlignSize(size_t size, size_t alignment) noexcept
		{
			return (size % alignment == 0)? size : size + alignment - size % alignment;
		}
	}
}