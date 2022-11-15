#pragma once
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsAnimations
		{
			class CAnimationExecutor : nsUtils::SNoncopyable
			{
			public:
				constexpr CAnimationExecutor() = default;
				~CAnimationExecutor();

				bool Init();

				void Release();

			private:

				void Terminate();

			private:

			};

		}
	}
}