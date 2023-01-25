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
			private:
				static const std::string m_kAnimEventKeyNodeName;

			public:
				constexpr CAnimationClip() = default;
				~CAnimationClip();

				bool Init(const char* animFilePath, bool registerAnimBank);

				void Release();

				void CalcAndGetAnimatedBoneTransforms(
					float timeInSeconds,
					std::vector<nsMath::CMatrix>* pMTransforms,
					CSkelton* pSkelton,
					unsigned int animIdx = 0,
					bool isLoop = true
				) noexcept;

				constexpr bool IsPlayedAnimationToEnd() const noexcept
				{
					return m_isPlayedAnimationToEnd;
				}

				constexpr void ResetAnimationParam() noexcept
				{
					m_prevAnimEventIdx = 0;
					m_isPlayedAnimationToEnd = false;
					m_animLoopCounter = 0;
				}

				_CONSTEXPR20_CONTAINER void ReserveAnimationEventFuncArray(unsigned int size)
				{
					m_animationEventFuncArray.reserve(static_cast<size_t>(size));
				}

				_CONSTEXPR20_CONTAINER void AddAnimationEventFunc(
					const std::function<void(void)>& animationEventFunc)
				{
					m_animationEventFuncArray.emplace_back(animationEventFunc);
				}

				constexpr bool IsLoaded() const noexcept
				{
					return m_isLoaded;
				}

				constexpr bool IsShared() const noexcept
				{
					return m_isShared;
				}

				constexpr void SetIsShared(bool isShared) noexcept
				{
					m_isShared = isShared;
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
					const aiAnimation& animation,
					CSkelton* pSkelton,
					unsigned int animIdx
				) noexcept;

				const aiNodeAnim* FindNodeAnim(
					const aiAnimation& Animation,
					const std::string& NodeName,
					unsigned int animIdx) const noexcept;

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



				void ReadAnimKeyEventNode(
					float animTimeTicks,
					const aiNode& node,
					const aiAnimation& animation,
					unsigned int animIdx
				) noexcept;

			private:
				Assimp::Importer* m_importer = nullptr;
				const aiScene* m_scene = nullptr;
				bool m_isPlayedAnimationToEnd = false;
				unsigned int m_prevAnimEventIdx = 0;
				int m_animLoopCounter = 0;
				std::vector<std::function<void(void)>> m_animationEventFuncArray = {};
				bool m_isLoaded = false;
				bool m_isShared = false;
				std::vector<std::unordered_map<std::string, const aiNodeAnim*>>
					m_nodeAnimMapArray = {};
			};

		}
	}
}