#pragma once
namespace nsYMEngine
{
	namespace nsUtils
	{
		/**
		 * @brief �A���C�����g�ɂ��낦���T�C�Y��Ԃ��B
		 * @param[in] size ���̃T�C�Y
		 * @param[in] alignment �A���C�����g�T�C�Y
		 * @return �A���C�����g�����낦���T�C�Y
		*/
		inline size_t AlignmentedSize(size_t size, size_t alignment)
		{
			return size + alignment - size % alignment;
		}
	}
}