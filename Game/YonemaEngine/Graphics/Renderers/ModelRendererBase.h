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

				virtual inline void PlayAnimation(unsigned int animIdx) noexcept {};

				virtual inline bool IsPlaying() const noexcept { return false; };

				virtual inline void SetAnimationSpeed(float animSpeed) noexcept {};

				virtual inline void SetIsAnimationLoop(bool isLoop) noexcept {};

			public:
				constexpr IModelRendererBase() = default;
				virtual ~IModelRendererBase() = default;

			private:
			};

		}
	}
}