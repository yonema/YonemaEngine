#include "YonemaEnginePreCompile.h"
#include "Level2D.h"

namespace nsYMEngine
{
	namespace nsLevel2D
	{
		void CLevel2D::Load(const char* path, std::function<bool(const SLevel2DSpriteData&)> hookFunc)
		{
			FILE* fp = nullptr;
			fopen_s(&fp, path, "rb");

			if (fp == nullptr)
			{
				//ファイルが見つからない。
				nsGameWindow::MessageBoxWarning(L"ファイルが見つかりません。");
				return;
			}

			char engineVer;
			fread(&engineVer, 1, 1, fp);

			if (engineVer != 'y')
			{
				//ファイルがこのエンジンに対応していない。
				nsGameWindow::MessageBoxWarning(L"ファイルのバージョンが異なります。");
				return;
			}

			int spriteNum = 0;
			fread(&spriteNum, sizeof(int), 1, fp);

			for (int i = 0;i < spriteNum;i++)
			{
				SLevel2DSpriteData spriteData;

				int pathLen = 0;
				fread(&pathLen, sizeof(int), 1, fp);

				std::unique_ptr<char[]> path = std::make_unique<char[]>(pathLen + 1);
				fread(path.get(), pathLen + 1, 1, fp);

				spriteData.Path = "Assets/Level2D/Sprites/";
				spriteData.Path += path.get();

				int nameLen = 0;
				fread(&nameLen, sizeof(int), 1, fp);

				std::unique_ptr<char[]> name = std::make_unique<char[]>(nameLen + 1);
				fread(name.get(), nameLen + 1, 1, fp);

				spriteData.Name = name.get();

				fread(&spriteData.SpriteSize.x, sizeof(float), 1, fp);

				fread(&spriteData.SpriteSize.y, sizeof(float), 1, fp);

				fread(&spriteData.Position.x, sizeof(float), 1, fp);

				fread(&spriteData.Position.y, sizeof(float), 1, fp);

				fread(&spriteData.RotateDeg, sizeof(float), 1, fp);

				fread(&spriteData.Scale.x, sizeof(float), 1, fp);

				fread(&spriteData.Scale.y, sizeof(float), 1, fp);

				int MulColor255A = 255, MulColor255R = 255, MulColor255G = 255, MulColor255B = 255;
				fread(&MulColor255A, sizeof(int), 1, fp);
				fread(&MulColor255R, sizeof(int), 1, fp);
				fread(&MulColor255G, sizeof(int), 1, fp);
				fread(&MulColor255B, sizeof(int), 1, fp);

				float NormalizedA = static_cast<float>(MulColor255A) / 255.0f;
				float NormalizedR = static_cast<float>(MulColor255R) / 255.0f;
				float NormalizedG = static_cast<float>(MulColor255G) / 255.0f;
				float NormalizedB = static_cast<float>(MulColor255B) / 255.0f;

				spriteData.MulColor = CVector4(NormalizedR, NormalizedG, NormalizedB, NormalizedA);

				fread(&spriteData.Pivot.x, sizeof(float), 1, fp);

				fread(&spriteData.Pivot.y, sizeof(float), 1, fp);

				fread(&spriteData.Priority, sizeof(int), 1, fp);

				ConvertToEngineCoordinate(spriteData);

				int animNum = 0;
				fread(&animNum, sizeof(int), 1, fp);

				spriteData.animPathVector.reserve(animNum);

				for (int j = 0;j < animNum;j++)
				{
					int animNameLength = 0;
					fread(&animNameLength, sizeof(int), 1, fp);

					std::unique_ptr<char[]> animName = std::make_unique<char[]>(animNameLength + 1);
					fread(animName.get(), animNameLength + 1, 1, fp);

					std::string animPath = "Assets/Animations2D/";
					animPath += animName.get();
					animPath += ".tda";

					spriteData.animPathVector.emplace_back(animPath);
				}

				//Hook実行
				if (hookFunc != nullptr && hookFunc(spriteData) == true)
				{
					continue;
				}

				//スプライトのリストに追加
				m_spritesList.emplace_back(CreateNewSpriteFromData(spriteData));
			}

			fclose(fp);
		}

		void CLevel2D::ConvertToEngineCoordinate(SLevel2DSpriteData& spriteData)
		{
			//yピボット、y座標が逆になる。
			spriteData.Pivot.y = 1.0f - spriteData.Pivot.y;
			spriteData.Position.y *= -1.0f;
		}

		CAnimatedSpriteRenderer* CLevel2D::CreateNewSpriteFromData(const SLevel2DSpriteData& spriteData)
		{
			CAnimatedSpriteRenderer* newSprite = NewGO<CAnimatedSpriteRenderer>(static_cast<EnGOPriority>(spriteData.Priority), spriteData.Name.c_str());
			
			SSpriteInitData spriteInitData;
			spriteInitData.filePath = spriteData.Path.c_str();
			spriteInitData.spriteSize = spriteData.SpriteSize;
			spriteInitData.alphaBlendMode = nsGraphics::nsSprites::EnAlphaBlendMode::enTrans;

			newSprite->Init(spriteInitData);

			newSprite->SetPosition(spriteData.Position);

			CQuaternion qRot;
			qRot.SetRotationZDeg(spriteData.RotateDeg);
			newSprite->SetRotation(qRot);

			newSprite->SetScale(CVector3(spriteData.Scale.x,spriteData.Scale.y,1.0f));

			newSprite->SetMulColor(spriteData.MulColor);

			newSprite->SetPivot(spriteData.Pivot);

			//アニメーションの登録
			for (std::string animPath : spriteData.animPathVector)
			{
				newSprite->InitAnimation(animPath.c_str());
			}

			return newSprite;
		}
	}
}