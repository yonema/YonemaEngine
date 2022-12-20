#pragma once
namespace nsAWA
{
	namespace nsSamples
	{
		class CAnimationEventSample : public IGameObject
		{
		private:
			enum class EnAnimType
			{
				enAnimWithEvent,
				enNum
			};
			static const char* m_kModelFilePath;
			static const char* m_kAnimFilePaths[static_cast<int>(EnAnimType::enNum)];
			static const unsigned int m_kNumAnimEvent = 3;

		public:
			bool Start() override final;

			void OnDestroy() override final;

			void Update(float deltaTime) override final;

		public:
			constexpr CAnimationEventSample() = default;
			~CAnimationEventSample() = default;

		private:

			void AnimEventFunc1() noexcept;
			void AnimEventFunc2() noexcept;
			void AnimEventFunc3() noexcept;

		private:
			CModelRenderer* m_modelRenderer = nullptr;

			CFontRenderer* m_eventCounterFR[m_kNumAnimEvent] = {};
			int m_eventCounter[m_kNumAnimEvent] = {};
		};
	}
}
