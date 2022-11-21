#pragma once
namespace nsAWA
{
	namespace nsScenes
	{
		namespace nsSample
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

			};
		}

	}
}