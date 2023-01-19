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

			int spriteNum;
			fread(&spriteNum, sizeof(int), 1, fp);

			for (int i = 0;i < spriteNum;i++)
			{
				SLevel2DSpriteData spriteData;

				int pathLen;
				fread(&pathLen, sizeof(int), 1, fp);

				std::unique_ptr<char[]> path = std::make_unique<char[]>(pathLen + 1);
				fread(path.get(), pathLen + 1, 1, fp);

				spriteData.Path = "Assets/2DLevel/Sprites/";
				spriteData.Path += path.get();

				int nameLen;
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

				fread(&spriteData.MulColor.a, sizeof(int), 1, fp);

				fread(&spriteData.MulColor.r, sizeof(int), 1, fp);

				fread(&spriteData.MulColor.g, sizeof(int), 1, fp);

				fread(&spriteData.MulColor.b, sizeof(int), 1, fp);

				fread(&spriteData.Pivot.x, sizeof(float), 1, fp);

				fread(&spriteData.Pivot.y, sizeof(float), 1, fp);

				fread(&spriteData.Priority, sizeof(int), 1, fp);

				ConvertToEngineCoordinate(spriteData);

				int animNum;
				fread(&animNum, sizeof(int), 1, fp);

				for (int j = 0;j < animNum;j++)
				{
					//アニメーション読み込み
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

		CSpriteRenderer* CLevel2D::CreateNewSpriteFromData(const SLevel2DSpriteData& spriteData)
		{
			CSpriteRenderer* newSprite = NewGO<CSpriteRenderer>(static_cast<EnGOPriority>(spriteData.Priority), spriteData.Name.c_str());
			
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

			//newSprite->SetMultipleColor(spriteData.MulColor / 255);

			newSprite->SetPivot(spriteData.Pivot);

			return newSprite;
		}
	}
}