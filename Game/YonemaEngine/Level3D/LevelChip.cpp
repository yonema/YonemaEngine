#include "LevelChip.h"
#include "Level3D.h"
#include "../Graphics/Renderers//ModelRenderer.h"

namespace nsYMEngine
{
	namespace nsLevel3D
	{
		CLevelChip::CLevelChip(
			const SLevelChipData& chipData,
			const SLevel3DinitData& levelInitData,
			const char* filePath) noexcept
		{
			nsGraphics::nsRenderers::SModelInitData initData;
			initData.modelFilePath = filePath;
			initData.vertexBias = levelInitData.levelChipBias;

			nsPhysics::SMeshGeometryBuffer physicsMeshBuffer = {};
			if (chipData.isCreateStaticPhysicsObject && 
				levelInitData.isCreateStaticPhysicsObjectForAll)
			{
				initData.physicsMeshGeomBuffer = &physicsMeshBuffer;
			}

			//モデルを読み込む。
			m_modelRenderer = NewGO<nsGraphics::nsRenderers::CModelRenderer>(chipData.name);
			m_modelRenderer->SetPosition(chipData.position);
			m_modelRenderer->SetRotation(chipData.rotation);
			m_modelRenderer->SetScale(chipData.scale);
			m_modelRenderer->Init(initData);

			if (chipData.isCreateStaticPhysicsObject && 
				levelInitData.isCreateStaticPhysicsObjectForAll)
			{
				for (auto& vertex : physicsMeshBuffer.m_vertices)
				{
					vertex.x *= chipData.scale.x;
					vertex.y *= chipData.scale.y;
					vertex.z *= chipData.scale.z;
				}
			}

			if (chipData.isCreateStaticPhysicsObject &&
				levelInitData.isCreateStaticPhysicsObjectForAll)
			{
				//静的物理オブジェクトを作成。
				m_physicsStaticObject.InitAsMesh(physicsMeshBuffer, chipData.position);
				m_physicsStaticObject.SetRotation(chipData.rotation);
			}

			
			return;
		}

		CLevelChip::~CLevelChip()
		{
			m_physicsStaticObject.Release();
			DeleteGO(m_modelRenderer);

			return;
		}






	}
}
