#pragma once
#include "../Renderers/Renderer.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsShadow
		{
			class CShadowModelRenderer : public nsRenderers::IRenderer
			{
			private:
				void Draw(nsDx12Wrappers::CCommandList* commandList) override final;

			private:

				struct SConstantBufferData : nsUtils::SNoncopyable
				{
					constexpr SConstantBufferData() = default;
					~SConstantBufferData() = default;

					nsMath::CMatrix mWorld;
					nsMath::CMatrix mLightViewProj;
					nsMath::CVector3 lightPos;	
				};

				enum class EnDescHeapLayout
				{
					enModelCB,
					enBoneMatrixArraySRV,
					enWorldMatrixArraySRV,
					enNum
				};

			public:
				constexpr CShadowModelRenderer() = default;
				~CShadowModelRenderer();

				void Init(
					const std::function<void(nsDx12Wrappers::CCommandList* commandList)>& drawFunc,
					nsRenderers::CRendererTable::EnRendererType baseModelType,
					const std::vector<nsMath::CMatrix>* pBoneMatrixArray = nullptr,
					unsigned int maxInstance = 1
					);

				void Release();

				void Update(const nsMath::CMatrix& worldMatrix);

				constexpr bool IsValid() const noexcept
				{
					return IsEnableDrawing();
				}

				constexpr bool IsDrawingFlag() const noexcept
				{
					return m_drawingFlag;
				}

				constexpr void SetDrawingFlag(bool drawingFlag) noexcept
				{
					m_drawingFlag = drawingFlag;
				}

				void UpdateBoneMatrixArray(
					const std::vector<nsMath::CMatrix>* pBoneMatrixArray) noexcept;

				void UpdateWorldMatrixArray(
					const std::vector<nsMath::CMatrix>* pWrldMatrixArray,
					unsigned int fixNumInstanceOnFrame) noexcept;

			private:
				void Terminate();

				void CreateModelCBV();

				void CreateBoneMatrixArraySB(const std::vector<nsMath::CMatrix>* pBoneMatrixArray);

				void CreateWorldMatrixArraySB(unsigned int maxInstance);

			private:
				std::function<void(nsDx12Wrappers::CCommandList* commandList)> m_drawFunc = {};
				nsDx12Wrappers::CConstantBuffer m_modelCB = {};
				nsDx12Wrappers::CStructuredBuffer m_boneMatrixArraySB = {};
				nsDx12Wrappers::CStructuredBuffer m_worldMatrixArraySB = {};
				nsDx12Wrappers::CDescriptorHeap m_descriptorHeap = {};
				nsDx12Wrappers::CDescriptorHandle m_descHandle = {};
				bool m_drawingFlag = false;
				bool m_isSkeltalAnimation = false;
				bool m_isInstancing = false;
			};

		}
	}
}