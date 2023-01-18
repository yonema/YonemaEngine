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
		class CDrawInstancingSample : public IGameObject
		{
		private:
			static const char* const m_kModelFilePath;
			static const unsigned int m_kNumInstanceX;
			static const unsigned int m_kNumInstanceZ;
			static const float m_kDistance;

		public:
			bool Start() override final;

			void OnDestroy() override final;

			void Update(float deltaTime) override final;
		public:
			constexpr CDrawInstancingSample() = default;
			~CDrawInstancingSample() = default;

		private:
			CModelRenderer* m_modelRenderer = nullptr;
			nsYMEngine::nsDebugSystem::CSimpleMover* m_simpleMover = nullptr;
		};

	}
}