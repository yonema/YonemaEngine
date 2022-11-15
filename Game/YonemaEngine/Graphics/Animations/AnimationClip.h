#pragma once

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsAnimations
		{
			class CAnimationClip : nsUtils::SNoncopyable
			{
			public:
				constexpr CAnimationClip() = default;
				~CAnimationClip();

				bool Init();

				void Release();

			private:

				void Terminate();

			private:

			};

		}
	}
}