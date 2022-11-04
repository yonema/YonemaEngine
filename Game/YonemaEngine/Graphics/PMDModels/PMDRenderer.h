#pragma once
#include "../Renderers/ModelRendererBase.h"
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDx12Wrappers
		{
			class CTexture;
		}
	}
}

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsPMDModels
		{

			class CPMDRenderer : public nsRenderers::IModelRendererBase
			{
			private:
				static const size_t m_kPmdVertexSize;
				static const size_t m_kAlignedPmdVertexSize;
				static const int m_kNumMaterialDescriptors;
				static const int m_kNumCalculationsOnBezier;

				/**
				 * @brief PMDファイルのヘッダ情報構造体
				*/
				struct SPMDHeader
				{
					// 先頭3バイトにシグネチャとして"pmd"という文字列が入っているが、
					// これを構造体に含めてfread()してしまうと、アライメントによるズレが
					// 発生して面倒なため、構造体には含めない。
					float version;
					char model_name[20];
					char comment[256];
				};


#pragma pack(1)
				/**
				 * @brief PMDファイルのマテリアルデータ
				 * @details 1バイトパッキングにして、アライメントによるサイズのずれを防ぐ。
				 * 処理効率は落ちるが、読み込むのは最初だけであり、
				 * さらに後で別の構造体にコピーするから、あまり問題はないと思う。
				*/
				struct SPMDMaterial
				{
					nsMath::CVector3 diffuse;
					float alpha = 0.0f;
					float specularity = 0.0f;
					nsMath::CVector3 specular;
					nsMath::CVector3 ambient;
					unsigned char toonIdx;
					unsigned char edgeFlg;
					// 注意：アライメントにより、ここに2バイトのパディングが入る。
					unsigned int indicesNum = 0;
					char texFilePath[20];
				};
#pragma pack()


				/**
				 * @brief シェーダー側に渡すファイルのマテリアルデータ
				*/
				struct SMaterialForHlsl
				{
					nsMath::CVector3 diffuse;
					float alpha = 0.0f;
					nsMath::CVector3 specular;
					float specularity = 0.0f;
					nsMath::CVector3 ambient;
				};

				/**
				 * @brief それ以外のマテリアルデータ
				*/
				struct SAdditionalMaterial
				{
					std::string texPath;
					int toonIdx = 0;
					bool edgeFlg = false;
				};

				/**
				 * @brief 全体をまとめるPMDマテリアルデータ
				*/
				struct SMaterial
				{
					unsigned int indicesNum = 0;
					SMaterialForHlsl matForHlsl;
					SAdditionalMaterial additionalMat;
				};

				struct SConstantBuff
				{
					nsMath::CMatrix mWorld;
					nsMath::CMatrix mWorldViewProj;
					//std::vector<nsMath::CMatrix> mBones;
				};

#pragma pack(1)
				/**
				 * @brief PMDモデルの読み込み用ボーンデータ
				 * @details 1バイトパッキングにして、アライメントによるサイズのずれを防ぐ。
				*/
				struct SPMDBone
				{
					char boneName[20];
					unsigned short parentNo;
					unsigned short nextNo;
					unsigned char type;
					// ここでアライメントが発生してしまう。
					unsigned short ikBoneNo;
					nsMath::CVector3 pos;
				};
#pragma pack()

				struct SBoneNode
				{
					int boneIdx;
					nsMath::CVector3 startPos;
					nsMath::CVector3 enePos;
					std::vector<SBoneNode*> children;
				};

				struct SVMDMotion
				{
					char boneName[15];
					unsigned int frameNo;
					nsMath::CVector3 location;
					nsMath::CQuaternion rotation;
					unsigned char bezier[64];	// [4][4][4]のベジェ補完パラメータ
				};

				struct SKeyFrame
				{
					unsigned int frameNo;
					nsMath::CQuaternion rotation;
					nsMath::CVector2 p1;	// ベジェ曲線のコントロールポイント1
					nsMath::CVector2 p2;	// ベジェ曲線のコントロールポイント2
					SKeyFrame(unsigned int fNo, const nsMath::CQuaternion& rot,
						const nsMath::CVector2& ip1, const nsMath::CVector2& ip2)
						:frameNo(fNo), rotation(rot), p1(ip1), p2(ip2) {};
				};


			protected:
				void Draw(nsDx12Wrappers::CCommandList* commandList) override final;

			public:
				void UpdateWorldMatrix(
					const nsMath::CVector3& position,
					const nsMath::CQuaternion& rotation,
					const nsMath::CVector3& scale
				) override final;

				void UpdateAnimation(float deltaTime) override final;

				void PlayAnimation() override final;

			public:
				CPMDRenderer(const char* filePath, const char* animFilePath = nullptr);
				~CPMDRenderer();


				inline void DebugSetPosition(float x) noexcept
				{
					m_debugPosX = x;
				}



			private:
				void Init(const char* filePath, const char* animFilePath = nullptr);
				void Terminate();

				void LoadPMDModel(const char* filePath);

				void CreateBuff(
					std::vector<unsigned char>& pmdVertices,
					std::vector<unsigned short>& pmdIndices,
					size_t pmdIndicesSize,
					unsigned int pmdMaterialNum
				);

				void CreateVertexBuff(
					ID3D12Device5* const device,
					const D3D12_HEAP_PROPERTIES& heapProp,
					const D3D12_RESOURCE_DESC& resDesc,
					std::vector<unsigned char>& pmdVertices
					);

				void CreateIndexBuff(
					ID3D12Device5* const device,
					const D3D12_HEAP_PROPERTIES& heapProp,
					D3D12_RESOURCE_DESC& resDesc,
					std::vector<unsigned short>& pmdIndices,
					size_t pmdIndicesSize
				);

				void CreateConstantBuff(ID3D12Device5* const device);
				
				void CreateMaterialBuff(
					ID3D12Device5* const device,
					const D3D12_HEAP_PROPERTIES& heapProp,
					D3D12_RESOURCE_DESC& resDesc,
					unsigned int pmdMaterialNum
				);

				void RecursiveMatrixMultiply(SBoneNode* node, const nsMath::CMatrix& mat);

				void LoadVMDAnimation(const char* filePath);



				float GetYFromXOnBezier(
					float x, const nsMath::CVector2& a, const nsMath::CVector2& b, uint8_t n) noexcept;


			private:
				std::vector<SMaterial> m_materials;
				std::vector<nsDx12Wrappers::CTexture*> m_textures;
				std::vector<nsDx12Wrappers::CTexture*> m_sphTextures;
				std::vector<nsDx12Wrappers::CTexture*> m_spaTextures;
				nsMath::CMatrix m_mWorld;
				nsDx12Wrappers::CIndexBuffer m_indexBuffer;
				nsDx12Wrappers::CVertexBuffer m_vertexBuffer;
				nsDx12Wrappers::CConstantBuffer m_modelCB;
				nsDx12Wrappers::CDescriptorHeap m_modelDH;
				nsDx12Wrappers::CConstantBuffer m_materialCB;
				nsDx12Wrappers::CDescriptorHeap m_materialDH;

				std::vector<nsMath::CMatrix> m_boneMatrices;
				std::unordered_map<std::string, SBoneNode> m_boneNodeTable;

				std::unordered_map<std::string, std::vector<SKeyFrame>> m_motionData;
				float m_playAnimTime = 0.0f;
				unsigned int m_maxFrameNo = 0;

				float m_debugPosX = 0.0f;
				float m_debugRotY= 0.0f;

			};
		}
	}
}