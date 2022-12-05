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
		class CPhysicsSample : public IGameObject
		{
		private:
			static const int m_kNumPhysicsObjects = 100;

		public:
			bool Start() override final;

			void OnDestroy() override final;

			void Update(float deltaTime) override final;

		public:
			constexpr CPhysicsSample() = default;
			~CPhysicsSample() = default;

		private:
			CPhysicsStaticObject m_staticPlane;
			CPhysicsDynamicObject m_dynamicBox[m_kNumPhysicsObjects];
			CPhysicsDynamicObject m_dynamicSphere[m_kNumPhysicsObjects];
			CPhysicsDynamicObject m_dynamicCapusule[m_kNumPhysicsObjects];
			CPhysicsStaticObject m_staticPlayer;

			nsYMEngine::nsDebugSystem::CSimpleMover* m_simpleMover = nullptr;
		};

	}
}