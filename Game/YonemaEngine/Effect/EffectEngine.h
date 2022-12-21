#pragma once
#include <Effekseer.h>
#include <EffekseerRendererDX12.h>

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDx12Wrappers
		{
			class CCommandList;
		}
	}
}

namespace nsYMEngine
{
	namespace nsEffect
	{
		class CEffectEngine : nsUtils::SNoncopyable
		{
		private:
			static const unsigned int m_kNumBackBuffers = 2;
			static const unsigned int m_kNumRenderTargets = 1;
			static const unsigned int m_kMaxParticles = 10000;
			static const unsigned int m_kMaxInstance = 5000;

		private:
			CEffectEngine();
			~CEffectEngine();

		public:
			static inline CEffectEngine* CreateInstance() noexcept
			{
				if (m_instance == nullptr)
				{
					m_instance = new CEffectEngine();
				}
				return m_instance;
			}

			static inline CEffectEngine* GetInstance() noexcept
			{
				return m_instance;
			}

			static void DeleteInstance() noexcept
			{
				if (m_instance == nullptr)
				{
					return;
				}
				delete m_instance;
				m_instance = nullptr;
			}


			void Draw(nsGraphics::nsDx12Wrappers::CCommandList* commandList);

			const Effekseer::EffectRef* RegisterEffect(const wchar_t* filePath);

			int Play(const Effekseer::EffectRef* effectRef, const nsMath::CVector3& position);

			int Play(const Effekseer::EffectRef* effectRef, const nsMath::CVector3& position, int startFrame);

			bool IsPlaying(int handle) const;

			void Stop(int handle);

			void Pause(int handle);

			bool IsPaused(int handle) const;

			void SetSpeed(int handle, float speed);

			float GetSpeed(int handle) const;

			void SetPosition(int handle, const nsMath::CVector3& position);
			void SetRotation(int handle, const nsMath::CVector3& axis, float radAngle);
			void SetScale(int handle, const nsMath::CVector3& scale);

		private:

			bool Init();

			void Terminate();

			void UpdateViewAndProjMatrix();

		private:
			static CEffectEngine* m_instance;
			Effekseer::ManagerRef m_efkManager = nullptr;
			EffekseerRenderer::RendererRef m_efkRenderer = nullptr;
			Effekseer::RefPtr<EffekseerRenderer::SingleFrameMemoryPool> m_efkMemoryPool = nullptr;
			Effekseer::RefPtr<EffekseerRenderer::CommandList> m_efkCommandList = nullptr;

			std::unordered_map<std::wstring, Effekseer::EffectRef> m_effectMap = {};
			int m_totalEffectKey = 0;
			Effekseer::Handle m_efkHandle = 0;
		};
	}
}
