#pragma once
namespace nsYMEngine
{
	namespace nsDebugSystem
	{
		class CSimpleMover;
	}
}

namespace nsAWA
{
	namespace nsSamples
	{
		class CGetBoneMatrixSample : public IGameObject
		{
		private:
			// HumanData
			enum class EnHumanAnimType
			{
				enDance,
				enNum
			};
			static const char* m_kHumanModelFilePath;
			static const char* m_kHumanAnimFilePaths[static_cast<int>(EnHumanAnimType::enNum)];
			static const char* const m_kHumanRightHandBoneName;

			// MonsterData
			enum class EnMonsterAnimType
			{
				enIdle,
				enNum
			};
			static const char* m_kMonsterModelFilePath;
			static const char* m_kMonsterAnimFilePaths[static_cast<int>(EnMonsterAnimType::enNum)];
			static const char* const m_kMonsterRightHandBoneName;


		public:
			bool Start() override final;

			void OnDestroy() override final;

			void Update(float deltaTime) override final;

		public:
			constexpr CGetBoneMatrixSample() = default;
			~CGetBoneMatrixSample() = default;

		private:
			// humanMember
			CModelRenderer* m_humanMR = nullptr;
			CPhysicsTriggerObject m_humanBoneTriggerBox;
			unsigned int m_humanBoneId = 0;

			// monsterMember
			CModelRenderer* m_monsterMR = nullptr;
			CPhysicsTriggerObject m_monsterBoneTriggerBox;
			unsigned int m_monsterBoneId = 0;

			nsYMEngine::nsDebugSystem::CSimpleMover* m_simpleMover = nullptr;
		};

	}
}
