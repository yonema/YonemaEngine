#pragma once
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
			CModelRenderer* m_modelRenderer = nullptr;
		};

	}
}