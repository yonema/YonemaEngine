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
		class CLoadAsynchronousSample : public IGameObject
		{
		private:
			enum class EnAnimType
			{
				enIdle,
				enRumba,
				enHipHop,
				enRobot,
				enNum
			};
			static const char* m_kModelFilePath;
			static const char* m_kAnimFilePaths[static_cast<int>(EnAnimType::enNum)];

		public:
			bool Start() override final;

			void OnDestroy() override final;

			void Update(float deltaTime) override final;
		public:
			constexpr CLoadAsynchronousSample() = default;
			~CLoadAsynchronousSample() = default;

		private:
			CModelRenderer* m_modelRenderer = nullptr;
			CModelRenderer* m_boxMR = nullptr;
			SAnimationInitData m_animationInitData = {};
			nsYMEngine::nsDebugSystem::CSimpleMover* m_simpleMover = nullptr;
		};

	}
}