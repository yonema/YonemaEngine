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
		 * @brief CPhysicsStaticObjectをメッシュとして初期化するときに必要なデータ。
		 * @details このデータを構築する手順は以下の通りです。
		 * 1.SMeshGeometryBufferのオブジェクトを生成します。
		 * 2.SModelInitDataのオブジェクトを生成します。
		 * 3.SModelInitDataに、SMeshGeometryBufferのポインタを設定します。
		 * 4.SModelInitDataを使ってCModelRendererを初期化します。
		 * ここでSModelInitDataに渡したSMeshGeometryBufferのデータが構築されます。
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
			 * @brief 平面として初期化(StaticObject限定)
			 * @param[in] normal 平面の法線
			 * @param[in] distance 原点からの距離
			 * @param[in] attribute 物理属性
			 * @return 初期化に成功したか？
			*/
			bool InitAsPlane(
				const nsMath::CVector3& normal,
				float distance,
				EnPhysicsAttributes attribute = EnPhysicsAttributes::enNone);

			/**
			 * @brief メッシュとして初期化(StaticObject限定)
			 * @param[in] meshGeometryBuffer メッシュとして初期化するときに必要なデータ。
			 * @param[in] position 座標
			 * @param[in] scaleBias 生成するメッシュのスケーリング
			 * @param[in] attribute 物理属性
			 * @return 初期化に成功したか？
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