#pragma once

namespace nsYMEngine
{
	namespace nsUtils
	{
		/**
		 * @brief �t�@�C���p�X����g���q���擾����B
		 * @param[in] filePath �Ώۂ̃t�@�C���p�X
		 * @return �g���q�B�g���q���Ȃ��ꍇ�̓t�@�C���p�X�����̂܂ܕԂ��B
		*/
		std::string GetExtension(const std::string& filePath) noexcept;

		/**
		 * @brief �t�@�C���p�X����g���q���擾����B
		 * @param[in] filePath �Ώۂ̃t�@�C���p�X
		 * @return �g���q�B�g���q���Ȃ��ꍇ�̓t�@�C���p�X�����̂܂ܕԂ��B
		*/
		const char* GetExtension(const char* filePath) noexcept;

		/**
		 * @brief �t�@�C���p�X����t�@�C�������擾����B
		 * @param filePath �t�@�C���p�X
		 * @return �t�@�C����
		*/
		std::string GetFileNameFromFilePath(const std::string& filePath) noexcept;

		/**
		 * @brief char����wstrin�ɕϊ�����
		 * @param[in] str �ϊ�����char������
		 * @return �ϊ����ꂽwstr������
		*/
		std::wstring GetWideStringFromString(const char* str) noexcept;

		/**
		 * @brief str����wstrin�ɕϊ�����
		 * @param[in] str �ϊ�����str������
		 * @return �ϊ����ꂽwstr������
		*/
		std::wstring GetWideStringFromString(const std::string& str) noexcept;


		/**
		 * @brief �t�@�C�������Z�p���[�^�����ŕ�������
		 * @param[in] fileName �Ώۂ̃t�@�C�����i�t�@�C���p�X�͕s�j
		 * @param[in] spliter ��؂蕶��
		 * @return �����O��̕�����y�A
		*/
		std::pair<std::string, std::string> SplitFilename(
			const std::string& fileName,
			const char spliter = '*'
		) noexcept;

		/**
		 * @brief �t�@�C�������Z�p���[�^�����ŕ�������
		 * @param[in] fileName �Ώۂ̃t�@�C�����i�t�@�C���p�X�͕s�j
		 * @param[in] spliter ��؂蕶��
		 * @return �����O��̕�����y�A
		*/
		std::pair<std::string, std::string> SplitFilename(
			const char* fileName,
			const char spliter = '*'
		) noexcept;

		/**
		 * @brief ���f���̃p�X�ƃe�N�X�`���̃p�X���獇���p�X�𓾂�
		 * @param[in] modelPath �A�v���P�[�V�������猩��pmd���f���̃p�X
		 * @param[in] texPath pmd���f�����猩���e�N�X�`���̃p�X
		 * @return �A�v���P�[�V�������猩���e�N�X�`���̃p�X
		*/
		std::string GetTexturePathFromModelAndTexPath(
			const std::string& modelPath,
			const char* const texPath
		) noexcept;

		std::string ToUTF8(const std::wstring& wstr) noexcept;


	}
}
