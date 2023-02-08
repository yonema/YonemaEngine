#include "SkyCubeGenericRenderer.h"
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsEnvironment
		{
			const wchar_t* const CSkyCubeGenericRenderer::m_kVsFilePath =
				L"Assets/Shaders/Models/SkyCubeVertexShader.hlsl";
			const char* const CSkyCubeGenericRenderer::m_kVsEntryFuncName = "VSMain";
			const wchar_t* const CSkyCubeGenericRenderer::m_kPsFilePath =
				L"Assets/Shaders/Models/SkyCubePixelShader.hlsl";
			const char* const CSkyCubeGenericRenderer::m_kPsEntryFuncName = "PSMain";

			void CSkyCubeGenericRenderer::CreateRootParameter(
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
				constexpr unsigned int kRangeNoForMaterialSRV =
					static_cast<unsigned int>(EnDescRangeType::enSrvForMaterial);

				pDescTblRanges->resize(kNumDescTblRanges);
				// ���ʂ�ModelCB�Ɗg���萔�o�b�t�@��2��
				pDescTblRanges->at(kRangeNoForModelCB).Init(
					D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 2, numCBVs++);
				// diffuse�ɐݒ肵�Ă���L���[�u�}�b�v�̂ݎg�p
				// diffuse���ݒ肵�Ă���3�ԃ��W�X�^5
				pDescTblRanges->at(kRangeNoForMaterialSRV).Init(
					D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3);


				constexpr unsigned int kNumRootParameters =
					static_cast<unsigned int>(EnRootParameterType::enNumRootParamerterTypes);
				constexpr unsigned int kParamNoForModel =
					static_cast<unsigned int>(EnRootParameterType::enModel);
				constexpr unsigned int kParamNoForMaterial =
					static_cast<unsigned int>(EnRootParameterType::enMaterial);

				pRootParameters->resize(kNumRootParameters);

				pRootParameters->at(kParamNoForModel).InitAsDescriptorTable(
					1,
					&pDescTblRanges->at(kRangeNoForModelCB),
					D3D12_SHADER_VISIBILITY_ALL
				);
				pRootParameters->at(kParamNoForMaterial).InitAsDescriptorTable(
					1,
					&pDescTblRanges->at(kRangeNoForMaterialSRV),
					D3D12_SHADER_VISIBILITY_PIXEL
				);

				return;
			}


			bool CSkyCubeGenericRenderer::CreateShader(
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



		}
	}
}