#pragma once
namespace nsYMEngine
{
	namespace nsUtils
	{
		struct SNoncopyable
		{
			constexpr SNoncopyable() = default;
			~SNoncopyable() = default;
			constexpr SNoncopyable(const SNoncopyable&) = delete;
			constexpr SNoncopyable& operator=(const SNoncopyable&) = delete;
		};
	}
}