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
		class CCharacterControllerSample : public IGameObject
		{
		public:
			bool Start() override final;

			void OnDestroy() override final;

			void Update(float deltaTime) override final;

		public:
			constexpr CCharacterControllerSample() = default;
			~CCharacterControllerSample() = default;

		private:
			CPhysicsStaticObject m_staticGround;
			CPhysicsStaticObject m_staticWall[3];
			CModelRenderer* m_modelRenderer = nullptr;
			CCharacterController* m_charaCon = nullptr;
			nsYMEngine::nsDebugSystem::CSimpleMover* m_simpleMover = nullptr;
			nsMath::CVector3 m_prevPos = nsMath::CVector3::Zero();

			CFontRenderer* m_debugFR = nullptr;
		};

	}
}