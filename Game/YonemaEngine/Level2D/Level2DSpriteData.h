#pragma once

namespace nsYMEngine
{
	namespace nsLevel2D
	{
		struct SLevel2DSpriteData : private nsUtils::SNoncopyable
		{
		public:
			std::string Path = "";						//�t�@�C���p�X
			std::string Name = "";						//���O
			nsMath::CVector2 SpriteSize;				//�T�C�Y
			nsMath::CVector2 Position;					//���W
			float RotateDeg = 0.0f;						//��]�p�x
			nsMath::CVector2 Scale;						//�g�嗦
			nsMath::CVector4 MulColor;					//��Z�J���[
			nsMath::CVector2 Pivot;						//�s�{�b�g
			int Priority = 0;							//�D��x
			int animNum = 0;							//�A�j���[�V������
			std::vector<std::string> animPathVector;	//�A�j���[�V�����p�X�̔z��
		};
	}
}