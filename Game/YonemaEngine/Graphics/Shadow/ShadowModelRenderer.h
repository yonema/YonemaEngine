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

			public:
				constexpr CShadowModelRenderer() = default;
				~CShadowModelRenderer();

				void Init(
					const std::function<void(nsDx12Wrappers::CCommandList* commandList)>& drawFunc,
					nsRenderers::CRendererTable::EnRendererType baseModelType
					);

				void Release();

				void Update(const nsMath::CMatrix& worldMatrix);

				constexpr bool IsValid() const noexcept
				{
					return IsEnableDrawing();
				}

			private:
				void Terminate();

				void CreateModelCBV();

			private:
				std::function<void(nsDx12Wrappers::CCommandList* commandList)> m_drawFunc = {};
				nsDx12Wrappers::CConstantBuffer m_modelCB = {};
				nsDx12Wrappers::CDescriptorHeap m_modelDH = {};
			};

		}
	}
}