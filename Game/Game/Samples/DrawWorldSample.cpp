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
			modelInitData.SetFlags(EnModelInitDataFlags::enShadowCaster);
			modelInitData.retargetSkeltonName = m_kRetargetSkeltonName;
			modelInitData.distanceToReducingUpdate = 50.0f;
			//modelInitData.lodMedelFilePath = "Assets/Models/Humans/Player1_Low.fbx";
			//modelInitData.distanceToLOD = 50.0f;

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
			modelInitData.SetFlags(EnModelInitDataFlags::enShadowCaster);
			modelInitData.retargetSkeltonName = CHumans::m_kRetargetSkeltonName;
			modelInitData.distanceToReducingUpdate = 50.0f;


			m_modelRenderer = NewGO<CModelRenderer>();
			m_modelRenderer->Init(modelInitData);
			m_modelRenderer->SetScale(0.1f);



			// CreateSimpleMover
			m_simpleMover = NewGO<nsDebugSystem::CSimpleMover>();
			m_simpleMover->SetPosition(m_spawnPoint);

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
			constexpr float kFarClip = 5000.0f;

			// CameraSetting
			MainCamera()->SetFarClip(kFarClip);

			m_skyCubeRenderer = NewGO<CSkyCubeRenderer>();
			m_skyCubeRenderer->Init(EnSkyType::enNormal);
			m_skyCubeRenderer->SetScale(kFarClip);
			m_skyCubeRenderer->SetAutoFollowCameraFlag(true);
			m_skyCubeRenderer->SetAutoRotateFlag(true);

			// まず最初にベースとなるモデルをロードしておく。
			// アニメーションやAnimationScaledのため。
			CHumans::RegisterBase();

			// CreateWorldFromLevel3D
			SLevel3DInitData initData;
			initData.isCreateStaticPhysicsObjectForAll = false;

			CQuaternion rot;
			rot.SetRotationXDeg(-90.0f);

			struct STRS
			{
				STRS(const CVector3& pos, const CQuaternion& rot, const CVector3& scale)
					:pos(pos), rot(rot), scale(scale)
				{};
				CVector3 pos;
				CQuaternion rot;
				CVector3 scale;
			};
			std::unordered_map<std::string, std::list<STRS>> modelTRSArray = {};

			m_level3D.Init(
				"Assets/Level3D/WorldLevel.fbx",
				initData,
				[&](const SLevelChipData& chipData)->bool
				{
					if (chipData.ForwardMatchName("Humans_"))
					{
						auto* name = chipData.name.c_str();
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
					else if (chipData.ForwardMatchName("Building_") || chipData.ForwardMatchName("Town_"))
					{
						constexpr const char* const rootPath = "Assets/Models/Town/";

						std::string nameStr = chipData.name;

						nameStr += ".fbx";

						std::string filePath = rootPath + nameStr;

						modelTRSArray[filePath].emplace_back(
							chipData.position, chipData.rotation, chipData.scale);

						return true;
					}
					else if (chipData.ForwardMatchName("Meadow_"))
					{
						constexpr const char* const rootPath = "Assets/Models/Meadow/";

						std::string nameStr = chipData.name;

						nameStr += ".fbx";

						std::string filePath = rootPath + nameStr;
						modelTRSArray[filePath].emplace_back(
							chipData.position, chipData.rotation, chipData.scale);

						return true;
					}
					else if (chipData.ForwardMatchName("Mountain_"))
					{
						constexpr const char* const rootPath = "Assets/Models/Mountain/";

						std::string nameStr = chipData.name;

						nameStr += ".fbx";

						std::string filePath = rootPath + nameStr;
						modelTRSArray[filePath].emplace_back(
							chipData.position, chipData.rotation, chipData.scale);

						return true;
					}
					else if (chipData.ForwardMatchName("Monster_"))
					{
						const char* charName = chipData.name.c_str();
						charName += strlen("Monster_");

						if (nsUtils::ForwardMatchName(charName, "Meadow_"))
						{
							charName += strlen("Meadow_");
						}

						int num = atoi(charName);


						constexpr const char* const filePath =
							"Assets/Models/Monsters/Giyara.fbx";
						constexpr unsigned int numAnims = 1;
						constexpr const char* const animFilePaths[numAnims] =
						{
							"Assets/Animations/Monsters/Giyara/Giyara_Idle.fbx"
						};

						SModelInitData modelInitData;
						modelInitData.modelFilePath = filePath;
						modelInitData.SetFlags(EnModelInitDataFlags::enShadowCaster);
						modelInitData.SetFlags(EnModelInitDataFlags::enLoadingAsynchronous);
						modelInitData.SetFlags(EnModelInitDataFlags::enRegisterAnimationBank);
						modelInitData.SetFlags(EnModelInitDataFlags::enRegisterTextureBank);
						modelInitData.animInitData.Init(numAnims, animFilePaths);
						modelInitData.textureRootPath = "monster";


						auto* mr = NewGO<CModelRenderer>();
						mr->SetScale(0.1f);
						mr->SetPosition(chipData.position);
						mr->SetRotation(chipData.rotation * rot);
						mr->Init(modelInitData);

						return true;
					}
					else if (chipData.EqualObjectName("PlayerSpawn"))
					{
						m_debugPlayer = NewGO<CDebugPlayer>();
						m_debugPlayer->SetSpawnPoint(chipData.position);

						return true;
					}

					std::wstring wstr = L"予想外のオブジェクト ";
					wstr += nsUtils::GetWideStringFromString(chipData.name);
					wstr.erase(wstr.end() - 1);
					wstr += L" がレベルで読み込まれています。";
					nsGameWindow::MessageBoxWarning(wstr.c_str());

					return true;
				}
			);


			for (const auto& modelTRS : modelTRSArray)
			{
				const auto& filePath = modelTRS.first;

				SModelInitData modelInitData;
				modelInitData.modelFilePath = filePath;
				//modelInitData.SetFlags(EnModelInitDataFlags::enNodeTransform);
				modelInitData.SetFlags(EnModelInitDataFlags::enShadowCaster);
				modelInitData.SetFlags(EnModelInitDataFlags::enRegisterTextureBank);
				modelInitData.maxInstance = static_cast<unsigned int>(modelTRS.second.size());

				auto* mr = NewGO<CModelRenderer>();
				mr->Init(modelInitData);

				if (modelInitData.maxInstance > 1)
				{
					auto* worldMatArray = mr->GetWorldMatrixArrayRef();

					int idx = 0;
					for (const auto& trs : modelTRS.second)
					{
						CMatrix mModel, mTrans, mRot, mScale;
						mTrans.MakeTranslation(trs.pos);
						mScale.MakeScaling(trs.scale);
						mRot.MakeRotationFromQuaternion(trs.rot);

						mModel = mScale * mRot * mTrans;

						(*worldMatArray)[idx++] = mModel;
					}
				}
				else
				{
					const auto& trs = modelTRS.second.begin();
					mr->SetPosition(trs->pos);
					mr->SetScale(trs->scale);
					mr->SetRotation(trs->rot);
				}

			}


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

			DeleteGO(m_skyCubeRenderer);

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

			//nsGeometries::CGeometryData::DebugUpdate(deltaTime);

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