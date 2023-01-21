#pragma once
namespace nsYMEngine
{
	namespace nsDebugSystem
	{
		class CSimpleMover;
	}
}

namespace nsAWA
{
	namespace nsSamples
	{
		class CShareAnimationSample : public IGameObject
		{
		private:
			enum class EnAnimType
			{
				enIdle,
				enJumpAttack,
				enNum
			};
			static const int m_kNumModels = 3;
			static const char* m_kModelFilePath[m_kNumModels];
			static const char* m_kAnimFilePaths[static_cast<int>(EnAnimType::enNum)];

		public:
			bool Start() override final;

			void OnDestroy() override final;

			void Update(float deltaTime) override final;
		public:
			constexpr CShareAnimationSample() = default;
			~CShareAnimationSample() = default;

		private:
			CModelRenderer* m_modelRenderer[m_kNumModels] = {};
			nsYMEngine::nsDebugSystem::CSimpleMover* m_simpleMover = nullptr;
		};
	}
}

