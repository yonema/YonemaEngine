#include "GeometryData.h"
#include "../Graphics/Models/BasicModelRenderer.h"
#include "../Graphics/GraphicsEngine.h"
#include "../Physics/PhysicsEngine.h"

namespace nsYMEngine
{
	namespace nsGeometries
	{
		void CGeometryData::Init(const std::vector<SMesh>& meshes) noexcept
		{
			m_aabb.InitFromMeshes(meshes);

			return;
		}

		void CGeometryData::Update(const nsMath::CMatrix& mWorld) noexcept
		{
			nsMath::CVector3 vertexPos[CAABB::m_kNumVertesPos];
			m_aabb.CalcVertexPositions(vertexPos, mWorld);

			//DrawDebugLine(vertexPos);

			const auto& mViewProj = nsGraphics::CGraphicsEngine::GetInstance()->GetMatrixViewProj();
			
			// デバック用
			//nsMath::CMatrix mView, mProj, mViewProj;
			//nsMath::CVector3 eyePos = { 0.0f,10.0f,0.0f };
			//nsMath::CVector3 targetPos = nsMath::CVector3::Front();
			//nsMath::CQuaternion rot;
			//static float angle = 0.0f;
			//angle += 0.001f;
			//if (angle > 360.0f)
			//{
			//	angle = 0.0f;
			//}
			//rot.SetRotationYDeg(angle);
			//rot.Apply(targetPos);
			//targetPos.Normalize();
			//targetPos.Scale(10.0f);
			//targetPos.y = eyePos.y;
			//mView.MakeViewMatrix(eyePos, targetPos, nsMath::CVector3::Up());
			//mProj.MakeProjectionMatrix(nsMath::DegToRad(60.0f), 1280.0f / 720.0f, 0.1f, 10000.0f);
			//mViewProj = mView* mProj;

			nsMath::CVector4 vMax(-FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX);
			nsMath::CVector4 vMin(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);

			m_isInViewFrustum = false;

			for (const auto& vertex : vertexPos)
			{
				// ビュープロジェクション座標系に変換
				nsMath::CVector4 v(vertex.x, vertex.y, vertex.z, 1.0f);
				mViewProj.Apply(v);

				// wで割って正規化
				v.x /= fabsf(v.w);
				v.y /= fabsf(v.w);
				v.z /= fabsf(v.w);

				vMax.Max(v);
				vMin.Min(v);
			}

			if (vMax.x > -1.0f &&
				vMax.y > -1.0 && 
				vMax.z > 0.0f && 
				vMin.x < 1.0f && 
				vMin.y < 1.0f && 
				vMin.z < 1.0f)
			{
				m_isInViewFrustum = true;
			}

			return;
		}


		void CGeometryData::DrawDebugLine(const nsMath::CVector3* vertexPos)
		{
			constexpr int kNumLines = 12;
			static const nsMath::CVector4 color = nsMath::CVector4::Green();
			constexpr unsigned int X0Y0Z0 =
				static_cast<unsigned int>(CAABB::EnVertexPos::enX0Y0Z0);	// 左下手前
			constexpr unsigned int X0Y0Z1 =
				static_cast<unsigned int>(CAABB::EnVertexPos::enX0Y0Z1);	// 左下奥
			constexpr unsigned int X0Y1Z0 =
				static_cast<unsigned int>(CAABB::EnVertexPos::enX0Y1Z0);	// 左上手前
			constexpr unsigned int X0Y1Z1 =
				static_cast<unsigned int>(CAABB::EnVertexPos::enX0Y1Z1);	// 左上奥
			constexpr unsigned int X1Y0Z0 =
				static_cast<unsigned int>(CAABB::EnVertexPos::enX1Y0Z0);	// 右下手前
			constexpr unsigned int X1Y0Z1 =
				static_cast<unsigned int>(CAABB::EnVertexPos::enX1Y0Z1);	// 右下奥
			constexpr unsigned int X1Y1Z0 =
				static_cast<unsigned int>(CAABB::EnVertexPos::enX1Y1Z0);	// 右上手前
			constexpr unsigned int X1Y1Z1 =
				static_cast<unsigned int>(CAABB::EnVertexPos::enX1Y1Z1);	// 右上奥
			nsMath::CVector3 origin[kNumLines]
			{
				vertexPos[X0Y0Z0],	// 0
				vertexPos[X1Y0Z0],	// 1
				vertexPos[X1Y0Z1],	// 2
				vertexPos[X0Y0Z1],	// 3
				vertexPos[X0Y0Z0],	// 4
				vertexPos[X1Y0Z0],	// 5
				vertexPos[X1Y0Z1],	// 6
				vertexPos[X0Y0Z1],	// 7
				vertexPos[X0Y1Z0],	// 8
				vertexPos[X1Y1Z0],	// 9
				vertexPos[X1Y1Z1],	// 10
				vertexPos[X0Y1Z1]	// 11
			};
			nsMath::CVector3 toPos[kNumLines] =
			{
				vertexPos[X0Y1Z0],
				vertexPos[X1Y1Z0],
				vertexPos[X1Y1Z1],
				vertexPos[X0Y1Z1],
				vertexPos[X1Y0Z0],
				vertexPos[X1Y0Z1],
				vertexPos[X0Y0Z1],
				vertexPos[X0Y0Z0],
				vertexPos[X1Y1Z0],
				vertexPos[X1Y1Z1],
				vertexPos[X0Y1Z1],
				vertexPos[X0Y1Z0]
			};


			for (int i = 0; i < kNumLines; i++)
			{
				nsPhysics::CPhysicsEngine::GetInstance()->PushDebugLine(
					nsPhysics::CPhysicsEngine::SMyDebugLine(
						origin[i],
						{ color.r, color.g, color.b },
						toPos[i],
						{ color.r, color.g, color.b }
				));
			}

			return;
		}


	}
}