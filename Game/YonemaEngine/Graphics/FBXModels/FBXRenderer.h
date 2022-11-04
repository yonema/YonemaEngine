#pragma once
#include "../Renderers/ModelRendererBase.h"
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
			class CFBXRenderer : public nsRenderers::IModelRendererBase
			{
			private:

				struct SFbxVertex
				{
					nsMath::CVector3 position;
					nsMath::CVector3 normal;
					nsMath::CVector4 color;
					nsMath::CVector2 uv;
				};

				struct SFbxMaterial
				{
					nsMath::CVector4 ambient;
					nsMath::CVector4 diffuse;
					nsMath::CVector4 specular;
					float alpha = 1.0f;
				};

			protected:
				void Draw(nsDx12Wrappers::CCommandList* commandList) override final;

			public:
				void UpdateWorldMatrix(
					const nsMath::CVector3& position,
					const nsMath::CQuaternion& rotation,
					const nsMath::CVector3& scale
				) override final;

			public:
				CFBXRenderer(const SModelInitData& modelInitData);
				~CFBXRenderer();

				void Release();

			private:
				bool Init(const SModelInitData& modelInitData);
				void Terminate();

				void LoadMaterial(
					fbxsdk::FbxSurfaceMaterial* material,
					std::unordered_map<std::string, SFbxMaterial>* pFbxMaterials,
					//nsDx12Wrappers::CTexture** ppTexture,
					std::unordered_map < std::string, nsDx12Wrappers::CTexture*>* pTextures,
					const char* filePath);

				void CreateMesh(
					const fbxsdk::FbxMesh* mesh,
					std::vector<SFbxVertex>* pVertices,
					std::vector<unsigned short>* pIndices,
					nsDx12Wrappers::CDescriptorHeap** ppMaterialDH,
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
				std::vector<nsDx12Wrappers::CDescriptorHeap*> m_materialDHs;
				std::unordered_map <std::string, nsDx12Wrappers::CTexture*> m_diffuseTextures;

				nsDx12Wrappers::CConstantBuffer m_constantBuffer;
				nsDx12Wrappers::CDescriptorHeap m_descriptorHeap;
				std::unordered_map <std::string, nsDx12Wrappers::CConstantBuffer*> m_materialCBTable;
				std::unordered_map <std::string, nsDx12Wrappers::CDescriptorHeap*> m_materialDHTable;
			};

		}
	}
}