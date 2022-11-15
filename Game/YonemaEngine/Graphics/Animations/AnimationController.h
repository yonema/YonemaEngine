#pragma once
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsAnimations
		{
			class CAnimationController : nsUtils::SNoncopyable
			{
			public:
				constexpr CAnimationController() = default;
				~CAnimationController();

				bool Init();

				void Release();

			private:

				void Terminate();

			private:

			};

		}
	}
}