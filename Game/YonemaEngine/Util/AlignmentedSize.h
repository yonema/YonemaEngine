#pragma once
namespace nsYMEngine
{
	namespace nsUtil
	{
		/**
		 * @brief アライメントにそろえたサイズを返す。
		 * @param[in] size 元のサイズ
		 * @param[in] alignment アライメントサイズ
		 * @return アライメントをそろえたサイズ
		*/
		inline size_t AlignmentedSize(size_t size, size_t alignment)
		{
			return size + alignment - size % alignment;
		}
	}
}