#pragma once
namespace nsYMEngine
{
	namespace nsGraphics
	{
		class CTexture;
	}
}

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsPMDModels
		{

			class CPMDRenderer : private nsUtils::SNoncopyable
			{
			private:
				static const size_t m_kPmdVertexSize;
				static const int m_kNumMaterialDescriptors;

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




			public:
				CPMDRenderer(const char* filePath);
				~CPMDRenderer();

				void Update();

				void Draw();

				inline void DebugSetPosition(float x)
				{
					m_debugPosX = x;
				}

			private:
				void Init(const char* filePath);
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

			private:
				std::vector<SMaterial> m_materials;
				std::vector<CTexture*> m_textures;
				std::vector<CTexture*> m_sphTextures;
				std::vector<CTexture*> m_spaTextures;
				// XMMATRIXは16バイトアライメントが強制されるため、newすると危険。
				// そのため、XMFLOAT4X4を使用する。
				nsMath::CMatrix m_mWorld;
				ID3D12Resource* m_vertexBuff = nullptr;
				D3D12_VERTEX_BUFFER_VIEW m_vertexBuffView = {};
				ID3D12Resource* m_indexBuff = nullptr;
				D3D12_INDEX_BUFFER_VIEW m_indexBuffView = {};
				ID3D12Resource* m_constantBuff = nullptr;
				ID3D12DescriptorHeap* m_cbDescriptorHeap = nullptr;
				SConstantBuff* m_mappedConstantBuff = nullptr;
				ID3D12Resource* m_materialBuff = nullptr;
				ID3D12DescriptorHeap* m_mbDescriptorHeap = nullptr;

				float m_debugPosX = 0.0f;
				float m_debugRotY= 0.0f;

			};
		}
	}
}