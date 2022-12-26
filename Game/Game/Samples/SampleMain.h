#pragma once
namespace nsAWA
{
	namespace nsSamples
	{
		class CSampleMain : public IGameObject
		{
		public:
			bool Start() override final;

			void OnDestroy() override final;

			void Update(float deltaTime) override final;

		public:
			constexpr CSampleMain() = default;
			~CSampleMain() = default;

		private:
			IGameObject* m_sampleGO = nullptr;
		};
	}
}