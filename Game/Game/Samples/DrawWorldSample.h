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
		class CDrawWorldSample : public IGameObject
		{
		public:
			bool Start() override final;

			void OnDestroy() override final;

			void Update(float deltaTime) override final;
		public:
			constexpr CDrawWorldSample() = default;
			~CDrawWorldSample() = default;

		private:
			CLevel3D m_level3D = {};
			nsYMEngine::nsDebugSystem::CSimpleMover* m_simpleMover = nullptr;

		};

	}
}