#pragma once
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsAnimations
		{
			class CAnimationController;
		}
	}
}
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsAnimations
		{
			class CAnimator : nsUtils::SNoncopyable
			{
			public:
				constexpr CAnimator() = default;
				~CAnimator();

				bool Init(
					unsigned int numAnimations,
					const char* animationFilePaths[],
					CAnimationController* animationController = nullptr
					);

				void Release();

			private:

				void Terminate();

			private:

			};

		}
	}
}