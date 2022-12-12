#include "PhysicsDebugLineGenericRenderer.h"
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDebugRenderers
		{
			const wchar_t* const CPhysicsDebugLineGenericRenderer::m_kVsFilePath =
				L"Assets/Shaders/PhysicsDebugLine/PhysicsDebugLineVertexShader.hlsl";
			const char* const CPhysicsDebugLineGenericRenderer::m_kVsEntryFuncName = "VSMain";
			const wchar_t* const CPhysicsDebugLineGenericRenderer::m_kPsFilePath =
				L"Assets/Shaders/PhysicsDebugLine/PhysicsDebugLinePixelShader.hlsl";
			const char* const CPhysicsDebugLineGenericRenderer::m_kPsEntryFuncName = "PSMain";




			void CPhysicsDebugLineGenericRenderer::CreateRootParameter(
				std::vector<CD3DX12_DESCRIPTOR_RANGE1>* pDescTblRanges,
				std::vector<CD3DX12_ROOT_PARAMETER1>* pRootParameters
			) const noexcept
			{
				int numCBVs = 0;
				int numSRVs = 0;

				constexpr unsigned int kNumDescTblRanges =
					static_cast<unsigned int>(EnDescRangeType::enNumDescRangeTypes);
				constexpr unsigned int kDescRangeNoForSceneCB =
					static_cast<unsigned int>(EnDescRangeType::enCbvForSceneData);

				pDescTblRanges->resize(kNumDescTblRanges);
				pDescTblRanges->at(kDescRangeNoForSceneCB).Init(
					D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, numCBVs++);


				constexpr unsigned int kNumRootParameters =
					static_cast<unsigned int>(EnRootParameterType::enNumRootParamerterTypes);
				constexpr unsigned int kRootParamNoForSceneCB =
					static_cast<unsigned int>(EnRootParameterType::enSceneData);

				CD3DX12_ROOT_PARAMETER1 rootParameters[kNumRootParameters] = {};
				pRootParameters->resize(kNumRootParameters);
				pRootParameters->at(kRootParamNoForSceneCB).InitAsDescriptorTable(
					1,
					&pDescTblRanges->at(kDescRangeNoForSceneCB),
					D3D12_SHADER_VISIBILITY_VERTEX
				);

				return;
			}


			bool CPhysicsDebugLineGenericRenderer::CreateShader(
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

			void CPhysicsDebugLineGenericRenderer::CreateInputLayout(
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
						"COLOR",
						0,
						DXGI_FORMAT_R32G32B32_FLOAT,
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