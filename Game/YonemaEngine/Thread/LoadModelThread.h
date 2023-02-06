#pragma once
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsRenderers
		{
			struct SModelInitData;
		}
		namespace nsModels
		{
			class CBasicModelRenderer;
		}
		namespace nsAnimations
		{
			class CAnimator;
			class CAnimationClip;
		}
	}
}

namespace nsYMEngine
{
	namespace nsThread
	{
		class CLoadModelThread : private nsUtils::SNoncopyable
		{
		public:
			enum class EnLoadProcessType
			{
				enLoadModel,
				enLoadAnim
			};
		private:
			static const unsigned int m_kNumThread = 3;

			struct SLoadModelProcess
			{
				constexpr SLoadModelProcess() = default;
				constexpr SLoadModelProcess(
					EnLoadProcessType loadProcessType,
					nsGraphics::nsModels::CBasicModelRenderer* modelRef,
					std::shared_ptr<nsGraphics::nsAnimations::CAnimator>* animator,
					nsGraphics::nsAnimations::CAnimationClip* animClipRef,
					const char* animFilePath,
					bool registerAnimBank
				)
					:loadProcessType(loadProcessType),
					modelRef(modelRef),
					animatorRef(animator),
					animClipRef(animClipRef),
					animFilePath(animFilePath),
					registerAnimBank(registerAnimBank)
				{};

				~SLoadModelProcess() = default;

				EnLoadProcessType loadProcessType = EnLoadProcessType::enLoadModel;
				nsGraphics::nsModels::CBasicModelRenderer* modelRef = nullptr;
				std::shared_ptr<nsGraphics::nsAnimations::CAnimator>* animatorRef = nullptr;
				nsGraphics::nsAnimations::CAnimationClip* animClipRef = nullptr;
				const char* animFilePath = nullptr;
				bool registerAnimBank = false;
			};

		private:
			CLoadModelThread();
			~CLoadModelThread();

		public:

			static CLoadModelThread* CreateInstance()
			{
				if (m_instance == nullptr)
				{
					m_instance = new CLoadModelThread();
				}

				return m_instance;
			}

			static constexpr CLoadModelThread* GetInstance() noexcept
			{
				return m_instance;
			}

			static void DeleteInstance()
			{
				if (m_instance != nullptr)
				{
					delete m_instance;
					m_instance = nullptr;
				}
			}

			static void ProcessThread(CLoadModelThread* instance, unsigned int threadID) noexcept
			{
				while (m_isProcessingThread)
				{
					instance->Update(threadID);
				}

				return;
			}




			inline void PushLoadModelProcess(
				EnLoadProcessType loadProcessType,
				nsGraphics::nsModels::CBasicModelRenderer* modelRef,
				std::shared_ptr<nsGraphics::nsAnimations::CAnimator>* animator = nullptr,
				nsGraphics::nsAnimations::CAnimationClip* animClipRef = nullptr,
				const char* animFilePath = nullptr,
				bool registerAnimBank = false
			) noexcept
			{
				m_loadModelProcess[m_threadIdx].emplace_back(
					loadProcessType, modelRef, animator, animClipRef, animFilePath, registerAnimBank);
				m_threadIdx++;
				if (m_threadIdx >= m_kNumThread)
				{
					m_threadIdx = 0;
				}
			}

		private:

			void Init();

			void Terminate();

			void Update(unsigned int threadID) noexcept;

		private:
			static CLoadModelThread* m_instance;
			static bool m_isProcessingThread;
			std::array<std::thread*, m_kNumThread> m_thread = {};
			std::array <std::list<SLoadModelProcess>, m_kNumThread>
				m_loadModelProcess = {};
			unsigned int m_threadIdx = 0;
		};

	}
}