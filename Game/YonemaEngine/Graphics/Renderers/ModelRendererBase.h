#pragma once
#include "Renderer.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsRenderers
		{
			class IModelRendererBase : public IRenderer
			{
			public:
				virtual void UpdateWorldMatrix(
					const nsMath::CVector3& position,
					const nsMath::CQuaternion& rotation,
					const nsMath::CVector3& scale
				) = 0 {};

				virtual void UpdateAnimation(float deltaTime) {};

				virtual void PlayAnimation() {};

			public:
				constexpr IModelRendererBase() = default;
				virtual ~IModelRendererBase() = default;

			private:
			};

		}
	}
}