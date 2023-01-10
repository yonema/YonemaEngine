#include "Level3D.h"
#include "../Graphics/Models/AssimpCommon.h"
#include "LevelChip.h"

namespace nsYMEngine
{
	namespace nsLevel3D
	{
		bool CLevel3D::Init(
			const char* filePath,
			std::function<bool(const SLevelChipData& chipData)> hookFunc
		)
		{
			SLevel3DinitData initData = {};
			return Init(filePath, initData, hookFunc);
		}

		bool CLevel3D::Init(
			const char* filePath,
			const SLevel3DinitData& initData,
			std::function<bool(const SLevelChipData& chipData)> hookFunc
		)
		{
			Assimp::Importer* importer = nullptr;
			const aiScene* scene = nullptr;

			if (nsGraphics::nsAssimpCommon::ImportScene(
				filePath,
				importer,
				scene,
				nsGraphics::nsAssimpCommon::g_kNavMeshRemoveComponentFlags,
				nsGraphics::nsAssimpCommon::g_kNavMeshPostprocessFlags
			) != true)
			{
				return false;
			}

			const unsigned int kNumLevelChip = scene->mRootNode->mNumChildren;

			for (unsigned int chipIdx = 0; chipIdx < kNumLevelChip; chipIdx++)
			{
				const auto& levelChip = scene->mRootNode->mChildren[chipIdx];

				SLevelChipData levelChipData = {};

				nsMath::CMatrix mTransformation;
				nsMath::CVector3 translation, scaling;
				nsMath::CQuaternion rotation;
				nsGraphics::nsAssimpCommon::AiMatrixToMyMatrix(
					levelChip->mTransformation, &mTransformation);
				nsMath::CMatrix mTrans, mRot, mScale;
				mTrans.MakeTranslation(translation);
				mRot.MakeRotationFromQuaternion(rotation);
				mScale.MakeScaling(scaling);
				mTransformation = initData.mBias * mTransformation;

				nsMath::CalcTRSFromMatrix(translation, rotation, scaling, mTransformation);
				levelChipData.position = translation * initData.positionBias;
				levelChipData.rotation = rotation;
				levelChipData.scale = scaling;
				levelChipData.number = chipIdx;
				levelChipData.name = levelChip->mName.C_Str();


				//Hook���o�^�ς݂Ȃ�΃}�b�v�`�b�v�͍쐬�s�v
				//false �̂܂܂Ȃ�쐬����B
				bool isHooked = false;

				//hookFunc�ɂȂɂ������Ă��Ȃ���Sobject�ɂ͉����Ȃ��B
				if (hookFunc != nullptr) 
				{
					isHooked = hookFunc(levelChipData);
				}

				//hook��false�Ȃ܂܂Ȃ�΁B
				if (isHooked == false) 
				{
					std::string modelFilePath = "Assets/Models/";
					if (initData.modelRootPath)
					{
						modelFilePath += initData.modelRootPath;
						modelFilePath += "/";
					}
					const char* findChar = strchr(levelChipData.name, static_cast<int>('.'));
					if (findChar)
					{
						const int copySize = static_cast<int>(findChar - levelChipData.name);
						char* fileName = new char[copySize + 1];
						strncpy_s(fileName, copySize + 1, levelChipData.name, copySize);
						modelFilePath += fileName;
						delete[] fileName;
					}
					else
					{
						modelFilePath += levelChipData.name;
					}
					modelFilePath += ".fbx";
					//�}�b�v�`�b�v�����_�[���쐬�B
					CreateMapChip(levelChipData, initData, modelFilePath.c_str());
				}
			}



			return true;
		}


		void CLevel3D::CreateMapChip(
			const SLevelChipData& chipData,
			const SLevel3DinitData& initData,
			const char* filePath
		) noexcept
		{
			//�t�b�N����Ȃ������̂ŁA�}�b�v�`�b�v���쐬����B
			auto levelChip = std::make_shared<CLevelChip>(chipData, initData, filePath);
			m_levelChipList.emplace_back(levelChip);

			return;
		}



	}
}