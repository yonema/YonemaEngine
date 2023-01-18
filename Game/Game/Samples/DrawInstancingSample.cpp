#include "DrawInstancingSample.h"
#include "../../YonemaEngine/DebugSystem/SimpleMover.h"

namespace nsAWA
{
	namespace nsSamples
	{
		const char* const CDrawInstancingSample::m_kModelFilePath = "Assets/Models/Samples/TreeTest.fbx";
		const unsigned int CDrawInstancingSample::m_kNumInstanceX = 100;
		const unsigned int CDrawInstancingSample::m_kNumInstanceZ = 100;
		const float CDrawInstancingSample::m_kDistance = 5.0f;

		bool CDrawInstancingSample::Start()
		{
			// CameraSetting
			MainCamera()->SetFarClip(10000.0f);

			// CreateModel
			m_modelRenderer = NewGO<CModelRenderer>();
			SModelInitData modelInitData;
			modelInitData.modelFilePath = m_kModelFilePath;
			modelInitData.maxInstance = m_kNumInstanceX * m_kNumInstanceZ;
			m_modelRenderer->Init(modelInitData);

			auto* worldMatrixArray = m_modelRenderer->GetWorldMatrixArrayRef();

			nsMath::CMatrix mRot;
			mRot.MakeRotationAxis(nsMath::CVector3::AxisX(), nsMath::YM_PIDIV2);
			unsigned int instanceIdx = 0;
			for (unsigned int z = 0; z < m_kNumInstanceZ; z++)
			{
				for (unsigned int x = 0; x < m_kNumInstanceX; x++)
				{
					nsMath::CMatrix mWorld, mTrans;
					mTrans.MakeTranslation({ m_kDistance * x, 0.0f, m_kDistance * z });
					mWorld = mRot * mTrans;
					(*worldMatrixArray)[instanceIdx] = mWorld;
					instanceIdx++;
				}
			}

			m_modelRenderer->UpdateWorldMatrixArray();


			// CreateSimpleMover
			m_simpleMover = NewGO<nsDebugSystem::CSimpleMover>();

			return true;
		}

		void CDrawInstancingSample::OnDestroy()
		{
			DeleteGO(m_simpleMover);

			DeleteGO(m_modelRenderer);

			return;
		}

		void CDrawInstancingSample::Update(float deltaTime)
		{
			m_simpleMover->MoveCameraBySimpleMover();

			return;
		}
	}
}