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
		class CModelRendererSample : public IGameObject
		{
		private:
			static const char* const m_kModelFilePath;

		public:
			bool Start() override final;

			void OnDestroy() override final;

			void Update(float deltaTime) override final;

		public:
			constexpr CModelRendererSample() = default;
			~CModelRendererSample() = default;

		private:
			void MoveCameraBySimpleMover() noexcept;

		private:
			CModelRenderer* m_modelRenderer = nullptr;
			nsYMEngine::nsDebugSystem::CSimpleMover* m_simpleMover = nullptr;
		};

	}
}