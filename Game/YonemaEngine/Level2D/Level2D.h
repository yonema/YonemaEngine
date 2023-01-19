#pragma once
#include "Level2DSpriteData.h"

namespace nsYMEngine
{
	namespace nsLevel2D
	{
		class CLevel2D : private nsUtils::SNoncopyable
		{
		public:
			/**
			 * @brief 2D���x���f�[�^�����[�h����
			 * @param path 2D���x���f�[�^�̃p�X
			 * @param hookFunc hook�֐�
			*/
			void Load(const char* path, std::function<bool(const SLevel2DSpriteData&)> hookFunc = nullptr);

			~CLevel2D()
			{
				for (nsGraphics::nsRenderers::CSpriteRenderer* sprite : m_spritesList)
				{
					DeleteGO(sprite);
				}
			}
		private:
			/**
			 * @brief �c�[���Ő������ꂽ�X�v���C�g�f�[�^���G���W���̍��W�n�ɕϊ�����
			 * @param spriteData �c�[���Ő������ꂽ�X�v���C�g�̃f�[�^
			*/
			void ConvertToEngineCoordinate(SLevel2DSpriteData& spriteData);

			/**
			 * @brief �X�v���C�g�̃f�[�^�����ɃX�v���C�g���쐬����
			 * @param spriteData 
			 * @return �쐬�����X�v���C�g�����_���[
			*/
			nsGraphics::nsRenderers::CSpriteRenderer* CreateNewSpriteFromData(const SLevel2DSpriteData& spriteData);

			//�X�v���C�g�̃��X�g
			std::list<nsGraphics::nsRenderers::CSpriteRenderer*> m_spritesList;
		};
	}
}

