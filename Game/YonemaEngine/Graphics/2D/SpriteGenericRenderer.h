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
				 * @brief シェーダーをデフォルトと変えたいときは、このクラスを継承したクラスで
				 * この関数をオーバーライドし、変更点を書く。
				 * @param[out] pVsFilePath [出力用]頂点シェーダーファイルパス
				 * @param[out] pVsEntryFuncName [出力用]頂点シェーダーエントリー関数名
				 * @param[out] pPsFilePath [出力用]ピクセルシェーダーファイルパス
				 * @param[out] pPsEntryFuncName [出力用]ピクセルシェーダーエントリー関数名
				*/
				virtual void OverrideShader(
					const wchar_t** pVsFilePath,
					const char** pVsEntryFuncName,
					const wchar_t** pPsFilePath,
					const char** pPsEntryFuncName
				) {};

				/**
				 * @brief グラフィックスパイプラインステートの設定をデフォルトと変えたいときは、
				 * このクラスを継承したクラスでこの関数をオーバーライドし、変更点を書く。
				 * @param[out] pPipelineDesc [出力用]パイプライン設定
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
