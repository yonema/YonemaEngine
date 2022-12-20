#include "PhysicsStaticObject.h"
#include <PxPhysicsAPI.h>

namespace nsYMEngine
{
	namespace nsPhysics
	{
		CPhysicsStaticObject::~CPhysicsStaticObject()
		{
			Terminate();
			return;
		}

		void CPhysicsStaticObject::Terminate()
		{
			Release();
			return;
		}

		void CPhysicsStaticObject::Release()
		{
			if (m_rigidStatic == nullptr)
			{
				return;
			}

			ReleaseRigidActor();
			m_rigidStatic = nullptr;

			return;
		}

		bool CPhysicsStaticObject::InitCore(
			const physx::PxGeometry& geometry,
			const nsMath::CVector3& position
		)
		{
			Release();

			auto physics = GetPhysics();
			auto material = GetDefaultMaterial();

			m_rigidStatic = PxCreateStatic(
				*physics,
				physx::PxTransform(position.x, position.y, position.z),
				geometry,
				*material
			);

			if (m_rigidStatic == nullptr)
			{
				return false;
			}

			InitRigidActor(m_rigidStatic);

			return true;
		}

		bool CPhysicsStaticObject::InitAsPlane(
			const nsMath::CVector3& normal, float distance, EnPhysicsAttributes attribute)
		{
			Release();

			auto physics = GetPhysics();
			auto material = GetDefaultMaterial();

			m_rigidStatic = PxCreatePlane(
				*physics, physx::PxPlane(normal.x, normal.y, normal.z, distance), *material);

			if (m_rigidStatic == nullptr)
			{
				return false;
			}

			InitRigidActor(m_rigidStatic);

			InitExtendedData(this, attribute);

			return true;
		}


		bool CPhysicsStaticObject::InitAsMesh(
			const SMeshGeometryBuffer& meshGeometryBuffer,
			const nsMath::CVector3& position,
			float scaleBias,
			EnPhysicsAttributes attribute
		)
		{
			auto* triangleMesh = CreateTriangleMesh(meshGeometryBuffer);
			if (triangleMesh == nullptr)
			{
				return false;
			}

			auto res = InitCore(
				physx::PxTriangleMeshGeometry(triangleMesh, physx::PxMeshScale(scaleBias)),
				position);

			InitExtendedData(this, attribute);

			return res;
		}


		physx::PxTriangleMesh* CPhysicsStaticObject::CreateTriangleMesh(
			const SMeshGeometryBuffer& meshGeometryBuffer)
		{
			physx::PxTriangleMeshDesc meshDesc;
			meshDesc.setToDefault();

			meshDesc.points.count =
				static_cast<physx::PxU32>(meshGeometryBuffer.m_vertices.size());
			meshDesc.points.stride = sizeof(meshGeometryBuffer.m_vertices[0]);
			meshDesc.points.data = meshGeometryBuffer.m_vertices.data();

			meshDesc.triangles.count =
				static_cast<physx::PxU32>(meshGeometryBuffer.m_indices.size() / 3);
			meshDesc.triangles.stride = 3 * sizeof(meshGeometryBuffer.m_indices[0]);
			meshDesc.triangles.data = meshGeometryBuffer.m_indices.data();

			meshDesc.flags = physx::PxMeshFlags();
			meshDesc.flags |= physx::PxMeshFlag::e16_BIT_INDICES;

			if (meshDesc.isValid() != true)
			{
				return nullptr;
			}

			const auto& cooking = GetCooking();
			const auto& physics = GetPhysics();

			physx::PxDefaultMemoryOutputStream writeBuffer = {};
			physx::PxTriangleMeshCookingResult::Enum result;
			bool status = cooking->cookTriangleMesh(meshDesc, writeBuffer, &result);
			if (status != true)
			{
				return nullptr;
			}

			physx::PxDefaultMemoryInputData readBuffer(
				writeBuffer.getData(), writeBuffer.getSize());


			return physics->createTriangleMesh(readBuffer);
		}



	}
}