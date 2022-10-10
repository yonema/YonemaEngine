namespace nsYMEngine
{
	namespace nsUtil
	{
		std::string GetExtension(const std::string& filePath)
		{
			int idx = static_cast<int>(filePath.rfind('.'));

			// そのままだと'.'が入ってしまうから、一個進める。
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
			// 呼び出し1回目
			// 文字数を得るため。
			auto num1 = MultiByteToWideChar(
				CP_ACP,
				MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
				str,
				-1,
				nullptr,
				0
			);

			// 得られた文字数でリサイズ。
			std::wstring wstr;
			wstr.resize(num1);

			// 呼び出し2回目
			// 確保済みのwstrに変換文字列をコピー。
			auto num2 = MultiByteToWideChar(
				CP_ACP,
				MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
				str,
				-1,
				&wstr[0],
				num1
			);

			// 一応チェック。
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
			// spliterを含めないために一個進める。
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
			// spliterを含めないために一個進める。
			idx++;
			ret.second = str.substr(idx, str.length() - idx);

			return ret;
		}

		std::string GetTexturePathFromModelAndTexPath(
			const std::string& modelPath,
			const char* const texPath
		)
		{
			// rfind()が、戻り値site_tのくせにsize_tで受け取ると、
			// 文字が見つからなかったときに-1（負の数）で受け取れない。
			// だから、キャストしてintで受け取る。
			int pathIndex1 = static_cast<int>(modelPath.rfind('/'));
			int pathIndex2 = static_cast<int>(modelPath.rfind('\\'));

			// '/'か'\\'のどちらか有効なほうを採用する。
			auto pathIndex = max(pathIndex1, pathIndex2);

			// そのままだと最後の'/'が入らないから、一個進める。
			pathIndex++;
			auto folderPath = modelPath.substr(0, pathIndex);

			return folderPath + texPath;
		}

	}
}