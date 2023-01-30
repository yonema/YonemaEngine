#include "DrawWorldSample.h"
#include "../../YonemaEngine/DebugSystem/SimpleMover.h"

namespace nsAWA
{
	namespace nsSamples
	{
		const std::unordered_map<std::string, const char* const> CHumans::m_kNameToFilePath =
		{
			{ "Base", "Assets/Models/Humans/Base.fbx" },
			{ "Player1", "Assets/Models/Humans/Player1.fbx" },
			{ "Player2", "Assets/Models/Humans/Player2.fbx" },
			{ "Player3", "Assets/Models/Humans/Player3.fbx" },
			{ "Goddess", "Assets/Models/Humans/Goddess.fbx" },
			{ "Chris", "Assets/Models/Humans/Chris.fbx" },
			{ "Mary", "Assets/Models/Humans/Mary.fbx" },
			{ "Rachel", "Assets/Models/Humans/Rachel.fbx" },
			{ "Barbara", "Assets/Models/Humans/Barbara.fbx" },
			{ "Michelle", "Assets/Models/Humans/Michelle.fbx" },
			{ "John", "Assets/Models/Humans/John.fbx" },
			{ "Alain", "Assets/Models/Humans/Alain.fbx" },
			{ "Sara", "Assets/Models/Humans/Sara.fbx" },
			{ "Tom", "Assets/Models/Humans/Tom.fbx" },
			{ "James", "Assets/Models/Humans/James.fbx" },
			{ "Kevin", "Assets/Models/Humans/Kevin.fbx" },
			{ "Guardian", "Assets/Models/Humans/Guardian.fbx" },
			{ "MobMan", "Assets/Models/Humans/MobMan.fbx" },
			{ "MobWoman", "Assets/Models/Humans/MobWoman.fbx" },
		};
		const char* const CHumans::
			m_kAnimFilePaths[static_cast<int>(EnAnimType::enNum)] =
		{
			"Assets/Animations/Player/Sword_Idle.fbx",
			//"Assets/Animations/Player/Sword_JumpAttack.fbx"
		};
		const char* const CHumans::m_kRetargetSkeltonName= "PlayerSkelton";
		const float CHumans::m_kModelScale = 0.1f;

		bool CHumans::Start()
		{

			return true;
		}

		void CHumans::OnDestroy()
		{
			DeleteGO(m_modelRenderer);
			return;
		}

		void CHumans::Update(float deltaTime)
		{

			return;
		}

		void CHumans::Init(
			const char* const keyname,
			const CVector3& pos,
			const CQuaternion& rot
		) noexcept
		{
			auto itr = m_kNameToFilePath.find(keyname);
			if (itr == m_kNameToFilePath.end())
			{
				return;
			}


			SModelInitData modelInitData;
			modelInitData.modelFilePath = itr->second;
			modelInitData.animInitData.Init(
				static_cast<int>(EnAnimType::enNum), m_kAnimFilePaths);
			modelInitData.vertexBias.SetRotationXDeg(90.0f);
			modelInitData.SetFlags(EnModelInitDataFlags::enLoadingAsynchronous);
			modelInitData.SetFlags(EnModelInitDataFlags::enRegisterAnimationBank);
			modelInitData.SetFlags(EnModelInitDataFlags::enRegisterTextureBank);
			modelInitData.SetFlags(EnModelInitDataFlags::enCullingOff);
			modelInitData.retargetSkeltonName = m_kRetargetSkeltonName;
			modelInitData.distanceToReducingUpdate = 50.0f;

			m_modelRenderer = NewGO<CModelRenderer>();
			m_modelRenderer->SetPosition(pos);
			m_modelRenderer->SetScale(m_kModelScale);
			m_modelRenderer->SetRotation(rot);
			m_modelRenderer->Init(modelInitData);


			return;
		}


		void CHumans::RegisterBase() noexcept
		{
			SModelInitData modelInitData;
			modelInitData.modelFilePath = CHumans::m_kNameToFilePath.at("Base");
			modelInitData.animInitData.Init(
				static_cast<int>(CHumans::EnAnimType::enNum), CHumans::m_kAnimFilePaths);
			modelInitData.SetFlags(EnModelInitDataFlags::enRegisterAnimationBank);
			modelInitData.retargetSkeltonName = CHumans::m_kRetargetSkeltonName;

			auto* modelRenderer = NewGO<CModelRenderer>();
			modelRenderer->SetScale(0.0f);
			modelRenderer->Init(modelInitData);
			DeleteGO(modelRenderer);
		}





		bool CDebugPlayer::Start()
		{
			SModelInitData modelInitData;
			modelInitData.modelFilePath = CHumans::m_kNameToFilePath.at("Player1");
			modelInitData.animInitData.Init(
				static_cast<int>(CHumans::EnAnimType::enNum), CHumans::m_kAnimFilePaths);
			modelInitData.vertexBias.SetRotationXDeg(90.0f);
			modelInitData.SetFlags(EnModelInitDataFlags::enLoadingAsynchronous);
			modelInitData.SetFlags(EnModelInitDataFlags::enRegisterAnimationBank);
			modelInitData.SetFlags(EnModelInitDataFlags::enRegisterTextureBank);
			modelInitData.SetFlags(EnModelInitDataFlags::enCullingOff);
			modelInitData.retargetSkeltonName = CHumans::m_kRetargetSkeltonName;
			modelInitData.distanceToReducingUpdate = 50.0f;


			m_modelRenderer = NewGO<CModelRenderer>();
			m_modelRenderer->Init(modelInitData);
			m_modelRenderer->SetScale(0.1f);



			// CreateSimpleMover
			m_simpleMover = NewGO<nsDebugSystem::CSimpleMover>();
			m_simpleMover->SetPosition({ 0.0f,0.0f,0.0f });

			return true;
		}

