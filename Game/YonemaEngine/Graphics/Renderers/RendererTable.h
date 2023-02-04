#pragma once
#include "GenericRenderer.h"
#include "../../../Game/RendererPriority.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsRenderers
		{
			class IRenderer;
		}
	}
}

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
				using RendererListArray = std::array<RendererList, g_kNumRendererPriority>;


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

				constexpr RendererListArray& GetRendererListArray(EnRendererType rendererType) noexcept
				{
					return GetRendererListArray(static_cast<int>(rendererType));
				}
				constexpr RendererListArray& GetRendererListArray(unsigned int rendererType) noexcept
				{
					return m_rendererListArrayTable[rendererType];
				}

				inline void RegisterRenderer(
					EnRendererType rendererType,
					IRenderer* renderer,
					EnRendererPriority priority = EnRendererPriority::enMid
				)
				{
					RegisterRenderer(rendererType, renderer, static_cast<unsigned int>(priority));
				}
				inline void RegisterRenderer(
					EnRendererType rendererType, IRenderer* renderer, unsigned int priority)
				{
					GetRendererListArray(rendererType)[priority].emplace_back(renderer);
				}


				inline void RemoveRenderer(EnRendererType rendererType, IRenderer* renderer)
				{
					auto& rendererListArray = GetRendererListArray(rendererType);
					for (auto& rendererList : rendererListArray)
					{
						bool isRemoved = false;
						rendererList.remove_if(
							[&](IRenderer* item)
							{
								if (item == renderer)
								{
									isRemoved = true;
									return true;
								}

								return false;
							}
						);

						if (isRemoved)
						{
							break;
						}
					}
				}

			private:
				void Terminate();

				void InitGenericRenderTable();


			private:
				IGenericRenderer* 
					m_genericRendererTable[static_cast<int>(EnRendererType::enNumType)] = {nullptr};
				RendererListArray m_rendererListArrayTable[static_cast<int>(EnRendererType::enNumType)] = {};
			};
		}
	}
}