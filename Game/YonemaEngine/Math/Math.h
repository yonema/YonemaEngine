#pragma once
namespace nsYMEngine
{
	namespace nsMath
	{
        static constexpr float YM_PI = 3.141592654f;
        static constexpr float YM_2PI = 6.283185307f;
        static constexpr float YM_1DIVPI = 0.318309886f;
        static constexpr float YM_1DIV2PI = 0.159154943f;
        static constexpr float YM_PIDIV2 = 1.570796327f;
        static constexpr float YM_PIDIV4 = 0.785398163f;

        static constexpr float DegToRad(float degree) noexcept
        {
            return degree * (YM_PI / 180.0f);
        }
        static constexpr float RadToDeg(float radian) noexcept
        {
            return radian * (180.0f / YM_PI);
        }
        static constexpr float Lerpf(float rate, float f0, float f1) noexcept
        {
            return f0 + (f1 - f0) * rate;
        }

	}
}