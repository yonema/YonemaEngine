#pragma once


namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsRenderers
		{
			class IRenderer : private nsUtils::SNoncopyable
			{
			protected:
				virtual void Draw(nsDx12Wrappers::CCommandList* commandList) = 0 {};


			public:
				constexpr void SetRenderType(CRendererTable::EnRendererType renderType) noexcept
				{
					m_renderType = renderType;
				}
				constexpr CRendererTable::EnRendererType GetRenderType() const noexcept
				{
					return m_renderType;
				}
				void EnableDrawing();

				void DisableDrawing();

				constexpr bool IsEnableDrawing() const noexcept
				{
					return m_isEnableDrawing;
				}

			public:
				constexpr IRenderer() = default;
				virtual ~IRenderer() = default;

				inline void DrawWrapper(nsDx12Wrappers::CCommandList* commandList)
				{
					Draw(commandList);
				}

			private:
				CRendererTable::EnRendererType m_renderType =
					CRendererTable::EnRendererType::enNone;
				bool m_isEnableDrawing = false;

			};
		}

	}
}