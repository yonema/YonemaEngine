namespace nsYMEngine
{
	namespace nsUtils
	{
		std::string GetExtension(const std::string& filePath)
		{
			int idx = static_cast<int>(filePath.rfind('.'));

			// ���̂܂܂���'.'�������Ă��܂�����A��i�߂�B
			idx++;
			return filePath.substr(idx, filePath.length() - idx);
		}

		const char* GetExtension(const char* filePath)
		{
			auto cp = std::strrchr(filePath, '.');
			if (cp != nullptr)
			{
				cp++;
			}
			else
			{
				cp = filePath;
			}
			return cp;
		}

		std::wstring GetWideStringFromString(const char* str)
		{
			// �Ăяo��1���
			// �������𓾂邽�߁B
			auto num1 = MultiByteToWideChar(
				CP_ACP,
				MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
				str,
				-1,
				nullptr,
				0
			);

			// ����ꂽ�������Ń��T�C�Y�B
			std::wstring wstr;
			wstr.resize(num1);

			// �Ăяo��2���
			// �m�ۍς݂�wstr�ɕϊ���������R�s�[�B
			auto num2 = MultiByteToWideChar(
				CP_ACP,
				MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
				str,
				-1,
				&wstr[0],
				num1
			);

			// �ꉞ�`�F�b�N�B
			assert(num1 == num2);

			return wstr;
		}

		std::wstring GetWideStringFromString(const std::string& str)
		{
			return GetWideStringFromString(str.c_str());
		}

		std::pair<std::string, std::string> SplitFilename(
			const std::string& fileName,
			const char spliter = '*'
		)
		{
			int idx = static_cast<int>(fileName.find(spliter));

			std::pair<std::string, std::string> ret;
			ret.first = fileName.substr(0, idx);
			// spliter���܂߂Ȃ����߂Ɉ�i�߂�B
			idx++;
			ret.second = fileName.substr(idx, fileName.length() - idx);

			return ret;
		}

		std::pair<std::string, std::string> SplitFilename(
			const char* fileName,
			const char spliter = '*'
		)
		{
			std::string str = fileName;
			int idx = static_cast<int>(str.find(spliter));

			std::pair<std::string, std::string> ret;
			ret.first = str.substr(0, idx);
			// spliter���܂߂Ȃ����߂Ɉ�i�߂�B
			idx++;
			ret.second = str.substr(idx, str.length() - idx);

			return ret;
		}

		std::string GetTexturePathFromModelAndTexPath(
			const std::string& modelPath,
			const char* const texPath
		)
		{
			// rfind()���A�߂�lsite_t�̂�����size_t�Ŏ󂯎��ƁA
			// ������������Ȃ������Ƃ���-1�i���̐��j�Ŏ󂯎��Ȃ��B
			// ������A�L���X�g����int�Ŏ󂯎��B
			int pathIndex1 = static_cast<int>(modelPath.rfind('/'));
			int pathIndex2 = static_cast<int>(modelPath.rfind('\\'));

			// '/'��'\\'�̂ǂ��炩�L���Ȃق����̗p����B
			auto pathIndex = max(pathIndex1, pathIndex2);

			// ���̂܂܂��ƍŌ��'/'������Ȃ�����A��i�߂�B
			pathIndex++;
			auto folderPath = modelPath.substr(0, pathIndex);

			return folderPath + texPath;
		}

	}
}