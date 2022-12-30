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
				aiProcess_RemoveComponent			|	// コンポーネントの一部を消去する
				aiProcess_CalcTangentSpace			|	// 接線と従法線を計算する
				aiProcess_JoinIdenticalVertices		|	// メッシュ内の同一頂点の結合
				aiProcess_MakeLeftHanded			|	// 左手座標系に変換。DirectXの場合必須。
				aiProcess_Triangulate				|	// 全てのポリゴンを三角形ポリゴンに変換
				aiProcess_GenSmoothNormals			|	// 全ての頂点になめらかな法線を生成
				aiProcess_LimitBoneWeights			|	// 1つの頂点に影響を与えるボーンの最大数を制限
				aiProcess_ImproveCacheLocality		|	// 頂点キャッシュの局所性を高めるために三角形の順序を変更
				aiProcess_RemoveRedundantMaterials	|	// 冗長なマテリアルの消去
				aiProcess_FindInvalidData			|	// 無効なデータを探し、消去・修正
				aiProcess_GenUVCoords				|	// UV以外のマッピングをUVへ変換
				aiProcess_OptimizeMeshes			|	// メッシュを最適化して数を減らす
				aiProcess_FlipUVs					|	// UV座標のY座標を反転する。DirectXの場合必須。
				aiProcess_FlipWindingOrder;				// CCWをCWにする。背面を右回りでカリングする。DirectXの場合必須。

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
				// DirectX系の行列に直すために転置する。
				dstMyMatrix->Transpose();
				return;
			}


		}
	}
}