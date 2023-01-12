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
			class CAnimationClip;
			class CSkelton;
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
					nsGraphics::nsAnimations::CAnimationClip* animClipRef,
					const char* animFilePath,
					nsGraphics::nsAnimations::CSkelton* skeltonRef
				)
					:loadProcessType(loadProcessType),
					modelRef(modelRef),
					animClipRef(animClipRef),
					animFilePath(animFilePath),
					skeltonRef(skeltonRef)
				{};

				~SLoadModelProcess() = default;

				EnLoadProcessType loadProcessType = EnLoadProcessType::enLoadModel;
				nsGraphics::nsModels::CBasicModelRenderer* modelRef = nullptr;
				nsGraphics::nsAnimations::CAnimationClip* animClipRef = nullptr;
				const char* animFilePath = nullptr;
				nsGraphics::nsAnimations::CSkelton* skeltonRef = nullptr;
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
				nsGraphics::nsAnimations::CAnimationClip* animClipRef = nullptr,
				const char* animFilePath = nullptr,
				nsGraphics::nsAnimations::CSkelton* skeltonRef = nullptr
			) noexcept
			{
				m_loadModelProcess[m_threadIdx].emplace_back(
					loadProcessType, modelRef, animClipRef, animFilePath, skeltonRef);
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