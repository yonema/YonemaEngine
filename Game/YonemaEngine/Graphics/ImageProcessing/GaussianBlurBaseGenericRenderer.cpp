#include "GaussianBlurBaseGenericRenderer.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsImageProcessing
		{

			void CGaussianBlurBaseGenericRenderer::CreateRootParameter(
				std::vector<CD3DX12_DESCRIPTOR_RANGE1>* pDescTblRanges,
				std::vector<CD3DX12_ROOT_PARAMETER1>* pRootParameters
			) const noexcept
			{
				int numCBVs = 0;
				int numSRVs = 0;

				constexpr unsigned int kNumDescTblRanges =
					static_cast<unsigned int>(EnDescRangeType::enNumDescRangeTypes);
				constexpr unsigned int kRangeNoForSpriteCB =
					static_cast<unsigned int>(EnDescRangeType::enCbvForSpriteData);
				constexpr unsigned int kRangeNoForTextureSR =
					static_cast<unsigned int>(EnDescRangeType::enSrvForSceneTexture);
				constexpr unsigned int kRangeNoForWeightsCB =
					static_cast<unsigned int>(EnDescRangeType::enCbvForWeights);

				pDescTblRanges->resize(kNumDescTblRanges);
				pDescTblRanges->at(kRangeNoForSpriteCB).Init(
					D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, numCBVs++);
				pDescTblRanges->at(kRangeNoForTextureSR).Init(
					D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, numSRVs++);
				pDescTblRanges->at(kRangeNoForWeightsCB).Init(
					D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, numCBVs++);

				constexpr unsigned int kNumRootParameters =
					static_cast<unsigned int>(EnRootParameterType::enNumRootParamerterTypes);
				constexpr unsigned int kParamNoForSpriteCB =
					static_cast<unsigned int>(EnRootParameterType::enSpriteData);
				constexpr unsigned int kParamNoForTextureSR =
					static_cast<unsigned int>(EnRootParameterType::enSceneTexture);
				constexpr unsigned int kParamNoForWeightsCB =
					static_cast<unsigned int>(EnRootParameterType::enWeights);

				CD3DX12_ROOT_PARAMETER1 rootParameters[kNumRootParameters] = {};
				pRootParameters->resize(kNumRootParameters);
				pRootParameters->at(kParamNoForSpriteCB).InitAsDescriptorTable(
					1,
					&pDescTblRanges->at(kRangeNoForSpriteCB),
					D3D12_SHADER_VISIBILITY_ALL
				);
				pRootParameters->at(kParamNoForTextureSR).InitAsDescriptorTable(
					1,
					&pDescTblRanges->at(kRangeNoForTextureSR),
					D3D12_SHADER_VISIBILITY_ALL
				);
				pRootParameters->at(kParamNoForWeightsCB).InitAsDescriptorTable(
					1,
					&pDescTblRanges->at(kRangeNoForWeightsCB),
					D3D12_SHADER_VISIBILITY_PIXEL
				);

				return;
			}
		}
	}
}