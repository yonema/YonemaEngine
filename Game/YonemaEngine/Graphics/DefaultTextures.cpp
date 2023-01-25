#include "DefaultTextures.h"
#include "../Memory/ResourceBankTable.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		const char* const CDefaultTextures::m_kTextureFilePath[m_kNumTextures] =
		{
			"Assets/Images/Presets/white.jpg",
			"Assets/Images/Presets/black.jpg",
			"Assets/Models/Presets/defaultDiffuseMap.jpg",
			"Assets/Models/Presets/defaultNormalMap.jpg"
		};

		CDefaultTextures::~CDefaultTextures()
		{
			Terminate();

			return;
		}

		void CDefaultTextures::Init()
		{
			auto& textureBank = nsMemory::CResourceBankTable::GetInstance()->GetTextureBank();

			for (unsigned int texIdx = 0; texIdx < m_kNumTextures; texIdx++)
			{
				m_textures[texIdx] = new nsDx12Wrappers::CTexture();
				m_textures[texIdx]->Init(m_kTextureFilePath[texIdx]);

				textureBank.Register(m_kTextureFilePath[texIdx], m_textures[texIdx]);

				m_textures[texIdx]->SetShared(true);
			}

			return;
		}

		void CDefaultTextures::Terminate()
		{
			Release();

			return;
		}

		void CDefaultTextures::Release()
		{
			for (unsigned int texIdx = 0; texIdx < m_kNumTextures; texIdx++)
			{
				if (m_textures[texIdx])
				{
					if (m_textures[texIdx]->IsShared() != true)
					{
						delete m_textures[texIdx];
					}
					m_textures[texIdx] = nullptr;
				}
			}

			return;
		}




	}
}