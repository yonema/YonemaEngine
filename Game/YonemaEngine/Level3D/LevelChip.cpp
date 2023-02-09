#include "LevelChip.h"
#include "Level3D.h"
#include "../Graphics/Renderers//ModelRenderer.h"

namespace nsYMEngine
{
	namespace nsLevel3D
	{
		void CLevelChip::Init(const char* filePath) noexcept
		{
			const auto count = m_levelChipList.size();
			if (count == 0)
			{
				return;
			}

			const bool isInstancing = count > 1;

			const auto& firstChip = m_levelChipList.begin();
			const auto& firstInitData = m_initDataList.begin();

			nsGraphics::nsRenderers::SModelInitData initData;
			initData.modelFilePath = filePath;
			initData.vertexBias = firstInitData->levelChipBias;
			initData.maxInstance = static_cast<unsigned int>(count);

			nsPhysics::SMeshGeometryBuffer physicsMeshBuffer = {};
			if (firstChip->isCreateStaticPhysicsObject &&
				firstInitData->isCreateStaticPhysicsObjectForAll)
			{
				initData.physicsMeshGeomBuffer = &physicsMeshBuffer;
			}

			//モデルを読み込む。
			m_modelRenderer = NewGO<nsGraphics::nsRenderers::CModelRenderer>(firstChip->name.c_str());

			if (isInstancing != true)
			{
				// 通常描画
				m_modelRenderer->SetPosition(firstChip->position);
				m_modelRenderer->SetRotation(firstChip->rotation);
				m_modelRenderer->SetScale(firstChip->scale);
			}

			m_modelRenderer->Init(initData);

			if (isInstancing)
			{
				auto* worldMatrixArray = m_modelRenderer->GetWorldMatrixArrayRef();
				auto chipItr = m_levelChipList.begin();
				for (unsigned int instanceIdx = 0; instanceIdx < initData.maxInstance; instanceIdx++)
				{
					nsMath::CMatrix mTrans, mRot, mScale;
					mTrans.MakeTranslation(chipItr->position);
					mRot.MakeRotationFromQuaternion(chipItr->rotation);
					mScale.MakeScaling(chipItr->scale);

					worldMatrixArray->at(instanceIdx) = mScale * mRot * mTrans;

					chipItr++;
				}

				m_modelRenderer->UpdateWorldMatrixArray();
			}




			if (firstChip->isCreateStaticPhysicsObject && 
				firstInitData->isCreateStaticPhysicsObjectForAll)
			{
				if (isInstancing != true)
				{
					for (auto& vertex : physicsMeshBuffer.m_vertices)
					{
						vertex.x *= firstChip->scale.x;
						vertex.y *= firstChip->scale.y;
						vertex.z *= firstChip->scale.z;
					}

					//静的物理オブジェクトを作成。
					m_physicsStaticObjectList.emplace_back();
					auto itr = m_physicsStaticObjectList.end();
					itr--;
					itr->InitAsMesh(physicsMeshBuffer, firstChip->position);
					itr->SetRotation(firstChip->rotation);
				}
				else
				{
					auto chipItr = m_levelChipList.begin();
					for (unsigned int instanceIdx = 0; instanceIdx < initData.maxInstance; instanceIdx++)
					{
						for (auto& vertex : physicsMeshBuffer.m_vertices)
						{
							vertex.x *= chipItr->scale.x;
							vertex.y *= chipItr->scale.y;
							vertex.z *= chipItr->scale.z;
						}

						//静的物理オブジェクトを作成。
						m_physicsStaticObjectList.emplace_back();
						auto itr = m_physicsStaticObjectList.end();
						itr--;
						itr->InitAsMesh(physicsMeshBuffer, chipItr->position);
						itr->SetRotation(chipItr->rotation);

						chipItr++;
					}
				}

			}

			m_levelChipList.clear();
			m_initDataList.clear();
			return;
		}

		CLevelChip::~CLevelChip()
		{
			for (auto& physicsObject : m_physicsStaticObjectList)
			{
				physicsObject.Release();
			}
			m_physicsStaticObjectList.clear();

			DeleteGO(m_modelRenderer);

			return;
		}






	}
}
