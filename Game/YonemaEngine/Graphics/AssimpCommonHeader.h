#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsAssimpCommon
		{
			static inline void AiMatrixToMyMatrix(
				const aiMatrix4x4& srcAiMatrix, nsMath::CMatrix* dstMyMatrix)
			{
				memcpy_s(dstMyMatrix, sizeof(*dstMyMatrix), &srcAiMatrix, sizeof(srcAiMatrix));
				// DirectX�n�̍s��ɒ������߂ɓ]�u����B
				dstMyMatrix->Transpose();
				return;
			}

			struct SLocalTransform
			{
				aiVector3D scaling;
				aiQuaternion rotation;
				aiVector3D translation;
			};
		}
	}
}