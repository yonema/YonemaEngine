#pragma once
namespace nsYMEngine
{
	namespace nsLevel3D
	{
		class CLevelChip;
	}
}

namespace nsYMEngine
{
	namespace nsLevel3D
	{
		struct SLevelChipData : private nsUtils::SNoncopyable 
		{
			constexpr SLevelChipData() = default;
			~SLevelChipData() = default;

			nsMath::CVector3 position;		//���W�B
			nsMath::CQuaternion rotation;	//��]�B
			nsMath::CVector3 scale;			//�g�嗦�B
			const char* name = nullptr;	//���O�B
			int number = 0;
			bool isCreateStaticPhysicsObject = true;

			/// <summary>
			/// �����œn�����I�u�W�F�N�g���̃I�u�W�F�N�g�����ׂ�B
			/// </summary>
			/// <param name="objName">���ׂ閼�O�B</param>
			/// <returns>���O�������ꍇ��true��Ԃ��܂��B</returns>
			bool EqualObjectName(const char* objName) const noexcept
			{
				return strcmp(name, objName) == 0;
			}
			/// <summary>
			/// ���O���O����v���邩���ׂ�B
			/// </summary>
			/// <param name="n"></param>
			/// <returns></returns>
			bool ForwardMatchName(const char* n) const noexcept
			{
				auto len = strlen(n);
				auto namelen = strlen(name);
				if (len > namelen) {
					//���O�������B�s��v�B
					return false;
				}
				return strncmp(n, name, len) == 0;
			}
		};

		struct SLevel3DinitData : private nsUtils::SNoncopyable
		{
			constexpr SLevel3DinitData() = default;
			~SLevel3DinitData() = default;

			nsMath::CMatrix mBias = nsMath::CMatrix::Identity();
			float positionBias = 1.0f;
			const char* modelRootPath = nullptr;
			nsMath::CQuaternion levelChipBias = nsMath::CQuaternion::Identity();
			bool isCreateStaticPhysicsObjectForAll = true;
		};

		class CLevel3D : private nsUtils::SNoncopyable
		{
		private:
			using LevelChipPtr = std::shared_ptr<CLevelChip>;

		public:
			constexpr CLevel3D() = default;
			~CLevel3D() = default;

			bool Init(
				const char* filePath,
				std::function<bool(const SLevelChipData& chipData)> hookFunc
			);

			bool Init(
				const char* filePath,
				const SLevel3DinitData& initData,
				std::function<bool(const SLevelChipData& chipData)> hookFunc
			);

		private:
			void CreateMapChip(
				const SLevelChipData& chipData,
				const SLevel3DinitData& initData, 
				const char* filePath
			) noexcept;

		private:
			std::list<LevelChipPtr> m_levelChipList = {};
		};

	}
}