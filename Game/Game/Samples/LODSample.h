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
		class CLODSample : public IGameObject
		{
		private:
			enum class EnAnimType
			{
				enIdle,
				enNum
			};
			static const int m_kNumModels = 19;
			static const char* m_kModelFilePath[m_kNumModels];
			static const char* m_kLODModelFilePath;
			static const char* m_kAnimFilePaths[static_cast<int>(EnAnimType::enNum)];

		public:
			bool Start() override final;

			void OnDestroy() override final;

			void Update(float deltaTime) override final;
		public:
			constexpr CLODSample() = default;
			~CLODSample() = default;

		private:
			CModelRenderer* m_modelRenderer = nullptr;
			nsYMEngine::nsDebugSystem::CSimpleMover* m_simpleMover = nullptr;
		};

	}
}