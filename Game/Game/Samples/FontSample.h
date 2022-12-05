#pragma once
namespace nsAWA
{
	namespace nsSamples
	{
		class CFontSample : public IGameObject
		{
		public:
			bool Start() override final;

			void OnDestroy() override final;

			void Update(float deltaTime) override final;

		public:
			constexpr CFontSample() = default;
			~CFontSample() = default;

		private:
			std::vector<CFontRenderer*> m_fontRenderers = { nullptr };
		};
	}
}