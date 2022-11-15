#pragma once
#include "../Renderers/ModelRendererBase.h"
#include <fbxsdk.h>

namespace fbxsdk
{
	class FbxNode;
	class FbxMesh;
	class FbxSurfaceMaterial;
}
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsRenderers
		{
			struct SModelInitData;
		}
	}
}

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsFBXModels
		{

			class CFBXAnimTimeInfo : nsUtils::SNoncopyable
			{
			public:
				constexpr CFBXAnimTimeInfo() = default;
				~CFBXAnimTimeInfo() = default;

				void Init(fbxsdk::FbxScene* fbxScene);

				inline int GetMaxFrame() const noexcept
				{
					return static_cast<int>(m_totalTime.Get());
				}

				fbxsdk::FbxTime GetTime(int frame) const noexcept;

			private:
				fbxsdk::FbxTime m_startTime = {};
				fbxsdk::FbxTime m_stopTime = {};
				fbxsdk::FbxTime m_oneFrameTime = {};
				fbxsdk::FbxTime m_totalTime = {};

			};

			class CFBXRendererFBX_SDK : public nsRenderers::IModelRendererBase
			{
			private:

				struct SFbxVertex
				{
					nsMath::CVector3 position;
					nsMath::CVector3 normal;
					nsMath::CVector4 color;
					nsMath::CVector2 uv;
					unsigned short boneNo[4];
					unsigned short weights[4];
				};

				struct SFbxMaterial
				{
					nsMath::CVector4 ambient;
					nsMath::CVector4 diffuse;
					nsMath::CVector4 specular;
					float alpha = 1.0f;
				};

				struct SBoneInfo 
				{
					std::string name;			//! �{�[����
					unsigned int frameNum;		//! �t���[����
					nsMath::CMatrix initMatrix;		//! �����p��
					nsMath::CMatrix globalBindposeInvMatrix;
					std::vector<nsMath::CMatrix> frameMatrixArray;		//! �t���[�����p��
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

			public:
				CFBXRendererFBX_SDK(const SModelInitData& modelInitData);
				~CFBXRendererFBX_SDK();

				void Release();

			private:
				bool Init(const SModelInitData& modelInitData);

				/**
				 * @brief FbxSdk���g�p���鏀�������܂��B
				 * @param[out] pManager FbxManager�i�[�p�|�C���^
				 * @param[out] pScene FbxScene�i�[�p�|�C���^
				 * @return �������ɐ����������H
				 * @retval true ����������
				 * @retval false ���������s
				*/
				bool InitializeSdkObjects(fbxsdk::FbxManager*& pManager, fbxsdk::FbxScene*& pScene);

				/**
				 * @brief FBX_SDK�ɂ���č쐬���ꂽ���ׂẴI�u�W�F�N�g��j�����܂��B
				 * @param[in] pManager �j������FbxManager
				 * @param[in] exitStatus FBX_SDK�ɂ�鏈���������������Htrue�Ő����\���B
				*/
				void DestroySdkObjects(fbxsdk::FbxManager* pManager, bool exitStatus);

				void Terminate();

				void LoadMaterial(
					fbxsdk::FbxSurfaceMaterial* material,
					std::unordered_map<std::string, SFbxMaterial>* pFbxMaterials,
					//nsDx12Wrappers::CTexture** ppTexture,
					std::unordered_map < std::string, nsDx12Wrappers::CTexture*>* pTextures,
					const char* filePath);

				void CreateMesh(
					unsigned int objectIdx,
					const fbxsdk::FbxMesh* mesh,
					std::vector<std::vector<SFbxVertex>>* pVerticesArray,
					std::vector<std::vector<unsigned short>>* pIndicesArray,
					const SModelInitData& modelInitData
				);

				bool CreateVertexAndIndexBuffer(
					const std::vector<std::vector<SFbxVertex>>& verticesArray,
					const std::vector<std::vector<unsigned short>>& indicesArray
				);

				bool CreateConstantBufferView();

				bool CreateMaterialCBVTable(
					const std::unordered_map<std::string, SFbxMaterial>& fbxMaterials);

				bool CreateShaderResourceView();


			private:
				std::vector<nsDx12Wrappers::CIndexBuffer*> m_indexBuffers;
				std::vector<nsDx12Wrappers::CVertexBuffer*> m_vertexBuffers;
				std::vector<unsigned int> m_numIndicesArray;
				std::vector<nsDx12Wrappers::CDescriptorHeap*> m_materialDHs;
				std::unordered_map <std::string, nsDx12Wrappers::CTexture*> m_diffuseTextures;

				nsDx12Wrappers::CConstantBuffer m_constantBuffer;
				nsDx12Wrappers::CDescriptorHeap m_descriptorHeap;
				std::unordered_map <std::string, nsDx12Wrappers::CConstantBuffer*> m_materialCBTable;
				std::unordered_map <std::string, nsDx12Wrappers::CDescriptorHeap*> m_materialDHTable;

				std::vector<std::vector<std::unordered_map<std::string, float>>> m_weightTableArray;
				std::unordered_map<std::string, unsigned short> m_boneNameTable;
				nsMath::CMatrix m_bias;
				CFBXAnimTimeInfo m_animTimeInfo;
				std::vector<std::vector<nsMath::CMatrix>> m_nodeMatrixByFrameArray;
				std::vector<SBoneInfo> m_boneInfoArray;
				std::vector<nsMath::CMatrix> m_boneMatrixArray;
			};

		}
	}
}