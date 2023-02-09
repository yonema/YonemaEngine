#include "GeometryData.h"
#include "../Graphics/Models/BasicModelRenderer.h"
#include "../Graphics/GraphicsEngine.h"
#include "../Physics/PhysicsEngine.h"

namespace nsYMEngine
{
	namespace nsGeometries
	{
#ifdef _DEBUG
		nsMath::CMatrix CGeometryData::m_mViewProjDebug = nsMath::CMatrix::Identity();
		bool CGeometryData::m_isDebugDraw = false;

#endif // _DEBUG

		void CGeometryData::Init(const std::vector<SMesh>& meshes) noexcept
		{
			m_aabb.InitFromMeshes(meshes);

			return;
		}

		void CGeometryData::Update(const nsMath::CMatrix& mWorld) noexcept
		{
			nsMath::CVector3 vertexPos[CAABB::m_kNumVertesPos];
			m_aabb.CalcVertexPositions(vertexPos, mWorld);

#ifdef _DEBUG
			//if (m_isDebugDraw)
			//{
			//	DrawDebugLine(vertexPos, nsMath::CVector4::Green());
			//}

#endif // _DEBUG


			const auto& mViewProj = nsGraphics::CGraphicsEngine::GetInstance()->GetMatrixViewProj();
			
			// デバック用
			//const auto& mViewProj = m_mViewProjDebug;

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

		void CGeometryData::DebugUpdate(float deltaTime)
		{

			m_isDebugDraw = true;
			nsPhysics::CPhysicsEngine::GetInstance()->EnableDebugDrawLine();
			nsPhysics::CPhysicsEngine::GetInstance()->EnableAutoFitCullingBoxToMainCamera();
			CalcDebugViewProjMat(deltaTime);

			return;
		}


		void CGeometryData::DrawDebugLine(
			const nsMath::CVector3* vertexPos, const nsMath::CVector4& color)
		{
#ifdef _DEBUG

			constexpr int kNumLines = 12;
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

#endif // _DEBUG
			return;
		}


		void CGeometryData::CalcDebugViewProjMat(float deltaTime)
		{
#ifdef _DEBUG
			nsMath::CMatrix mView, mProj, mViewProj;
			nsMath::CVector3 eyePos = { 0.0f,10.0f,-50.0f };
			nsMath::CVector3 toTargetVec = nsMath::CVector3::Front();
			nsMath::CQuaternion rot;
			static float angle = 0.0f;
			angle += 50.0f * deltaTime;
			if (angle > 360.0f)
			{
				angle = 0.0f;
			}
			rot.SetRotationYDeg(angle);
			rot.Apply(toTargetVec);
			toTargetVec.Normalize();
			auto toTargetPos = toTargetVec;
			toTargetPos.Scale(10.0f);
			nsMath::CVector3 targetPos = eyePos + toTargetPos;
			mView.MakeViewMatrix(eyePos, targetPos, nsMath::CVector3::Up());
			static constexpr float nearDepth = 1.0f;
			static constexpr float farDepth = 1000.0f;
			static constexpr float fovAngleY = nsMath::DegToRad(60.0f);
			static constexpr float aspectRatio = 1280.0f / 720.0f;
			mProj.MakeProjectionMatrix(fovAngleY, aspectRatio, nearDepth, farDepth);
			mViewProj = mView * mProj;

			nsMath::CVector3 right = nsMath::CVector3::Right();
			rot.Apply(right);
			right.Normalize();

			DrawCameraFrustum(
				eyePos,
				toTargetVec,
				nsMath::CVector3::Up(),
				right,
				nearDepth, 
				farDepth, 
				fovAngleY,
				aspectRatio
			);


			m_mViewProjDebug = mViewProj;

#endif // _DEBUG
			return;
		}


		void CGeometryData::DrawCameraFrustum(
			const nsMath::CVector3& cameraPos,
			const nsMath::CVector3& cameraForward,
			const nsMath::CVector3& cameraUp,
			const nsMath::CVector3& cameraRight,
			float nearDepth,
			float farDepth,
			float fovAngleY,
			float aspectRatio
		)
		{
			// エリアの近平面の中心からの上面、下面までの距離を求める
			float nearY = tanf(fovAngleY * 0.5f) * nearDepth;
			// エリアの近平面の中心からの右面、左面までの距離を求める
			float nearX = nearY * aspectRatio;
			// エリアの遠平面の中心からの上面、下面までの距離を求める
			float farY = tanf(fovAngleY * 0.5f) * farDepth;
			// エリアの遠平面の中心からの右面、左面までの距離を求める
			float farX = farY * aspectRatio;
			// エリアの近平面の中心座標を求める
			auto nearPos = cameraPos + cameraForward * nearDepth;
			// エリアの遠平面の中心座標を求める
			auto farPos = cameraPos + cameraForward * farDepth;

			nsMath::CVector3 vertex[CAABB::m_kNumVertesPos];

			// 近平面の左下の頂点
			vertex[0] += nearPos + cameraUp * -nearY + cameraRight * -nearX;
			// 遠平面の左下の頂点
			vertex[1] += farPos + cameraUp * -farY + cameraRight * -farX;
			// 近平面の左上の頂点
			vertex[2] += nearPos + cameraUp * nearY + cameraRight * -nearX;
			// 遠平面の左上の頂点
			vertex[3] += farPos + cameraUp * farY + cameraRight * -farX;
			// 近平面の右下の頂点
			vertex[4] += nearPos + cameraUp * -nearY + cameraRight * nearX;
			// 遠平面の右下の頂点
			vertex[5] += farPos + cameraUp * -farY + cameraRight * farX;
			// 近平面の右上の頂点
			vertex[6] += nearPos + cameraUp * nearY + cameraRight * nearX;
			// 遠平面の右上の頂点
			vertex[7] += farPos + cameraUp * farY + cameraRight * farX;

			DrawDebugLine(vertex, nsMath::CVector4::Blue());


			return;
		}



	}
}