#include "EffectEngine.h"
#include "../Graphics/GraphicsEngine.h"
#include "../Graphics/Dx12Wrappers/CommandList.h"
#include "../Sound/SoundEngine.h"
#include "EffectSound.h"


namespace nsYMEngine
{
	namespace nsEffect
	{
		CEffectEngine* CEffectEngine::m_instance = nullptr;

		CEffectEngine::CEffectEngine()
		{
			Init();

			return;
		}

		CEffectEngine::~CEffectEngine()
		{
			Terminate();

			return;
		}

		void CEffectEngine::Terminate()
		{
			m_efkManager->StopAllEffects();
			m_effectMap.clear();

			return;
		}

		bool CEffectEngine::Init()
		{
			auto* device = nsGraphics::CGraphicsEngine::GetInstance()->GetDevice();
			auto* commandQueue = nsGraphics::CGraphicsEngine::GetInstance()->GetCommandQueue();
			DXGI_FORMAT bbFormats[m_kNumRenderTargets] = { DXGI_FORMAT_R8G8B8A8_UNORM };
			m_efkRenderer = EffekseerRendererDX12::Create(
				static_cast<ID3D12Device*>(device),
				commandQueue,
				m_kNumBackBuffers,
				bbFormats,
				m_kNumRenderTargets,
				DXGI_FORMAT_D32_FLOAT,
				false,
				m_kMaxParticles
				);

			m_efkManager = Effekseer::Manager::Create(m_kMaxInstance);

			m_efkManager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);

			m_efkManager->SetSpriteRenderer(m_efkRenderer->CreateSpriteRenderer());
			m_efkManager->SetRibbonRenderer(m_efkRenderer->CreateRibbonRenderer());
			m_efkManager->SetRingRenderer(m_efkRenderer->CreateRingRenderer());
			m_efkManager->SetTrackRenderer(m_efkRenderer->CreateTrackRenderer());
			m_efkManager->SetModelRenderer(m_efkRenderer->CreateModelRenderer());

			m_efkManager->SetTextureLoader(m_efkRenderer->CreateTextureLoader());
			m_efkManager->SetModelLoader(m_efkRenderer->CreateModelLoader());

			m_efkMemoryPool = EffekseerRenderer::CreateSingleFrameMemoryPool(
				m_efkRenderer->GetGraphicsDevice());
			m_efkCommandList = EffekseerRenderer::CreateCommandList(
					m_efkRenderer->GetGraphicsDevice(), m_efkMemoryPool);

			m_efkRenderer->SetCommandList(m_efkCommandList);


			auto* xAudio = nsSound::CSoundEngine::GetInstance()->GetXAudio();

			auto sound = Effekseer::MakeRefPtr<EffekseerSound::SoundImplemented>();
			sound->Initialize(xAudio, 16, 16);
			m_efkSound = sound;


			m_efkSoundLoader = ::Effekseer::MakeRefPtr<CEffectSoundLoader>(sound);
			m_efkManager->SetSoundLoader(m_efkSoundLoader);
			//m_efkManager->SetSoundLoader(m_efkSound->CreateSoundLoader());

			m_efkSoundPlayer = ::Effekseer::MakeRefPtr<CEffectSoundPlayer>(sound);
			m_efkManager->SetSoundPlayer(m_efkSoundPlayer);
			//m_efkManager->SetSoundPlayer(m_efkSound->CreateSoundPlayer());
			




			return true;
		}

		void CEffectEngine::Draw(nsGraphics::nsDx12Wrappers::CCommandList* commandList)
		{
			UpdateViewAndProjMatrix();

			m_efkManager->Update();

			m_efkMemoryPool->NewFrame();

			EffekseerRendererDX12::BeginCommandList(m_efkCommandList, commandList->Get());

			m_efkRenderer->BeginRendering();
			m_efkManager->Draw();
			m_efkRenderer->EndRendering();

			EffekseerRendererDX12::EndCommandList(m_efkCommandList);

			return;
		}

		void CEffectEngine::UpdateViewAndProjMatrix()
		{
			const auto* camera = nsGraphics::CGraphicsEngine::GetInstance()->GetMainCamera();
			const auto& mView = camera->GetViewMatirx();
			const auto& mProj = camera->GetProjectionMatirx();

			Effekseer::Matrix44 efkMView = {};
			Effekseer::Matrix44 efkMProj = {};

			for (int y = 0; y < 4; y++)
			{
				for (int x = 0; x < 4; x++)
				{
					efkMView.Values[y][x] = mView.m_fMat[y][x];
					efkMProj.Values[y][x] = mProj.m_fMat[y][x];
				}
			}

			m_efkRenderer->SetCameraMatrix(efkMView);
			m_efkRenderer->SetProjectionMatrix(efkMProj);

			return;
		}

		const Effekseer::EffectRef* CEffectEngine::RegisterEffect(const wchar_t* filePath)
		{
			auto itr = m_effectMap.find(filePath);

			if (itr == m_effectMap.end())
			{
				Effekseer::EffectRef efkRef = Effekseer::Effect::Create(
					m_efkManager, reinterpret_cast<const EFK_CHAR*>(filePath));
				m_effectMap.emplace(filePath, efkRef);

				itr = m_effectMap.find(filePath);
			}

			return &itr->second;
		}

		int CEffectEngine::Play(const Effekseer::EffectRef* effectRef, const nsMath::CVector3& position)
		{
			return m_efkManager->Play(*effectRef, position.x, position.y, position.z);
		}

		int CEffectEngine::Play(const Effekseer::EffectRef* effectRef, const nsMath::CVector3& position, int startFrame)
		{
			return m_efkManager->Play(*effectRef, { position.x, position.y, position.z }, startFrame);
		}

		bool CEffectEngine::IsPlaying(int handle) const
		{
			return m_efkManager->Exists(handle);
		}


		void CEffectEngine::Stop(int handle)
		{
			m_efkManager->StopEffect(handle);
			return;
		}

		void CEffectEngine::Pause(int handle)
		{
			m_efkManager->SetPaused(handle, true);
			return;
		}

		bool CEffectEngine::IsPaused(int handle) const
		{
			return m_efkManager->GetPaused(handle);
		}

		void CEffectEngine::SetSpeed(int handle, float speed)
		{
			m_efkManager->SetSpeed(handle, speed);
			return;
		}

		float CEffectEngine::GetSpeed(int handle) const 
		{
			return m_efkManager->GetSpeed(handle);
		}

		void CEffectEngine::SetPosition(int handle, const nsMath::CVector3& position)
		{
			m_efkManager->SetLocation(handle, { position.x, position.y, position.z });
			return;
		}

		void CEffectEngine::SetRotation(int handle, const nsMath::CVector3& axis, float radAngle)
		{
			m_efkManager->SetRotation(handle, { axis.x, axis.y, axis.z }, radAngle);
			return;
		}

		void CEffectEngine::SetScale(int handle, const nsMath::CVector3& scale)
		{
			m_efkManager->SetScale(handle, scale.x, scale.y, scale.z);
			return;
		}



	}
}