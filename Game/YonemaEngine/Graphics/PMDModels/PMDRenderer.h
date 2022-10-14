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
				 * @brief PMD�t�@�C���̃w�b�_���\����
				*/
				struct SPMDHeader
				{
					// �擪3�o�C�g�ɃV�O�l�`���Ƃ���"pmd"�Ƃ��������񂪓����Ă��邪�A
					// ������\���̂Ɋ܂߂�fread()���Ă��܂��ƁA�A���C�����g�ɂ��Y����
					// �������Ėʓ|�Ȃ��߁A�\���̂ɂ͊܂߂Ȃ��B
					float version;
					char model_name[20];
					char comment[256];
				};


#pragma pack(1)
				/**
				 * @brief PMD�t�@�C���̃}�e���A���f�[�^
				 * @details 1�o�C�g�p�b�L���O�ɂ��āA�A���C�����g�ɂ��T�C�Y�̂����h���B
				 * ���������͗����邪�A�ǂݍ��ނ͍̂ŏ������ł���A
				 * ����Ɍ�ŕʂ̍\���̂ɃR�s�[���邩��A���܂���͂Ȃ��Ǝv���B
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
					// ���ӁF�A���C�����g�ɂ��A������2�o�C�g�̃p�f�B���O������B
					unsigned int indicesNum = 0;
					char texFilePath[20];
				};
#pragma pack()


				/**
				 * @brief �V�F�[�_�[���ɓn���t�@�C���̃}�e���A���f�[�^
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
				 * @brief ����ȊO�̃}�e���A���f�[�^
				*/
				struct SAdditionalMaterial
				{
					std::string texPath;
					int toonIdx = 0;
					bool edgeFlg = false;
				};

				/**
				 * @brief �S�̂��܂Ƃ߂�PMD�}�e���A���f�[�^
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
				 * @brief PMD���f���̓ǂݍ��ݗp�{�[���f�[�^
				 * @details 1�o�C�g�p�b�L���O�ɂ��āA�A���C�����g�ɂ��T�C�Y�̂����h���B
				*/
				struct SPMDBone
				{
					char boneName[20];
					unsigned short parentNo;
					unsigned short nextNo;
					unsigned char type;
					// �����ŃA���C�����g���������Ă��܂��B
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
				// XMMATRIX��16�o�C�g�A���C�����g����������邽�߁Anew����Ɗ댯�B
				// ���̂��߁AXMFLOAT4X4���g�p����B
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