		void CDebugPlayer::OnDestroy()
		{
			DeleteGO(m_simpleMover);

			DeleteGO(m_modelRenderer);

			return;
		}

		void CDebugPlayer::Update(float deltaTime)
		{
			m_modelRenderer->SetPosition(m_simpleMover->GetPosition());
			auto front = nsMath::CVector3::Front();
			m_simpleMover->GetRotation().Apply(front);
			front.y = 0.0f;
			front.Normalize();
			nsMath::CQuaternion rot;
			rot.SetRotation(nsMath::CVector3::Back(), front);
			m_modelRenderer->SetRotation(rot);


			nsMath::CVector3 toCameraVec(0.0f, 30.0f, -40.0f);
			m_simpleMover->GetRotation().Apply(toCameraVec);
			nsMath::CVector3 cameraPos = m_simpleMover->GetPosition() + toCameraVec;

			nsMath::CVector3 toTargetVec(0.0f, 0.0f, 5.0f);
			m_simpleMover->GetRotation().Apply(toTargetVec);
			nsMath::CVector3 targetPos = m_simpleMover->GetPosition() + toTargetVec;


			MainCamera()->SetPosition(cameraPos);
			MainCamera()->SetTargetPosition(targetPos);


			return;
		}





		bool CDrawWorldSample::Start()
		{
			// CameraSetting
			MainCamera()->SetFarClip(1000.0f);

			// まず最初にベースとなるモデルをロードしておく。
			// アニメーションやAnimationScaledのため。
			CHumans::RegisterBase();

			// CreateWorldFromLevel3D
			SLevel3DInitData initData;
			initData.mBias.MakeScaling(0.01f, 0.01f, 0.01f);
			initData.isCreateStaticPhysicsObjectForAll = false;

			CQuaternion rot;
			rot.SetRotationXDeg(-90.0f);

			m_level3D.Init(
				"Assets/Level3D/Samples/TownLevelTest.fbx",
				initData,
				[&](const SLevelChipData& chipData)->bool
				{
					if (chipData.EqualObjectName("Town"))
					{
						static const char* const townFilePath = "Assets/Models/Town/Town.fbx";

						SModelInitData modelInitData;
						modelInitData.modelFilePath = townFilePath;
						modelInitData.SetFlags(EnModelInitDataFlags::enNodeTransform);

						auto* townMR = NewGO<CModelRenderer>();
						townMR->Init(modelInitData);
						townMR->SetScale(chipData.scale);
						townMR->SetPosition(chipData.position);
						townMR->SetRotation(chipData.rotation * rot);
						return true;
					}
					else if (chipData.ForwardMatchName("Humans_"))
					{
						auto* name = chipData.name;
						name += strlen("Humans_");
						std::string nameStr = name;
						const char* findChar = strchr(name, static_cast<int>('.'));
						if (findChar)
						{
							const int copySize = static_cast<int>(findChar - name);
							char* fileName = new char[copySize + 1];
							strncpy_s(fileName, copySize + 1, name, copySize);
							nameStr = fileName;
							delete[] fileName;
						}

						auto itr = CHumans::m_kNameToFilePath.find(nameStr);
						if (itr == CHumans::m_kNameToFilePath.end())
						{
							return true;
						}

						auto* human = NewGO<CHumans>();
						human->Init(nameStr.c_str(), chipData.position, chipData.rotation * rot);
						m_humansMap.emplace(name, human);

						return true;
					}
					return false;
				}
			);

			m_debugPlayer = NewGO<CDebugPlayer>();

			SFontParameter fontParam;
			fontParam.position = { 10.0f, 30.0f };
			fontParam.scale = 0.5f;
			fontParam.pivot = nsMath::CVector2::Zero();
			fontParam.anchor = EnAnchors::enTopLeft;
			m_fontRenderer = NewGO<CFontRenderer>();
			m_fontRenderer->Init(fontParam);

			return true;
		}

		void CDrawWorldSample::OnDestroy()
		{
			DeleteGO(m_fontRenderer);

			DeleteGO(m_debugPlayer);

			for (auto& human : m_humansMap)
			{
				DeleteGO(human.second);
			}
			m_humansMap.clear();

			return;
		}

		void CDrawWorldSample::Update(float deltaTime)
		{
			constexpr size_t kSize = 128;

			std::wstring text = {};
			text.reserve(kSize * (m_humansMap.size() + 1));

			{
				std::wstring wName(L"Player");

				const auto& pos = m_debugPlayer->GetPosition();
				wchar_t wText[kSize];
				StringCbPrintf(
					wText, kSize, L"%s : ( %2.2f, %2.2f, %2.2f)\n",
					wName.c_str(), pos.x, pos.y, pos.z);

				text += wText;
			}

			for (const auto& human : m_humansMap)
			{
				std::wstring wName = nsUtils::GetWideStringFromString(human.first);
				
				const auto& pos = human.second->GetPosition();
				wchar_t wText[kSize];
				StringCbPrintf(
					wText, kSize, L"%s : ( %2.2f, %2.2f, %2.2f)\n",
					wName.c_str(), pos.x, pos.y, pos.z);

				text += wText;
			}

			m_fontRenderer->SetText(text.c_str());

			return;
		}
	}
}