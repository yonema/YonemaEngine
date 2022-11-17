#include "PMDRenderer.h"
#include "../Utils/StringManipulation.h"
#include "../Dx12Wrappers/Texture.h"
#include "../GraphicsEngine.h"
#include "../../YonemaEngine.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsPMDModels
		{
			const size_t CPMDRenderer::m_kPmdVertexSize = 38;
			const size_t CPMDRenderer::m_kAlignedPmdVertexSize = 40;
			const int CPMDRenderer::m_kNumMaterialDescriptors = 4;
			const int CPMDRenderer::m_kNumCalculationsOnBezier = 12;

			void CPMDRenderer::Draw(nsDx12Wrappers::CCommandList* commandList)
			{
				// ○頂点バッファとインデックスバッファをセット
				commandList->SetVertexBuffer(m_vertexBuffer);
				commandList->SetIndexBuffer(m_indexBuffer);

				// ○定数バッファのディスクリプタヒープをセット
				ID3D12DescriptorHeap* modelDescHeaps[] = { m_modelDH.Get() };
				commandList->SetDescriptorHeaps(1, modelDescHeaps);
				auto descriptorHeapH = m_modelDH.GetGPUHandle();
				// 0番は、グラフィックスエンジンで、シーンデータ用に使用している。
				// そのため1番からスタート。
				commandList->SetGraphicsRootDescriptorTable(1, descriptorHeapH);

				// ○マテリアルバッファのディスクリプタヒープをセット
				ID3D12DescriptorHeap* materialDescHeaps[] = { m_materialDH.Get() };
				commandList->SetDescriptorHeaps(1, materialDescHeaps);

				descriptorHeapH = m_materialDH.GetGPUHandle();
				unsigned int idxOffset = 0;
				const auto cbvsrvIncSize =
					CGraphicsEngine::GetInstance()->GetDescriptorSizeOfCbvSrvUav() *
					m_kNumMaterialDescriptors;

				for (const auto& m : m_materials)
				{
					commandList->SetGraphicsRootDescriptorTable(2, descriptorHeapH);
					commandList->DrawInstanced(m.indicesNum, 1, idxOffset);
					descriptorHeapH.ptr += cbvsrvIncSize;
					idxOffset += m.indicesNum;
				}
				return;
			}

			CPMDRenderer::CPMDRenderer(const char* filePath, const char* animFilePath)
			{
				Init(filePath, animFilePath);

				return;
			}
			CPMDRenderer::~CPMDRenderer()
			{
				Terminate();

				return;
			}

			void CPMDRenderer::UpdateWorldMatrix(
				const nsMath::CVector3& position,
				const nsMath::CQuaternion& rotation,
				const nsMath::CVector3& scale
			)
			{
				// ワールド行列作成。
				nsMath::CMatrix mTrans, mRot, mScale, mWorld;
				mTrans.MakeTranslation(position);
				mRot.MakeRotationFromQuaternion(rotation);
				mScale.MakeScaling(scale);
				mWorld = mScale * mRot * mTrans;

				// 定数バッファにコピー。
				auto mappedCB =
					static_cast<nsMath::CMatrix*>(m_modelCB.GetMappedConstantBuffer());
				mappedCB[0] = mWorld;
				auto mViewProj = CGraphicsEngine::GetInstance()->GetMatrixViewProj();
				mappedCB[1] = mWorld * mViewProj;

				return;
			}



			void CPMDRenderer::PlayAnimation(unsigned int animIdx)noexcept
			{


			}


			void CPMDRenderer::Init(const char* filePath, const char* animFilePath)
			{
				if (animFilePath)
				{
					LoadVMDAnimation(animFilePath);
				}

				LoadPMDModel(filePath);

				return;
			}

			void CPMDRenderer::Terminate()
			{
				m_materialDH.Release();
				m_materialCB.Release();
				m_modelDH.Release();
				m_modelCB.Release();
				m_indexBuffer.Release();
				m_vertexBuffer.Release();

				for (int i = 0; i < m_textures.size(); i++)
				{
					delete m_textures[i];
					delete m_sphTextures[i];
					delete m_spaTextures[i];
				}

				return;
			}

			void CPMDRenderer::LoadPMDModel(const char* filePath)
			{
				// 〇PMDファイルの読み込み
				FILE* fileStream = nullptr;

				unsigned int pmdVertNum = 0;
				std::vector<unsigned char> pmdVertices;

				unsigned int pmdIndicesNum = 0;
				std::vector<unsigned short> pmdIndices;
				size_t pmdIndicesSize = 0;

				unsigned int pmdMaterialNum = 0;
				std::vector<SPMDMaterial> pmdMaterials;
				size_t pmdMaterialSize = 0;

				unsigned short pmdBoneNum = 0;
				std::vector<SPMDBone> pmdBones;

				if (fopen_s(&fileStream, filePath, "rb") == 0)
				{
					// ヘッダの読み込み
					char signature[3] = {};
					SPMDHeader pmdHeader = {};
					fread_s(signature, sizeof(signature), sizeof(signature), 1, fileStream);
					fread_s(&pmdHeader, sizeof(pmdHeader), sizeof(pmdHeader), 1, fileStream);

					// 頂点数の読み込み
					fread_s(&pmdVertNum, sizeof(pmdVertNum), sizeof(pmdVertNum), 1, fileStream);

					// 頂点データの読み込み
					//アライメント無視の読み込み
					//pmdVertices.resize(m_kPmdVertexSize * pmdVertNum);
					//fread_s(pmdVertices.data(), pmdVertices.size(), pmdVertices.size(), 1, fileStream);

					// アライメントを考慮した読み込み
					pmdVertices.resize(m_kAlignedPmdVertexSize * pmdVertNum);
					for (unsigned int i = 0; i < pmdVertNum; i++)
					{
						fread_s(
							&pmdVertices[i * m_kAlignedPmdVertexSize],
							m_kPmdVertexSize,
							m_kPmdVertexSize,
							1,
							fileStream
						);
					}

					// インデックス数の読み込み
					fread_s(&pmdIndicesNum, sizeof(pmdIndicesNum), sizeof(pmdIndicesNum), 1, fileStream);

					// インデックスデータの読み込み
					pmdIndices.resize(pmdIndicesNum);
					pmdIndicesSize = pmdIndices.size() * sizeof(pmdIndices[0]);
					fread_s(pmdIndices.data(), pmdIndicesSize, pmdIndicesSize, 1, fileStream);

					// マテリアル数の読み込み
					fread_s(&pmdMaterialNum, sizeof(pmdMaterialNum), sizeof(pmdMaterialNum), 1, fileStream);

					// マテリアルデータの読み込み
					pmdMaterials.resize(pmdMaterialNum);
					pmdMaterialSize = pmdMaterials.size() * sizeof(SPMDMaterial);
					fread_s(pmdMaterials.data(), pmdMaterialSize, pmdMaterialSize, 1, fileStream);

					// ボーン数の読み込み
					fread_s(&pmdBoneNum, sizeof(pmdBoneNum), sizeof(pmdBoneNum), 1, fileStream);

					// ボーンデータの読み込み
					pmdBones.resize(pmdBoneNum);
					auto pmdBoneSize = pmdBones.size() * sizeof(SPMDBone);
					//fread_s(pmdBones.data(), sizeof(SPMDBone), sizeof(SPMDBone), pmdBoneNum, fileStream);
					fread_s(pmdBones.data(), pmdBoneSize, pmdBoneSize, 1, fileStream);
					


					fclose(fileStream);
				}
				else
				{
					std::wstring wstr = L"PMDファイルの読み込みに失敗しました。ファイルパスを確認してください。";
					wstr += nsUtils::GetWideStringFromString(filePath);
					nsGameWindow::MessageBoxWarning(wstr.c_str());
				}

				m_materials.resize(pmdMaterials.size());
				m_textures.resize(pmdMaterials.size());
				m_sphTextures.resize(pmdMaterials.size());
				m_spaTextures.resize(pmdMaterials.size());


				for (int i = 0; i < pmdMaterials.size(); i++)
				{
					// マテリアルデータのコピー。
					m_materials[i].indicesNum = pmdMaterials[i].indicesNum;
					m_materials[i].matForHlsl.diffuse = pmdMaterials[i].diffuse;
					m_materials[i].matForHlsl.alpha = pmdMaterials[i].alpha;
					m_materials[i].matForHlsl.specular = pmdMaterials[i].specular;
					m_materials[i].matForHlsl.specularity = pmdMaterials[i].specularity;
					m_materials[i].matForHlsl.ambient = pmdMaterials[i].ambient;

					// テクスチャのロード

					if (strlen(pmdMaterials[i].texFilePath) == 0)
					{
						// テクスチャなし。
						continue;
					}

					// テクスチャあり。

					std::string texFileName = pmdMaterials[i].texFilePath;
					if (std::count(texFileName.begin(), texFileName.end(), '*') > 0)
					{
						// スプリッタあり。

						auto namePair = nsUtils::SplitFilename(texFileName);
						std::string otherTexFileName;

						// スフィアマップじゃないほうをテクスチャ名にする。
						if (nsUtils::GetExtension(namePair.first) == "sph" ||
							nsUtils::GetExtension(namePair.first) == "spa")
						{
							texFileName = namePair.second;
							otherTexFileName = namePair.first;
						}
						else
						{
							texFileName = namePair.first;
							otherTexFileName = namePair.second;
						}

						// スフィアマップの方のロード。

						auto otherTexFilePath = nsUtils::GetTexturePathFromModelAndTexPath(
							filePath,
							otherTexFileName.c_str()
						);

						if (nsUtils::GetExtension(otherTexFilePath) == "sph")
						{
							m_sphTextures[i] = new nsDx12Wrappers::CTexture();
							m_sphTextures[i]->Init(otherTexFilePath.c_str());
						}
						else
						{
							m_spaTextures[i] = new nsDx12Wrappers::CTexture();
							m_spaTextures[i]->Init(otherTexFilePath.c_str());
						}
					}

					auto texFilePath = nsUtils::GetTexturePathFromModelAndTexPath(
						filePath,
						texFileName.c_str()
					);

					if (nsUtils::GetExtension(texFilePath) == "sph")
					{
						m_sphTextures[i] = new nsDx12Wrappers::CTexture();
						m_sphTextures[i]->Init(texFilePath.c_str());
					}
					else if (nsUtils::GetExtension(texFilePath) == "spa")
					{
						m_spaTextures[i] = new nsDx12Wrappers::CTexture();
						m_spaTextures[i]->Init(texFilePath.c_str());
					}
					else
					{
						m_textures[i] = new nsDx12Wrappers::CTexture();
						m_textures[i]->Init(texFilePath.c_str());
					}

				}

				// ボーンデータのコピー

				std::vector<const char*> boneNames(pmdBones.size());
				// ボーンノードテーブルを作成
				for (int i = 0; i < pmdBones.size(); i++)
				{
					auto& pb = pmdBones[i];
					boneNames[i] = pb.boneName;
					auto& node = m_boneNodeTable[pb.boneName];
					node.boneIdx = i;
					node.startPos = pb.pos;
				}

				// 親子関係を構築する
				for (auto& pb : pmdBones)
				{
					if (pb.parentNo >= pmdBones.size())
					{
						// 親インデックスチェック。ありえない番号は飛ばす。
						continue;
					}

					auto parentName = boneNames[pb.parentNo];
					m_boneNodeTable[parentName].children.emplace_back(&m_boneNodeTable[pb.boneName]);
				}

				m_boneMatrices.resize(pmdBones.size());

				for (auto& boneMotion : m_motionData)
				{
					auto itBoneNode = m_boneNodeTable.find(boneMotion.first);
					if (itBoneNode == m_boneNodeTable.end())
					{
						// モデルにないボーンは無視する。
						continue;
					}
					auto node = itBoneNode->second;
					auto& pos = node.startPos;
					auto mat = nsMath::CMatrix::Translation(-pos.x, -pos.y, -pos.z) *
						nsMath::CMatrix::RotationFromQuaternion(boneMotion.second[0].rotation) *
						nsMath::CMatrix::Translation(pos);
					m_boneMatrices[node.boneIdx] = mat;
				}

				// ルートノードから開始する
				RecursiveMatrixMultiply(&m_boneNodeTable["センター"], nsMath::CMatrix::Identity());

				CreateBuff(pmdVertices, pmdIndices, pmdIndicesSize, pmdMaterialNum);

				return;
			}

			void CPMDRenderer::CreateBuff(
				std::vector<unsigned char>& pmdVertices,
				std::vector<unsigned short>& pmdIndices,
				size_t pmdIndicesSize,
				unsigned int pmdMaterialNum
			)
			{
				// 各バッファで同じ設定を使いまわす。
				D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
				D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(pmdVertices.size());

				static auto device = CGraphicsEngine::GetInstance()->GetDevice();

				
				CreateVertexBuff(device, heapProp, resDesc, pmdVertices);

				CreateIndexBuff(device, heapProp, resDesc, pmdIndices, pmdIndicesSize);

				CreateConstantBuff(device);

				CreateMaterialBuff(device, heapProp, resDesc, pmdMaterialNum);



				return;
			}


			void CPMDRenderer::CreateVertexBuff(
				ID3D12Device5* const device,
				const D3D12_HEAP_PROPERTIES& heapProp,
				const D3D12_RESOURCE_DESC& resDesc,
				std::vector<unsigned char>& pmdVertices
			)
			{
				m_vertexBuffer.Init(
					static_cast<unsigned int>(pmdVertices.size()),
					//m_kPmdVertexSize,	// アライメント無視
					m_kAlignedPmdVertexSize,	// アライメント考慮
					static_cast<void*>(&pmdVertices[0]),
					L"PMDModel"
				);


				return;
			}

			void CPMDRenderer::CreateIndexBuff(
				ID3D12Device5* const device,
				const D3D12_HEAP_PROPERTIES& heapProp,
				D3D12_RESOURCE_DESC& resDesc,
				std::vector<unsigned short>& pmdIndices,
				size_t pmdIndicesSize
			)
			{
				m_indexBuffer.Init(
					static_cast<unsigned int>(pmdIndicesSize),
					static_cast<void*>(&pmdIndices[0]),
					L"PMDModel"
				);

				return;
			}

			void CPMDRenderer::CreateConstantBuff(ID3D12Device5* const device)
			{
				// 〇定数バッファ作成
				auto cbSize = sizeof(nsMath::CMatrix) * (2 + m_boneMatrices.size());
				m_modelCB.Init(static_cast<unsigned int>(cbSize), L"PMDModel");

				// 〇マップされたデータにデータをコピー
				auto mappedCb = 
					static_cast<nsMath::CMatrix*>(m_modelCB.GetMappedConstantBuffer());
				mappedCb[0] = m_mWorld;
				auto mViewProj = CGraphicsEngine::GetInstance()->GetMatrixViewProj();
				mappedCb[1] = m_mWorld * mViewProj;
				copy(m_boneMatrices.begin(), m_boneMatrices.end(), mappedCb + 2);

				// 〇ディスクリプタヒープ作成
				constexpr unsigned int numDescHeaps = 1;
				m_modelDH.InitAsCbvSrvUav(numDescHeaps, L"PMDModel");

				// 〇定数バッファビュー作成
				m_modelCB.CreateConstantBufferView(m_modelDH.GetCPUHandle());

				return;

			}


			void CPMDRenderer::CreateMaterialBuff(
				ID3D12Device5* const device,
				const D3D12_HEAP_PROPERTIES& heapProp,
				D3D12_RESOURCE_DESC& resDesc,
				unsigned int pmdMaterialNum
			)
			{
				// 〇定数バッファ作成
				auto cbSize = static_cast<unsigned int>(sizeof(SMaterialForHlsl));
				m_materialCB.Init(cbSize, L"PMDMaterial", pmdMaterialNum);
				cbSize = m_materialCB.GetSizeInByte();

				// 〇マップされたデータにデータをコピー
				char* mapMaterial = static_cast<char*>(m_materialCB.GetMappedConstantBuffer());
				for (auto& m : m_materials)
				{
					// データのコピー
					*reinterpret_cast<SMaterialForHlsl*>(mapMaterial) = m.matForHlsl;
					// 次のアライメント位置まで進める。（256の倍数）
					mapMaterial += cbSize;
				}
				m_materialCB.Unmap();


				// 〇ディスクリプタヒープの作成
				const auto numDescHeaps = pmdMaterialNum * m_kNumMaterialDescriptors;
				m_materialDH.InitAsCbvSrvUav(numDescHeaps, L"PMDMaterial");

				D3D12_SHADER_RESOURCE_VIEW_DESC matSRVDesc = {};
				matSRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				matSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				matSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				matSRVDesc.Texture2D.MipLevels = 1;

				// 先頭を記録
				auto matDescHeapH = m_materialDH.GetCPUHandle();
				const auto inc = CGraphicsEngine::GetInstance()->GetDescriptorSizeOfCbvSrvUav();

				auto whiteTex = CGraphicsEngine::GetInstance()->GetWhiteTexture();
				auto blackTex = CGraphicsEngine::GetInstance()->GetBlackTexture();

				for (int i = 0; i < static_cast<int>(pmdMaterialNum); i++)
				{
					// マテリアルデータ用定数バッファビュー。
					m_materialCB.CreateConstantBufferView(matDescHeapH);

					matDescHeapH.ptr += inc;

					// テクスチャ用シェーダーリソースビュー。
					if (m_textures[i] != nullptr)
					{
						matSRVDesc.Format = m_textures[i]->GetResource()->GetDesc().Format;
						device->CreateShaderResourceView(
							m_textures[i]->GetResource(),
							&matSRVDesc,
							matDescHeapH
						);

					}
					else
					{
						matSRVDesc.Format = whiteTex->GetResource()->GetDesc().Format;
						device->CreateShaderResourceView(
							whiteTex->GetResource(),
							&matSRVDesc,
							matDescHeapH
						);
					}

					matDescHeapH.ptr += inc;

					if (m_sphTextures[i] != nullptr)
					{
						matSRVDesc.Format = m_sphTextures[i]->GetResource()->GetDesc().Format;
						device->CreateShaderResourceView(
							m_sphTextures[i]->GetResource(),
							&matSRVDesc,
							matDescHeapH
						);

					}
					else
					{
						matSRVDesc.Format = whiteTex->GetResource()->GetDesc().Format;
						device->CreateShaderResourceView(
							whiteTex->GetResource(),
							&matSRVDesc,
							matDescHeapH
						);
					}

					matDescHeapH.ptr += inc;

					if (m_spaTextures[i] != nullptr)
					{
						matSRVDesc.Format = m_spaTextures[i]->GetResource()->GetDesc().Format;
						device->CreateShaderResourceView(
							m_spaTextures[i]->GetResource(),
							&matSRVDesc,
							matDescHeapH
						);

					}
					else
					{
						matSRVDesc.Format = blackTex->GetResource()->GetDesc().Format;
						device->CreateShaderResourceView(
							blackTex->GetResource(),
							&matSRVDesc,
							matDescHeapH
						);
					}

					matDescHeapH.ptr += inc;

				}

				return;
			}

			void CPMDRenderer::RecursiveMatrixMultiply(SBoneNode* node, const nsMath::CMatrix& mat)
			{
				m_boneMatrices[node->boneIdx] *= mat;
				for (auto& childNode : node->children)
				{
					RecursiveMatrixMultiply(childNode, m_boneMatrices[node->boneIdx]);
				}
				return;
			}

			void CPMDRenderer::LoadVMDAnimation(const char* filePath)
			{
				FILE* fileStream = nullptr;
				if (fopen_s(&fileStream, filePath, "rb") == 0)
				{
					// ヘッダ50バイトは見る必要なし。
					fseek(fileStream, 50, SEEK_SET);

					unsigned int numMotionData = 0;
					fread_s(&numMotionData, sizeof(numMotionData), sizeof(numMotionData), 1, fileStream);

					std::vector<SVMDMotion> vmdMotionDatas(numMotionData);
					for (auto& motion : vmdMotionDatas)
					{
						fread_s(&motion.boneName, sizeof(motion.boneName), sizeof(motion.boneName), 1, fileStream);
						// アライメントにより、boneNameの後にアライメントが発生しているため、分けて読み込む。
						auto sizeRemainingData =
							sizeof(motion.frameNo) +
							sizeof(motion.location) +
							sizeof(motion.rotation) +
							sizeof(motion.bezier);
						fread_s(&motion.frameNo, sizeRemainingData, sizeRemainingData, 1, fileStream);
					}


					for (auto& vmdMotion : vmdMotionDatas)
					{
						nsMath::CVector2 p1 = {
							static_cast<float>(vmdMotion.bezier[3]) / 127.0f,
							static_cast<float>(vmdMotion.bezier[7]) / 127.0f };
						nsMath::CVector2 p2 = {
							static_cast<float>(vmdMotion.bezier[11]) / 127.0f,
							static_cast<float>(vmdMotion.bezier[15]) / 127.0f };
						m_motionData[vmdMotion.boneName].emplace_back(
							vmdMotion.frameNo, vmdMotion.rotation, p1, p2);

						m_maxFrameNo = std::max<unsigned int>(m_maxFrameNo, vmdMotion.frameNo);
					}

					for (auto& motion : m_motionData)
					{
						std::sort(motion.second.begin(), motion.second.end(),
							[](const SKeyFrame& lval, const SKeyFrame& rval)
							{
								return lval.frameNo <= rval.frameNo;
							});
						
					}

					fclose(fileStream);
				}
				else
				{
					std::wstring wstr = L"VMDファイルの読み込みに失敗しました。ファイルパスを確認してください。";
					wstr += nsUtils::GetWideStringFromString(filePath);
					nsGameWindow::MessageBoxWarning(wstr.c_str());
				}

				return;
			}


			void CPMDRenderer::UpdateAnimation(float deltaTime)
			{
				m_playAnimTime += deltaTime;
				unsigned int frameNo = static_cast<unsigned int>(30.0f * m_playAnimTime);

				if (frameNo > m_maxFrameNo)
				{
					m_playAnimTime = 0.0f;
					frameNo = 0;
				}

				// ボーンの行列をクリア。
				// クリアしないと、前フレームのポーズが重ね掛けされてモデルがおかしくなる。
				std::fill(m_boneMatrices.begin(), m_boneMatrices.end(),
					nsMath::CMatrix::Identity());

				for (auto& boneMotion : m_motionData)
				{
					auto node = m_boneNodeTable[boneMotion.first];

					auto motions = boneMotion.second;
					auto rIt = std::find_if(motions.rbegin(), motions.rend(),
						[frameNo](const SKeyFrame& keyFrame)
						{
							return keyFrame.frameNo <= frameNo;
						});

					if (rIt == motions.rend())
					{
						// 合致するものがなければ処理を飛ばす。
						continue;
					}

					nsMath::CQuaternion rotation(rIt->rotation);
					auto it = rIt.base();
					if (it != motions.end())
					{
						auto rate = static_cast<float>(30.0f * m_playAnimTime - rIt->frameNo) /
							static_cast<float>(it->frameNo - rIt->frameNo);
						rotation.Slerp(rate, rIt->rotation, it->rotation);

						rate = GetYFromXOnBezier(rate, it->p1, it->p2, m_kNumCalculationsOnBezier);
					}

					auto& pos = node.startPos;
					auto mat = nsMath::CMatrix::Translation(-pos.x, -pos.y, -pos.z) *
						nsMath::CMatrix::RotationFromQuaternion(rotation) *
						nsMath::CMatrix::Translation(pos);
					m_boneMatrices[node.boneIdx] = mat;

				}

				RecursiveMatrixMultiply(&m_boneNodeTable["センター"], nsMath::CMatrix::Identity());
				auto mappedCB = static_cast<nsMath::CMatrix*>(m_modelCB.GetMappedConstantBuffer());
				copy(m_boneMatrices.begin(), m_boneMatrices.end(), mappedCB + 2);

				return;
			}

			float CPMDRenderer::GetYFromXOnBezier(
				float x, const nsMath::CVector2& a, const nsMath::CVector2& b, uint8_t n) noexcept
			{
				if (a.x == a.y && b.x == b.y)
				{
					// 計算不要
					return x;
				}

				float t = x;
				const float k0 = 1.0f + 3.0f * a.x - 3.0f * b.x;	// t^3の係数
				const float k1 = 3.0f * b.x - 6.0f * a.x;			// t^2の係数
				const float k2 = 3.0f * a.x;						// tの係数

				constexpr float epsilon = 0.0005f;

				for (int i = 0; i < n; i++)
				{
					// f(t)を求める
					float ft = k0 * t * t * t + k1 * t * t + k2 * t - x;

					if (ft <= epsilon && ft >= -epsilon)
					{
						break;
					}

					// 刻む
					t -= ft / 2.0f;
				}

				float r = 1.0f - t;

				return t * t * t + 3.0f * t * t * r * b.y + 3.0f * t * r * r * a.y;
			}

		}
	}
}