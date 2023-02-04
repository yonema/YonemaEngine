#include "YonemaEnginePreCompile.h"
#include "Animation2DImporter.h"
#include "Animation2DData.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsAnimations2D
		{
			SAnimation2DData* CAnimation2DImporter::Load(const char* path)
			{
				FILE* fp = nullptr;
				fopen_s(&fp, path, "rb");

				if (fp == nullptr)
				{
					//ファイルが見つからない。
					nsGameWindow::MessageBoxWarning(L"ファイルが見つかりません。");
					return nullptr;
				}

				SAnimation2DData* animationData = new SAnimation2DData();;
				
				//アニメーション名
				int nameLen = 0;
				fread(&nameLen, sizeof(int), 1, fp);

				std::unique_ptr<char[]> animName = std::make_unique<char[]>(nameLen + 1);
				fread(animName.get(), nameLen + 1, 1, fp);

				animationData->m_animtionName = animName.get();

				//ループフラグ
				fread(&animationData->m_loopFlag, sizeof(bool), 1, fp);


				//移動モード
				fread(&animationData->m_isMoveAbsolute, sizeof(bool), 1, fp);


				//アニメーションのフレーム数
				fread(&animationData->m_totalFrame, sizeof(int), 1, fp);
				animationData->m_frameDatas.reserve(animationData->m_totalFrame);


				//キーフレームに関してはゲーム側では使わないため読み込んでも何もしない
				
				//アニメーションのキーフレーム数
				int animKeyFrameNum = 0;
				fread(&animKeyFrameNum, sizeof(int), 1, fp);

				//キーフレームが何フレーム目か
				for (int i = 0;i < animKeyFrameNum;i++)
				{
					int animKeyFrame = 0;
					fread(&animKeyFrame, sizeof(int), 1, fp);
				}


				//アニメーションイベントの数
				int animEventNum = 0;
				fread(&animEventNum, sizeof(int), 1, fp);


				//アニメーションのフレームデータ
				for (int i = 0;i < animationData->m_totalFrame;i++)
				{
					SAnimation2DFrameData frameData;
					fread(&frameData.Position.x, sizeof(float), 1, fp);
					fread(&frameData.Position.y, sizeof(float), 1, fp);

					fread(&frameData.RotateDeg, sizeof(float), 1, fp);

					fread(&frameData.Scale.x, sizeof(float), 1, fp);
					fread(&frameData.Scale.y, sizeof(float), 1, fp);

					int MulColor255A = 255, MulColor255R = 255, MulColor255G = 255, MulColor255B = 255;
					fread(&MulColor255A, sizeof(int), 1, fp);
					fread(&MulColor255R, sizeof(int), 1, fp);
					fread(&MulColor255G, sizeof(int), 1, fp);
					fread(&MulColor255B, sizeof(int), 1, fp);

					float NormalizedA = static_cast<float>(MulColor255A) / 255.0f;
					float NormalizedR = static_cast<float>(MulColor255R) / 255.0f;
					float NormalizedG = static_cast<float>(MulColor255G) / 255.0f;
					float NormalizedB = static_cast<float>(MulColor255B) / 255.0f;

					frameData.MulColor = CVector4(NormalizedR, NormalizedG, NormalizedB, NormalizedA);

					fread(&frameData.Pivot.x, sizeof(float), 1, fp);
					fread(&frameData.Pivot.y, sizeof(float), 1, fp);

					ConvertToEngineCoordinate(frameData);

					animationData->m_frameDatas.emplace_back(frameData);
				}

				//アニメーションのイベントデータ
				for (int i = 0;i < animEventNum;i++)
				{
					int eventFrame = 0;
					fread(&eventFrame, sizeof(int), 1, fp);

					int eventNameLen = 0;
					fread(&eventNameLen, sizeof(int), 1, fp);

					std::unique_ptr<char[]> eventName = std::make_unique<char[]>(eventNameLen + 1);
					fread(eventName.get(), eventNameLen + 1, 1, fp);

					const std::string strEventName = eventName.get();

					animationData->m_eventNameMap.insert({ strEventName, eventFrame });
				}


				//ファイルクローズ
				fclose(fp);

				return animationData;
			}

			void CAnimation2DImporter::ConvertToEngineCoordinate(SAnimation2DFrameData& frameData)
			{
				//yピボット、y座標が逆になる。
				frameData.Pivot.y = 1.0f - frameData.Pivot.y;
				frameData.Position.y *= -1.0f;
			}
		}
	}
}