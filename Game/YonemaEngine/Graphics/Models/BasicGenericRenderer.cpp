#include "BasicGenericRenderer.h"
#include "../GraphicsEngine.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsModels
		{
			const wchar_t* const CBasicGenericRenderer::m_kVsFilePath =
				L"Assets/Shaders/Models/NonSkinModelVertexShader.hlsl";
			const char* const CBasicGenericRenderer::m_kVsEntryFuncName = "VSMain";
			const wchar_t* const CBasicGenericRenderer::m_kPsFilePath =
				L"Assets/Shaders/Models/BasicModelPixelShader.hlsl";
			const char* const CBasicGenericRenderer::m_kPsEntryFuncName = "PSMain";



			void CBasicGenericRenderer::CreateRootParameter(
				std::vector<CD3DX12_DESCRIPTOR_RANGE1>* pDescTblRanges,
				std::vector<CD3DX12_ROOT_PARAMETER1>* pRootParameters
			) const noexcept
			{
				int numCBVs = 0;
				int numSRVs = 0;

				constexpr unsigned int kNumDescTblRanges =
					static_cast<unsigned int>(EnDescRangeType::enNumDescRangeTypes);
				constexpr unsigned int kDescRangeNoForModelCB =
					static_cast<unsigned int>(EnDescRangeType::enCbvForModelData);
				constexpr unsigned int kDescRangeNoForMaterialSR =
					static_cast<unsigned int>(EnDescRangeType::enSrvForMaterialData);

				pDescTblRanges->resize(kNumDescTblRanges);
				pDescTblRanges->at(kDescRangeNoForModelCB).Init(
					D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, numCBVs++);
				pDescTblRanges->at(kDescRangeNoForMaterialSR).Init(
					D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, numSRVs++);


				constexpr unsigned int kNumRootParameters =
					static_cast<unsigned int>(EnRootParameterType::enNumRootParamerterTypes);
				constexpr unsigned int kRootParamNoForModelData =
					static_cast<unsigned int>(EnRootParameterType::enModelData);
				constexpr unsigned int kRootParamNoForMaterialData =
					static_cast<unsigned int>(EnRootParameterType::enMaterialData);

				pRootParameters->resize(kNumRootParameters);

				pRootParameters->at(kRootParamNoForModelData).InitAsDescriptorTable(
					1,
					&pDescTblRanges->at(kDescRangeNoForModelCB),
					D3D12_SHADER_VISIBILITY_VERTEX
				);
				pRootParameters->at(kRootParamNoForMaterialData).InitAsDescriptorTable(
					1,
					&pDescTblRanges->at(kDescRangeNoForMaterialSR),
					D3D12_SHADER_VISIBILITY_PIXEL
				);

				return;
			}

			bool CBasicGenericRenderer::CreateShader(
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

			void CBasicGenericRenderer::CreateInputLayout(
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
						"COLOR",
						0,
						DXGI_FORMAT_R32G32B32A32_FLOAT,
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