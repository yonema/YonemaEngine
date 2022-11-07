#pragma once
namespace nsAWA
{
	namespace nsScenes
	{
		class CYonejiDebugScene : public IGameObject
		{
		public:
			bool Start() override final;

			void OnDestroy() override final;

			void Update(float deltaTime) override final;

		public:
			constexpr CYonejiDebugScene() = default;
			~CYonejiDebugScene() = default;

		private:
			CModelRenderer* m_modelRenderer = nullptr;
		};

	}
}