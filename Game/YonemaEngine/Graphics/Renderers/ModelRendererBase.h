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

				virtual inline void ReserveAnimationEventFuncArray(
					unsigned int animIdx, unsigned int size) {};

				virtual inline void AddAnimationEventFunc(
					unsigned int animIdx,
					const std::function<void(void)>& animationEventFunc) {};

				virtual unsigned int FindBoneId(const std::string& boneName) const noexcept
				{ return 0; };

				virtual inline const nsMath::CMatrix& GetBoneMatrix(
					unsigned int boneId) const noexcept
				{ return nsMath::CMatrix::Identity(); };

				virtual inline const nsMath::CMatrix& GetWorldMatrix() const noexcept
				{ return nsMath::CMatrix::Identity(); };


			public:
				constexpr IModelRendererBase() = default;
				virtual ~IModelRendererBase() = default;

			private:
			};

		}
	}
}