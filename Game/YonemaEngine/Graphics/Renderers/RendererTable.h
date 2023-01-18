#pragma once
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsRenderers
		{
			class IGenericRenderer;
			class IRenderer;
		}
	}
}
#include "GenericRenderer.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsRenderers
		{
			class CRendererTable : private nsUtils::SNoncopyable
			{
			public:
				enum class EnRendererType
				{
					enNone = -1,
					enBasicModel,
					enSkinModel,
					enInstancingModel,
					enSkyCube,
					enCollisionRenderer,
					enSimplePostEffect,
					enSprite,
					enTransSprite,
					enNumType
				};
			private:
				using RendererList = std::list<IRenderer*>;


			public:
				constexpr CRendererTable() = default;
				~CRendererTable();

				void Init();

				constexpr auto GetRootSignature(EnRendererType rendererType) noexcept
				{
					return GetGenericRenderer(rendererType)->GetRootSignature();
				}
				constexpr auto GetPipelineState(EnRendererType rendererType) noexcept
				{
					return GetGenericRenderer(rendererType)->GetPipelineState();
				}

				constexpr IGenericRenderer* GetGenericRenderer(EnRendererType rendererType) noexcept
				{
					return m_genericRendererTable[static_cast<int>(rendererType)];
				}

				constexpr RendererList& GetRendererList(EnRendererType rendererType) noexcept
				{
					return m_rendererListTable[static_cast<int>(rendererType)];
				}

				inline void RegisterRenderer(EnRendererType rendererType, IRenderer* renderer)
				{
					GetRendererList(rendererType).emplace_back(renderer);
				}

				inline void RemoveRenderer(EnRendererType rendererType, IRenderer* renderer)
				{
					GetRendererList(rendererType).remove(renderer);
				}

			private:
				void Terminate();

				void InitGenericRenderTable();


			private:
				IGenericRenderer* 
					m_genericRendererTable[static_cast<int>(EnRendererType::enNumType)] = {nullptr};
				RendererList m_rendererListTable[static_cast<int>(EnRendererType::enNumType)] = {};
			};
		}
	}
}