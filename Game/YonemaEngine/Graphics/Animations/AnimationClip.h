#pragma once
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsAssimpCommon
		{
			struct SLocalTransform;
		}
		namespace nsAnimations
		{
			class CSkelton;
		}
	}
}
namespace Assimp
{
	class Importer;
}

typedef float ai_real;

template <typename TReal>
class aiVector3t;
typedef aiVector3t<ai_real> aiVector3D;

template <typename TReal>
class aiQuaterniont;
typedef aiQuaterniont<ai_real> aiQuaternion;

struct aiScene;
struct aiNode;
struct aiAnimation;
struct aiNodeAnim;

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsAnimations
		{
			class CAnimationClip : nsUtils::SNoncopyable
			{
			public:
				constexpr CAnimationClip() = default;
				~CAnimationClip();

				bool Init(const char* animFilePath, CSkelton* pSkelton);

				void Release();

				void CalcAndGetAnimatedBoneTransforms(
					float timeInSeconds,
					std::vector<nsMath::CMatrix>* pMTransforms,
					unsigned int animIdx = 0,
					bool isLoop = true
				) noexcept;

				constexpr bool IsPlayedAnimationToEnd() const noexcept
				{
					return m_isPlayedAnimationToEnd;
				}

			private:

				void Terminate();

				bool ImportScene(const char* animFilePath);

				float CalcAnimationTimeTicks(
					float timeInSeconds, unsigned int animIdx, bool isLoop) noexcept;

				void ReadNodeHierarchy(
					float animTimeTicks,
					const aiNode& node,
					const nsMath::CMatrix& parentTransform,
					const aiAnimation& animation
				) noexcept;

				const aiNodeAnim* FindNodeAnim(
					const aiAnimation& Animation, const std::string& NodeName) const noexcept;

				void CalcLocalTransform(
					nsAssimpCommon::SLocalTransform& localTransform,
					float animTimeTicks,
					const aiNodeAnim& nodeAnim
				) const noexcept;
				void CalcInterpolatedScaling(
					aiVector3D* pScaling, float animTimeTicks, const aiNodeAnim& nodeAnim) const noexcept;

				void CalcInterpolatedRotation(
					aiQuaternion* pRotation, float animTimeTicks, const aiNodeAnim& nodeAnim) const noexcept;

				void CalcInterpolatedPosition(
					aiVector3D* pPosition, float animTimeTicks, const aiNodeAnim& nodeAnim) const noexcept;

				unsigned int FindScaling(float animTimeTicks, const aiNodeAnim& nodeAnim) const noexcept;

				unsigned int FindRotation(float animTimeTicks, const aiNodeAnim& nodeAnim) const noexcept;

				unsigned int FindPosition(float animTimeTicks, const aiNodeAnim& nodeAnim) const noexcept;

			private:
				Assimp::Importer* m_importer = nullptr;
				const aiScene* m_scene = nullptr;
				CSkelton* m_skeltonRef = nullptr;
				bool m_isPlayedAnimationToEnd = false;
			};

		}
	}
}