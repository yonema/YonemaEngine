#pragma once

namespace nsAWA
{
	namespace nsSamples
	{

		//////////////// CDeathTrigger ////////////////
		
		/**
		 * @brief CRigidPlayer���������玀�ʃg���K�[
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
			 * @brief �g���K�[�ɉ����������u�ԂɌĂ΂��R�[���o�b�N�֐�
			 * @param[in,out] otherData �g���K�[�ɓ����Ă��������A�N�^�[�̊g���f�[�^
			*/
			void OnTriggerEnter(CExtendedDataForRigidActor* otherData);


		private:
			CPhysicsTriggerObject m_triggerBox;
		};


		//////////////// CWallTrigger ////////////////

		/**
		 * @brief �Ǒ��������g���K�[
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
		 * @brief �����I��Player
		*/
		class CRigidPlayer : public IGameObject
		{
		public:
			bool Start() override final;

			void OnDestroy() override final;

			void Update(float deltaTime) override final;

		private:

			/**
			 * @brief �g���K�[�ɉ����������u�ԂɌĂ΂��R�[���o�b�N�֐�
			 * @param[in,out] otherData �g���K�[�ɓ����Ă��������A�N�^�[�̊g���f�[�^
			*/
			void OnTriggerEnter(CExtendedDataForRigidActor* otherData);

			/**
			 * @brief �g���K�[�ɉ��������Ă���ԁA�Ă΂ꑱ����R�[���o�b�N�֐�
			 * @param[in,out] otherData �g���K�[�ɓ����Ă��镨���A�N�^�[�̊g���f�[�^
			*/
			void OnTriggerStay(CExtendedDataForRigidActor* otherData);

			/**
			 * @brief �g���K�[���牽���o�čs�����u�ԂɌĂ΂��R�[���o�b�N�֐�
			 * @param[in,out] otherData �g���K�[����o�čs���������A�N�^�[�̊g���f�[�^
			*/
			void OnTriggerExit(CExtendedDataForRigidActor* otherData);

		public:
			constexpr CRigidPlayer() = default;
			~CRigidPlayer() = default;

		private:

		private:

			//// �����I�u�W�F�N�g ////

			CPhysicsDynamicObject m_dynamicSphere;
			CPhysicsTriggerObject m_triggerBox;


			//// ���ʊm�F�p�̃t�H���g�\�������o ////

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

