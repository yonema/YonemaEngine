#pragma once
#include "PhysicsObject.h"
namespace physx
{
	class PxRigidStatic;
	class PxGeometry;
	class PxTriangleMesh;
}

namespace nsYMEngine
{
	namespace nsPhysics
	{
		/**
		 * @brief CPhysicsStaticObject�����b�V���Ƃ��ď���������Ƃ��ɕK�v�ȃf�[�^�B
		 * @details ���̃f�[�^���\�z����菇�͈ȉ��̒ʂ�ł��B
		 * 1.SMeshGeometryBuffer�̃I�u�W�F�N�g�𐶐����܂��B
		 * 2.SModelInitData�̃I�u�W�F�N�g�𐶐����܂��B
		 * 3.SModelInitData�ɁASMeshGeometryBuffer�̃|�C���^��ݒ肵�܂��B
		 * 4.SModelInitData���g����CModelRenderer�����������܂��B
		 * ������SModelInitData�ɓn����SMeshGeometryBuffer�̃f�[�^���\�z����܂��B
		*/
		struct SMeshGeometryBuffer : nsUtils::SNoncopyable
		{
			std::vector<nsMath::CVector3> m_vertices;
			std::vector<uint16_t> m_indices;

			void RotateVertices(const nsMath::CQuaternion& rotation) noexcept;
		};

		class CPhysicsStaticObject : public IPhysicsObject
		{
		private:
			bool InitCore(
				const physx::PxGeometry& geometry,
				const nsMath::CVector3& position
			) override final;

		public:
			constexpr CPhysicsStaticObject() = default;
			~CPhysicsStaticObject();

			/**
			 * @brief ���ʂƂ��ď�����(StaticObject����)
			 * @param[in] normal ���ʂ̖@��
			 * @param[in] distance ���_����̋���
			 * @param[in] attribute ��������
			 * @return �������ɐ����������H
			*/
			bool InitAsPlane(
				const nsMath::CVector3& normal,
				float distance,
				EnPhysicsAttributes attribute = EnPhysicsAttributes::enNone);

			/**
			 * @brief ���b�V���Ƃ��ď�����(StaticObject����)
			 * @param[in] meshGeometryBuffer ���b�V���Ƃ��ď���������Ƃ��ɕK�v�ȃf�[�^�B
			 * @param[in] position ���W
			 * @param[in] scaleBias �������郁�b�V���̃X�P�[�����O
			 * @param[in] attribute ��������
			 * @return �������ɐ����������H
			*/
			bool InitAsMesh(
				const SMeshGeometryBuffer& meshGeometryBuffer,
				const nsMath::CVector3& position = nsMath::CVector3::Zero(),
				float scaleBias = 1.0f,
				EnPhysicsAttributes attribute = EnPhysicsAttributes::enNone
			);

			void Release();

		private:

			void Terminate();

			physx::PxTriangleMesh* CreateTriangleMesh(
				const SMeshGeometryBuffer& meshGeometryBuffer);

		private:
			physx::PxRigidStatic* m_rigidStatic = nullptr;

		};

	}
}