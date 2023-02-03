#pragma once
#include "../Sprites/SpriteGenericRenderer.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsImageProcessing
		{
			class CGaussianBlurBaseGenericRenderer : public nsSprites::CSpriteGenericRenderer
			{
			private:
				enum class EnDescRangeType : unsigned int
				{
					enCbvForSpriteData,
					enSrvForSceneTexture,
					enCbvForWeights,
					enNumDescRangeTypes
				};
				enum class EnRootParameterType : unsigned int
				{
					enSpriteData,
					enSceneTexture,
					enWeights,
					enNumRootParamerterTypes
				};


			private:
				void CreateRootParameter(
					std::vector<CD3DX12_DESCRIPTOR_RANGE1>* pDescTblRanges,
					std::vector<CD3DX12_ROOT_PARAMETER1>* pRootParameters
				) const noexcept override;

				inline std::wstring CreateRootSignatureName() const noexcept override
				{
					return L"GaussianBlurBaseGenericRenderer";
				}

				inline std::wstring CreatePipelineStateName() const noexcept override
				{
					return L"GaussianBlurBaseGenericRenderer";
				}

			public:
				constexpr CGaussianBlurBaseGenericRenderer() = default;
				virtual ~CGaussianBlurBaseGenericRenderer() = default;


			private:

			};

		}
	}
}