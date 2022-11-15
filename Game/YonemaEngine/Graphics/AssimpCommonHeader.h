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
				// DirectXŒn‚Ìs—ñ‚É’¼‚·‚½‚ß‚É“]’u‚·‚éB
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