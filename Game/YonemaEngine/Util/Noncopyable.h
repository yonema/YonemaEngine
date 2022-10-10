#pragma once
namespace nsYMEngine
{
	namespace nsUtil
	{
		struct SNoncopyable
		{
			SNoncopyable() = default;
			~SNoncopyable() = default;
			SNoncopyable(const SNoncopyable&) = delete;
			SNoncopyable& operator=(const SNoncopyable&) = delete;
		};
	}
}