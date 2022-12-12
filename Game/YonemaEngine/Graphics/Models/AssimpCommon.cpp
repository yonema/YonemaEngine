#include "AssimpCommon.h"
#include "../../Utils/StringManipulation.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsAssimpCommon
		{
			bool ImportScene(
				const char* modelFilePath,
				Assimp::Importer*& pImporter,
				const aiScene*& pScene,
				unsigned int removeFlags,
				unsigned int postprocessFlag
			)
			{
				// utf8�̃t�@�C���p�X�����񂪕K�v�Ȃ��ߕϊ��B

				auto filePathInChar = modelFilePath;
				auto filePathInWStr = nsUtils::GetWideStringFromString(filePathInChar);
				auto filePathInUTF8Str = nsUtils::ToUTF8(filePathInWStr);

				pImporter = new Assimp::Importer;

				pImporter->SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, removeFlags);

				pImporter->SetPropertyInteger(
					AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE | aiPrimitiveType_POINT);


				pScene = pImporter->ReadFile(filePathInUTF8Str, postprocessFlag);

				if (pScene == nullptr)
				{
					std::wstring wstr = filePathInWStr;
					wstr += L"�̃��f���̓ǂݍ��݂Ɏ��s���܂����B";
					nsGameWindow::MessageBoxWarning(wstr.c_str());
					::OutputDebugStringA(pImporter->GetErrorString());
					::OutputDebugStringA("\n");
					return false;
				}

				return true;
			}
		}
	}
}