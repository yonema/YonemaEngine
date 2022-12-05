#pragma once

namespace nsAWA
{
	namespace nsSamples
	{

		//////////////// CDeathTrigger ////////////////
		
		/**
		 * @brief CRigidPlayerが入ったら死ぬトリガー
		*/
		class CDeathTrigger : public IGameObject
		{
		public:
			bool Start() override final;

			void OnDestroy() override final;

			void Update(float deltaTime) override final;

		public:
			constexpr CDeathTrigger() = default;
			~CDeathTrigger() = default;

		private:

			/**
			 * @brief トリガーに何か入った瞬間に呼ばれるコールバック関数
			 * @param[in,out] otherData トリガーに入ってきた物理アクターの拡張データ
			*/
			void OnTriggerEnter(CExtendedDataForRigidActor* otherData);


		private:
			CPhysicsTriggerObject m_triggerBox;
		};


		//////////////// CWallTrigger ////////////////

		/**
		 * @brief 壁属性を持つトリガー
		*/
		class CWallTrigger : public IGameObject
		{
		public:
			bool Start() override final;

			void OnDestroy() override final;

			void Update(float deltaTime) override final;

		public:
			constexpr CWallTrigger() = default;
			~CWallTrigger() = default;

		private:
			CPhysicsTriggerObject m_triggerBox;
		};


		//////////////// CRigidPlayer ////////////////

		/**
		 * @brief 物理的なPlayer
		*/
		class CRigidPlayer : public IGameObject
		{
		public:
			bool Start() override final;

			void OnDestroy() override final;

			void Update(float deltaTime) override final;

		private:

			/**
			 * @brief トリガーに何か入った瞬間に呼ばれるコールバック関数
			 * @param[in,out] otherData トリガーに入ってきた物理アクターの拡張データ
			*/
			void OnTriggerEnter(CExtendedDataForRigidActor* otherData);

			/**
			 * @brief トリガーに何か入っている間、呼ばれ続けるコールバック関数
			 * @param[in,out] otherData トリガーに入っている物理アクターの拡張データ
			*/
			void OnTriggerStay(CExtendedDataForRigidActor* otherData);

			/**
			 * @brief トリガーから何か出て行った瞬間に呼ばれるコールバック関数
			 * @param[in,out] otherData トリガーから出て行った物理アクターの拡張データ
			*/
			void OnTriggerExit(CExtendedDataForRigidActor* otherData);

		public:
			constexpr CRigidPlayer() = default;
			~CRigidPlayer() = default;

		private:

		private:

			//// 物理オブジェクト ////

			CPhysicsDynamicObject m_dynamicSphere;
			CPhysicsTriggerObject m_triggerBox;


			//// 結果確認用のフォント表示メンバ ////

			CFontRenderer* m_enterCounterFR = nullptr;
			CFontRenderer* m_stayTimerFR = nullptr;
			CFontRenderer* m_exitCounterFR = nullptr;
			int m_enterCounter = 0;
			float m_stayTimer = 0.0f;
			int m_exitCounter = 0;
			wchar_t m_enterCounterWChar[64] = {};
			wchar_t m_stayTimerWChar[64] = {};
			wchar_t m_exitCounterWChar[64] = {};
		};


		//////////////// CTriggerSample ////////////////
		class CTriggerSample : public IGameObject
		{
		public:
			bool Start() override final;

			void OnDestroy() override final;

			void Update(float deltaTime) override final;

		public:
			constexpr CTriggerSample() = default;
			~CTriggerSample() = default;

		private:
			CPhysicsStaticObject m_staticPlane;

			CDeathTrigger* m_deathTrigger = nullptr;
			CWallTrigger* m_wallTrigger = nullptr;
			CRigidPlayer* m_rigidPlayer = nullptr;
		};
	}
}

