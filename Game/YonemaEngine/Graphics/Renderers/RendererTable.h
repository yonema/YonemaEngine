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
					enShadowModel,
					enShadowSkinModel,
					enShadowInstancingModel,
					enGaussianBlurXForShadowMap,
					enGaussianBlurYForShadowMap,
					enBasicModel,
					enSkinModel,
					enInstancingModel,
					enBasicNonCullingModel,
					enSkinNonCullingModel,
					enInstancingNonCullingModel,
					enSkyCube,
					enCollisionRenderer,
					enSimplePostEffect,
					enSprite,
					enTransSprite,
					enNumType
				};

				static const unsigned int m_kFirstIndexOfModel =
					static_cast<unsigned int>(EnRendererType::enBasicModel);
				static const unsigned int m_kLastIndexOfModel =
					static_cast<unsigned int>(EnRendererType::enInstancingNonCullingModel);

				static const unsigned int m_kFirstIndexOfShadowModel =
					static_cast<unsigned int>(EnRendererType::enShadowModel);
				static const unsigned int m_kLastIndexOfShadowModel =
					static_cast<unsigned int>(EnRendererType::enShadowInstancingModel);

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
				constexpr auto GetRootSignature(unsigned int rendererType) noexcept
				{
					return GetGenericRenderer(rendererType)->GetRootSignature();
				}

				constexpr auto GetPipelineState(EnRendererType rendererType) noexcept
				{
					return GetGenericRenderer(rendererType)->GetPipelineState();
				}
				constexpr auto GetPipelineState(unsigned int rendererType) noexcept
				{
					return GetGenericRenderer(rendererType)->GetPipelineState();
				}

				constexpr IGenericRenderer* GetGenericRenderer(EnRendererType rendererType) noexcept
				{
					return GetGenericRenderer(static_cast<unsigned int>(rendererType));
				}
				constexpr IGenericRenderer* GetGenericRenderer(unsigned int rendererType) noexcept
				{
					return m_genericRendererTable[rendererType];
				}

				constexpr RendererList& GetRendererList(EnRendererType rendererType) noexcept
				{
					return GetRendererList(static_cast<int>(rendererType));
				}
				constexpr RendererList& GetRendererList(unsigned int rendererType) noexcept
				{
					return m_rendererListTable[rendererType];
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