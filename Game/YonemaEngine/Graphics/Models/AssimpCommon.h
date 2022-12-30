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
			//////// Structs ////////

			struct SLocalTransform
			{
				constexpr SLocalTransform() = default;
				~SLocalTransform() = default;

				aiVector3D scaling = {};
				aiQuaternion rotation = {};
				aiVector3D translation = {};
			};

			
			//////// Constants ////////

			static constexpr unsigned int g_kBasicRemoveComponentFlags = 
				aiComponent_LIGHTS |
				aiComponent_CAMERAS |
				aiComponent_ANIMATIONS;

			static constexpr unsigned int g_kBasicPostprocessFlags = 
				aiProcess_RemoveComponent			|	// �R���|�[�l���g�̈ꕔ����������
				aiProcess_CalcTangentSpace			|	// �ڐ��Ə]�@�����v�Z����
				aiProcess_JoinIdenticalVertices		|	// ���b�V�����̓��꒸�_�̌���
				aiProcess_MakeLeftHanded			|	// ������W�n�ɕϊ��BDirectX�̏ꍇ�K�{�B
				aiProcess_Triangulate				|	// �S�Ẵ|���S�����O�p�`�|���S���ɕϊ�
				aiProcess_GenSmoothNormals			|	// �S�Ă̒��_�ɂȂ߂炩�Ȗ@���𐶐�
				aiProcess_LimitBoneWeights			|	// 1�̒��_�ɉe����^����{�[���̍ő吔�𐧌�
				aiProcess_ImproveCacheLocality		|	// ���_�L���b�V���̋Ǐ��������߂邽�߂ɎO�p�`�̏�����ύX
				aiProcess_RemoveRedundantMaterials	|	// �璷�ȃ}�e���A���̏���
				aiProcess_FindInvalidData			|	// �����ȃf�[�^��T���A�����E�C��
				aiProcess_GenUVCoords				|	// UV�ȊO�̃}�b�s���O��UV�֕ϊ�
				aiProcess_OptimizeMeshes			|	// ���b�V�����œK�����Đ������炷
				aiProcess_FlipUVs					|	// UV���W��Y���W�𔽓]����BDirectX�̏ꍇ�K�{�B
				aiProcess_FlipWindingOrder;				// CCW��CW�ɂ���B�w�ʂ��E���ŃJ�����O����BDirectX�̏ꍇ�K�{�B

			static constexpr unsigned int g_kAnimationRemoveFlags =
				aiComponent_NORMALS |
				aiComponent_TANGENTS_AND_BITANGENTS |
				aiComponent_COLORS |
				aiComponent_TEXCOORDS |
				aiComponent_BONEWEIGHTS |
				aiComponent_TEXTURES |
				aiComponent_LIGHTS |
				aiComponent_CAMERAS |
				aiComponent_MESHES |
				aiComponent_MATERIALS;

			static constexpr unsigned int g_kAnimationPostprocessFlags =
				aiProcess_RemoveComponent |
				aiProcess_MakeLeftHanded;

			static constexpr unsigned int g_kNavMeshRemoveComponentFlags =
				aiComponent_TANGENTS_AND_BITANGENTS |
				aiComponent_COLORS |
				aiComponent_TEXCOORDS |
				aiComponent_BONEWEIGHTS |
				aiComponent_ANIMATIONS |
				aiComponent_TEXTURES |
				aiComponent_LIGHTS |
				aiComponent_CAMERAS |
				aiComponent_MATERIALS;

			static constexpr unsigned int g_kNavMeshPostprocessFlags =
				aiProcess_JoinIdenticalVertices |
				aiProcess_MakeLeftHanded |
				aiProcess_Triangulate |
				aiProcess_RemoveComponent |
				aiProcess_GenNormals;


			//////// Functions ////////

			bool ImportScene(
				const char* modelFilePath,
				Assimp::Importer*& pImporter,
				const aiScene*& pScene,
				unsigned int removeFlags = 0,
				unsigned int postprocessFlag = 0
			);

			static inline void AiMatrixToMyMatrix(
				const aiMatrix4x4& srcAiMatrix, nsMath::CMatrix* dstMyMatrix)
			{
				memcpy_s(dstMyMatrix, sizeof(*dstMyMatrix), &srcAiMatrix, sizeof(srcAiMatrix));
				// DirectX�n�̍s��ɒ������߂ɓ]�u����B
				dstMyMatrix->Transpose();
				return;
			}


		}
	}
}