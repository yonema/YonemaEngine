#pragma once
namespace nsAWA
{
	namespace nsScenes
	{
		namespace nsSample
		{
			class CSkeltalAnimationSample : public IGameObject
			{
			private:
				enum class EnAnimType
				{
					enRumba,
					enHipHop,
					enRobot,
					enNum
				};
				static const char* m_kModelFilePath;
				static const char* m_kAnimFilePaths[static_cast<int>(EnAnimType::enNum)];

			public:
				bool Start() override final;

				void OnDestroy() override final;

				void Update(float deltaTime) override final;

			public:
				constexpr CSkeltalAnimationSample() = default;
				~CSkeltalAnimationSample() = default;

			private:
				CModelRenderer* m_modelRenderer = nullptr;
			};
		}

	}
}