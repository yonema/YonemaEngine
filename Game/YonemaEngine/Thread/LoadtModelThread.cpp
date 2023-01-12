#include "LoadModelThread.h"
#include "../Graphics/Models/BasicModelRenderer.h"
#include "../Graphics/Animations/AnimationClip.h"
#include "../Graphics/Renderers/ModelRenderer.h"
#include "../Graphics/Animations/Skelton.h"

namespace nsYMEngine
{
	namespace nsThread
	{
		CLoadModelThread* CLoadModelThread::m_instance = nullptr;
		bool CLoadModelThread::m_isProcessingThread = false;

		CLoadModelThread::CLoadModelThread()
		{
			Init();

			return;
		}

		CLoadModelThread::~CLoadModelThread()
		{
			Terminate();

			return;
		}

		void CLoadModelThread::Init()
		{
			m_isProcessingThread = true;

			for (unsigned int threadIdx = 0; threadIdx < m_kNumThread; threadIdx++)
			{
				m_thread[threadIdx] = new std::thread(ProcessThread, this, threadIdx);
			}

			return;
		}

		void CLoadModelThread::Terminate()
		{
			m_isProcessingThread = false;

			for (unsigned int threadIdx = 0; threadIdx < m_kNumThread; threadIdx++)
			{
				if (m_thread[threadIdx])
				{
					m_thread[threadIdx]->join();
					delete m_thread[threadIdx];
					m_thread[threadIdx] = nullptr;
				}
			}

			return;
		}


		void CLoadModelThread::Update(unsigned int threadID) noexcept
		{
			if (m_modelAndAnimRef[threadID].empty())
			{
				return;
			}

			auto itr = m_modelAndAnimRef[threadID].begin();
			if (itr == m_modelAndAnimRef[threadID].end())
			{
				return;
			}
			if (itr->modelRef)
			{
				itr->modelRef->InitSynchronous();
			}
			if (itr->animClipRef)
			{
				itr->animClipRef->Init(itr->animFilePath, itr->skeltonRef);
			}
			m_modelAndAnimRef[threadID].erase(itr);

			return;
		}






	}
}