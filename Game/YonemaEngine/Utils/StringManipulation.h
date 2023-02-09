#pragma once

namespace nsYMEngine
{
	namespace nsUtils
	{
		/**
		 * @brief ファイルパスから拡張子を取得する。
		 * @param[in] filePath 対象のファイルパス
		 * @return 拡張子。拡張子がない場合はファイルパスをそのまま返す。
		*/
		std::string GetExtension(const std::string& filePath) noexcept;

		/**
		 * @brief ファイルパスから拡張子を取得する。
		 * @param[in] filePath 対象のファイルパス
		 * @return 拡張子。拡張子がない場合はファイルパスをそのまま返す。
		*/
		const char* GetExtension(const char* filePath) noexcept;

		/**
		 * @brief ファイルパスからファイル名を取得する。
		 * @param filePath ファイルパス
		 * @return ファイル名
		*/
		std::string GetFileNameFromFilePath(const std::string& filePath) noexcept;

		/**
		 * @brief charからwstrinに変換する
		 * @param[in] str 変換するchar文字列
		 * @return 変換されたwstr文字列
		*/
		std::wstring GetWideStringFromString(const char* str) noexcept;

		/**
		 * @brief strからwstrinに変換する
		 * @param[in] str 変換するstr文字列
		 * @return 変換されたwstr文字列
		*/
		std::wstring GetWideStringFromString(const std::string& str) noexcept;


		/**
		 * @brief ファイル名をセパレータ文字で分離する
		 * @param[in] fileName 対象のファイル名（ファイルパスは不可）
		 * @param[in] spliter 区切り文字
		 * @return 分離前後の文字列ペア
		*/
		std::pair<std::string, std::string> SplitFilename(
			const std::string& fileName,
			const char spliter = '*'
		) noexcept;

		/**
		 * @brief ファイル名をセパレータ文字で分離する
		 * @param[in] fileName 対象のファイル名（ファイルパスは不可）
		 * @param[in] spliter 区切り文字
		 * @return 分離前後の文字列ペア
		*/
		std::pair<std::string, std::string> SplitFilename(
			const char* fileName,
			const char spliter = '*'
		) noexcept;

		/**
		 * @brief モデルのパスとテクスチャのパスから合成パスを得る
		 * @param[in] modelPath アプリケーションから見たpmdモデルのパス
		 * @param[in] texPath pmdモデルから見たテクスチャのパス
		 * @return アプリケーションから見たテクスチャのパス
		*/
		std::string GetTexturePathFromModelAndTexPath(
			const std::string& modelPath,
			const char* const texPath
		) noexcept;

		std::string ToUTF8(const std::wstring& wstr) noexcept;

		/**
		 * @brief 文字列中に一致する単語を置き換える
		 * @param sentence 置き換えを行う文章
		 * @param target 置き換え対象の単語
		 * @param replacement 置き換え後の単語
		 * @return 置き換えが行われた文章
		*/
		std::wstring ReplaceWordFromWideString(const std::wstring& sentence, const std::wstring& target, const std::wstring& replacement);


		/**
		 * @brief 文字列が前方一致するか調べる
		 * @param str 調べられる文字列
		 * @param forwardStr 調べる文字列
		 * @return 一致しているかどうか
		 * @retval true : 一致している
		 * @retval false : 一致してない
		*/
		bool ForwardMatchName(const char* str, const char* forwardStr);
	}
}
