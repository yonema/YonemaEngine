#pragma once
#include "../Renderers/GenericRenderer.h"
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace ns2D
		{
			class CSpriteGenericRenderer : public nsRenderers::IGenericRenderer
			{
			private:
				enum class EnDescRangeType : unsigned int
				{
					enCbvForSpriteData,
					enSrvForSceneTexture,
					enNumDescRangeTypes
				};
				enum class EnRootParameterType : unsigned int
				{
					enSpriteData,
					enSceneTexture,
					enNumRootParamerterTypes
				};
				enum class EnSamplerType : unsigned int
				{
					enNormal,
					enNumSamplerTypes
				};

				static const wchar_t* const m_kVsFilePath;
				static const char* const m_kVsEntryFuncName;
				static const wchar_t* const m_kPsFilePath;
				static const char* const m_kPsEntryFuncName;

			public:
				constexpr CSpriteGenericRenderer() = default;
				~CSpriteGenericRenderer() = default;
				bool Init() override;

			protected:

				/**
				 * @brief �V�F�[�_�[���f�t�H���g�ƕς������Ƃ��́A���̃N���X���p�������N���X��
				 * ���̊֐����I�[�o�[���C�h���A�ύX�_�������B
				 * @param[out] pVsFilePath [�o�͗p]���_�V�F�[�_�[�t�@�C���p�X
				 * @param[out] pVsEntryFuncName [�o�͗p]���_�V�F�[�_�[�G���g���[�֐���
				 * @param[out] pPsFilePath [�o�͗p]�s�N�Z���V�F�[�_�[�t�@�C���p�X
				 * @param[out] pPsEntryFuncName [�o�͗p]�s�N�Z���V�F�[�_�[�G���g���[�֐���
				*/
				virtual void OverrideShader(
					const wchar_t** pVsFilePath,
					const char** pVsEntryFuncName,
					const wchar_t** pPsFilePath,
					const char** pPsEntryFuncName
				) {};

				/**
				 * @brief �O���t�B�b�N�X�p�C�v���C���X�e�[�g�̐ݒ���f�t�H���g�ƕς������Ƃ��́A
				 * ���̃N���X���p�������N���X�ł��̊֐����I�[�o�[���C�h���A�ύX�_�������B
				 * @param[out] pPipelineDesc [�o�͗p]�p�C�v���C���ݒ�
				*/
				virtual void OverrideGraphicsPipelineStateDesc(
					D3D12_GRAPHICS_PIPELINE_STATE_DESC* pPipelineDesc) {};

			private:

				bool CreateRootSignature(ID3D12Device5* device) override;
				bool CreatePipelineState(ID3D12Device5* device) override;

			private:

			};
		}
	}
}
