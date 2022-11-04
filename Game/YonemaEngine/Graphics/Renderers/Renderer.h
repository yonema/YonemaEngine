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
				constexpr IRenderer() = default;
				virtual ~IRenderer() = default;

				inline void DrawWrapper(nsDx12Wrappers::CCommandList* commandList)
				{
					Draw(commandList);
				}

			private:

			};
		}

	}
}