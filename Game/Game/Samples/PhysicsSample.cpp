#include "PhysicsSample.h"
#include "../../YonemaEngine/DebugSystem/SimpleMover.h"
#include <random>

namespace nsAWA
{
	namespace nsSamples
	{
		bool CPhysicsSample::Start()
		{
			MainCamera()->SetFarClip(1000.0f);

			// �f�o�b�N�p��Physics�̃��C���`��@�\�̗L�����B
			EnableDebugDrawPhysicsLine();
			// ���C���̃J�����O�{�b�N�X�̗L�����B
			SetCullingBoxForDebugDrawLine(50.0f, nsMath::CVector3::Zero());
			// ���C���̃J�����O�{�b�N�X�̎����J�����t�B�b�g�@�\�̗L�����B
			EnableAutoFitCullingBoxToMainCamera();


			// �v���C���[�̍쐬
			m_simpleMover = NewGO<nsYMEngine::nsDebugSystem::CSimpleMover>();
			m_simpleMover->SetPosition({ 0.0f,0.0f,0.0f });
			m_staticPlayer.InitAsSphere(5.0f, { 0.0f,0.0f,0.0f });


			// �n�ʂƂȂ�v���[�����쐬
			nsMath::CVector3 nVec = { 0.0f,1.0f,0.0f };
			nVec.Normalize();
			m_staticPlane.InitAsPlane(nVec, 0.0f);


			// ��������
			constexpr float distribute = 200.0f;	// �����͈̔�
			std::random_device rnd;     // �񌈒�I�ȗ���������𐶐�
			std::mt19937 mt(rnd());     //  �����Z���k�E�c�C�X�^��32�r�b�g�ŁA�����͏����V�[�h�l
			std::uniform_real_distribution<> rand(-distribute * 0.5f, distribute * 0.5f); // �͈͂̈�l����


			// ��ʂ̕����I�u�W�F�N�g�̐���
			constexpr float shapeSize = 2.0f;
			constexpr float heightPos = 20.0f;
			for (int i = 0; i < m_kNumPhysicsObjects; i++)
			{
				m_dynamicBox[i].InitAsBox(
					{ shapeSize, shapeSize, shapeSize },
					{ static_cast<float>(rand(mt)), heightPos, static_cast<float>(rand(mt))}
				);
				m_dynamicSphere[i].InitAsSphere(
					shapeSize,
					{ static_cast<float>(rand(mt)), heightPos, static_cast<float>(rand(mt)) }
				);
				m_dynamicCapusule[i].InitAsCapsule(
					shapeSize,
					shapeSize,
					{ static_cast<float>(rand(mt)), heightPos, static_cast<float>(rand(mt)) }
				);
			}

			return true;
		}

		void CPhysicsSample::OnDestroy()
		{
			for (int i = 0; i < m_kNumPhysicsObjects; i++)
			{
				m_dynamicCapusule[i].Release();
				m_dynamicSphere[i].Release();
				m_dynamicBox[i].Release();
			}

			m_staticPlane.Release();
			return;
		}

		void CPhysicsSample::Update(float deltaTime)
		{
			m_staticPlayer.SetPositionAndRotation(
				m_simpleMover->GetPosition(),
				m_simpleMover->GetRotation()
				);


			nsMath::CVector3 toCameraVec(0.0f, 10.0f, -20.0f);
			m_simpleMover->GetRotation().Apply(toCameraVec);
			nsMath::CVector3 cameraPos = m_simpleMover->GetPosition() + toCameraVec;

			nsMath::CVector3 toTargetVec(0.0f, 0.0f, 5.0f);
			m_simpleMover->GetRotation().Apply(toTargetVec);
			nsMath::CVector3 targetPos = m_simpleMover->GetPosition() + toTargetVec;
			

			MainCamera()->SetPosition(cameraPos);
			MainCamera()->SetTargetPosition(targetPos);

			return;
		}
	}
}