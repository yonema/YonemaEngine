#pragma once
namespace nsYMEngine
{
	namespace nsUtils
	{
		/**
		 * @brief データサイズをアライメントする
		 * @param[in] size 元のサイズ
		 * @param[in] alignment アライメントサイズ
		 * @return アライメントしたデータサイズ
		*/
		constexpr size_t AlignSize(size_t size, size_t alignment) noexcept
		{
			return (size % alignment == 0)? size : size + alignment - size % alignment;
		}
	}
}