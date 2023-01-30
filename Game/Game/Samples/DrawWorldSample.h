#pragma once
namespace nsYMEngine
{
	namespace nsDebugSystem
	{
		class CSimpleMover;
	}
}

namespace nsAWA
{
	namespace nsSamples
	{
		class CHumans : public IGameObject
		{
		public:
			enum class EnAnimType
			{
				enIdle,
				enNum
			};

			static const char* const m_kAnimFilePaths[static_cast<int>(EnAnimType::enNum)];
			static const std::unordered_map<std::string, const char* const> m_kNameToFilePath;
			static const char* const m_kRetargetSkeltonName;
			static const float m_kModelScale;

		public:
			bool Start() override final;

			void OnDestroy() override final;

			void Update(float deltaTime) override final;
		public:
			constexpr CHumans() = default;
			~CHumans() = default;

			void Init(
				const char* const keyname,
				const CVector3& pos,
				const CQuaternion& rot
				) noexcept;

			static void RegisterBase() noexcept;

			constexpr const CVector3& GetPosition() const noexcept
			{
				return m_modelRenderer ? m_modelRenderer->GetPosition() : CVector3::Zero();
			}

		private:
			CModelRenderer* m_modelRenderer = nullptr;
		};




		class CDebugPlayer : public IGameObject
		{
		public:
			bool Start() override final;

			void OnDestroy() override final;

			void Update(float deltaTime) override final;
		public:
			constexpr CDebugPlayer() = default;
			~CDebugPlayer() = default;

			constexpr const CVector3& GetPosition() const noexcept
			{
				return m_modelRenderer ? m_modelRenderer->GetPosition() : CVector3::Zero();
			}

		private:
			CModelRenderer* m_modelRenderer = nullptr;
			nsYMEngine::nsDebugSystem::CSimpleMover* m_simpleMover = nullptr;
		};



		class CDrawWorldSample : public IGameObject
		{
		public:
			bool Start() override final;

			void OnDestroy() override final;

			void Update(float deltaTime) override final;
		public:
			constexpr CDrawWorldSample() = default;
			~CDrawWorldSample() = default;

		private:
			CLevel3D m_level3D = {};
			CDebugPlayer* m_debugPlayer = nullptr;
			std::unordered_map<std::string, CHumans*> m_humansMap = {};
			CFontRenderer* m_fontRenderer = nullptr;
		};

	}
}