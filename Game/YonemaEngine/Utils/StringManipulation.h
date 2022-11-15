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


	}
}
