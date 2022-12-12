#include "SpriteGenericRenderer.h"
#include "../GraphicsEngine.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsSprites
		{
			const wchar_t* const CSpriteGenericRenderer::m_kVsFilePath =
				L"Assets/Shaders/SpriteVertexShader.hlsl";
			const char* const CSpriteGenericRenderer::m_kVsEntryFuncName = "VSMain";
			const wchar_t* const CSpriteGenericRenderer::m_kPsFilePath =
				L"Assets/Shaders/SpritePixelShader.hlsl";
			const char* const CSpriteGenericRenderer::m_kPsEntryFuncName = "PSMain";

			void CSpriteGenericRenderer::CreateRootParameter(
				std::vector<CD3DX12_DESCRIPTOR_RANGE1>* pDescTblRanges,
				std::vector<CD3DX12_ROOT_PARAMETER1>* pRootParameters
			) const noexcept
			{
				int numCBVs = 0;
				int numSRVs = 0;

				constexpr unsigned int kNumDescTblRanges =
					static_cast<unsigned int>(EnDescRangeType::enNumDescRangeTypes);
				constexpr unsigned int kDescRangeNoForSpriteCB =
					static_cast<unsigned int>(EnDescRangeType::enCbvForSpriteData);
				constexpr unsigned int kDescRangeNoForTextureSR =
					static_cast<unsigned int>(EnDescRangeType::enSrvForSceneTexture);

				pDescTblRanges->resize(kNumDescTblRanges);
				pDescTblRanges->at(kDescRangeNoForSpriteCB).Init(
					D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, numCBVs++);
				pDescTblRanges->at(kDescRangeNoForTextureSR).Init(
					D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, numSRVs++);

				constexpr unsigned int kNumRootParameters =
					static_cast<unsigned int>(EnRootParameterType::enNumRootParamerterTypes);
				constexpr unsigned int kRootParamNoForSpriteCB =
					static_cast<unsigned int>(EnRootParameterType::enSpriteData);
				constexpr unsigned int kRootParamNoForTextureSR =
					static_cast<unsigned int>(EnRootParameterType::enSceneTexture);

				CD3DX12_ROOT_PARAMETER1 rootParameters[kNumRootParameters] = {};
				pRootParameters->resize(kNumRootParameters);
				pRootParameters->at(kRootParamNoForSpriteCB).InitAsDescriptorTable(
					1,
					&pDescTblRanges->at(kDescRangeNoForSpriteCB),
					D3D12_SHADER_VISIBILITY_ALL
				);
				pRootParameters->at(kRootParamNoForTextureSR).InitAsDescriptorTable(
					1,
					&pDescTblRanges->at(kDescRangeNoForTextureSR),
					D3D12_SHADER_VISIBILITY_PIXEL
				);

				return;
			}


			bool CSpriteGenericRenderer::CreateShader(
				nsDx12Wrappers::CBlob* pVsBlob,
				nsDx12Wrappers::CBlob* pPsBlob
			) const noexcept
			{
				return LoadShader(
					m_kVsFilePath,
					m_kVsEntryFuncName,
					pVsBlob,
					m_kPsFilePath,
					m_kPsEntryFuncName,
					pPsBlob
				);
			}

			void CSpriteGenericRenderer::CreateInputLayout(
				std::vector<D3D12_INPUT_ELEMENT_DESC>* pInputLayout) const noexcept
			{
				constexpr D3D12_INPUT_ELEMENT_DESC inputLayout[] =
				{
					{
						"POSITION",
						0,
						DXGI_FORMAT_R32G32B32_FLOAT,
						0,
						D3D12_APPEND_ALIGNED_ELEMENT,
						D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
						0
					},
					{
						"TEXCOORD",
						0,
						DXGI_FORMAT_R32G32_FLOAT,
						0,
						D3D12_APPEND_ALIGNED_ELEMENT,
						D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
						0
					}
				};

				const auto kNumInputLayout = static_cast<unsigned int>(_countof(inputLayout));
				pInputLayout->reserve(kNumInputLayout);
				for (unsigned int i = 0; i < kNumInputLayout; i++)
				{
					pInputLayout->emplace_back(inputLayout[i]);
				}
				return;
			}


		}
	}
}