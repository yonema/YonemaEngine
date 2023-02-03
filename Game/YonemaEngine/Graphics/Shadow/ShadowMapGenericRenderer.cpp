#include "ShadowMapGenericRenderer.h"
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsShadow
		{
			const wchar_t* const CShadowMapGenericRenderer::m_kVsFilePath =
				L"Assets/Shaders/Shadow/ShadowMapVertexShader.hlsl";
			const char* const CShadowMapGenericRenderer::m_kVsEntryFuncName = "VSMain";
			const wchar_t* const CShadowMapGenericRenderer::m_kPsFilePath =
				L"Assets/Shaders/Shadow/ShadowMapPixelShader.hlsl";
			const char* const CShadowMapGenericRenderer::m_kPsEntryFuncName = "PSMain";



			void CShadowMapGenericRenderer::CreateRootParameter(
				std::vector<CD3DX12_DESCRIPTOR_RANGE1>* pDescTblRanges,
				std::vector<CD3DX12_ROOT_PARAMETER1>* pRootParameters
			) const noexcept
			{
				int numCBVs = 0;
				int numSRVs = 0;

				constexpr unsigned int kNumDescTblRanges =
					static_cast<unsigned int>(EnDescRangeType::enNumDescRangeTypes);
				constexpr unsigned int kRangeNoForModelCB =
					static_cast<unsigned int>(EnDescRangeType::enCbvForModel);
				constexpr unsigned int kRangeNoForBoneMatrixArraySRV =
					static_cast<unsigned int>(EnDescRangeType::enSrvForBoneMatrixArray);
				constexpr unsigned int kRangeNoForWorldMatrixArraySRV =
					static_cast<unsigned int>(EnDescRangeType::enSrvForWorldMatrixArray);

				pDescTblRanges->resize(kNumDescTblRanges);
				pDescTblRanges->at(kRangeNoForModelCB).Init(
					D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, numCBVs++);
				pDescTblRanges->at(kRangeNoForBoneMatrixArraySRV).Init(
					D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, numSRVs++);
				pDescTblRanges->at(kRangeNoForWorldMatrixArraySRV).Init(
					D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, numSRVs++);


				constexpr unsigned int kNumRootParameters =
					static_cast<unsigned int>(EnRootParameterType::enNumRootParamerterTypes);
				constexpr unsigned int kParamNoForModel =
					static_cast<unsigned int>(EnRootParameterType::enModel);
				constexpr unsigned int kParamNoForBoneMatrixArray =
					static_cast<unsigned int>(EnRootParameterType::enBoneMatrixArray);
				constexpr unsigned int kParamNoForWorldMatrixArray =
					static_cast<unsigned int>(EnRootParameterType::enWorldMatrixArray);

				pRootParameters->resize(kNumRootParameters);

				pRootParameters->at(kParamNoForModel).InitAsDescriptorTable(
					1,
					&pDescTblRanges->at(kRangeNoForModelCB),
					D3D12_SHADER_VISIBILITY_ALL
				);
				pRootParameters->at(kParamNoForBoneMatrixArray).InitAsDescriptorTable(
					1,
					&pDescTblRanges->at(kRangeNoForBoneMatrixArraySRV),
					D3D12_SHADER_VISIBILITY_VERTEX
				);
				pRootParameters->at(kParamNoForWorldMatrixArray).InitAsDescriptorTable(
					1,
					&pDescTblRanges->at(kRangeNoForWorldMatrixArraySRV),
					D3D12_SHADER_VISIBILITY_VERTEX
				);



				return;
			}

			bool CShadowMapGenericRenderer::CreateShader(
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

			void CShadowMapGenericRenderer::CreateInputLayout(
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
						"NORMAL",
						0,
						DXGI_FORMAT_R32G32B32_FLOAT,
						0,
						D3D12_APPEND_ALIGNED_ELEMENT,
						D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
						0
					},
					{
						"TANGENT",
						0,
						DXGI_FORMAT_R32G32B32_FLOAT,
						0,
						D3D12_APPEND_ALIGNED_ELEMENT,
						D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
						0
					},
					{
						"BINORMAL",
						0,
						DXGI_FORMAT_R32G32B32_FLOAT,
						0,
						D3D12_APPEND_ALIGNED_ELEMENT,
						D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
						0
					},
					{
						"TEXCOORD",	// UV
						0,
						DXGI_FORMAT_R32G32_FLOAT,
						0,
						D3D12_APPEND_ALIGNED_ELEMENT,
						D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
						0
					},
					{
						"BONE_NO",
						0,
						DXGI_FORMAT_R16G16B16A16_UINT,
						0,
						D3D12_APPEND_ALIGNED_ELEMENT,
						D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
						0
					},
					{
						"WEIGHT",
						0,
						DXGI_FORMAT_R16G16B16A16_UINT,
						0,
						D3D12_APPEND_ALIGNED_ELEMENT,
						D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
						0
					},
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