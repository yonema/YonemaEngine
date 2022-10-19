#include "PMDRenderer.h"
#include "../../GameWindow/MessageBox.h"
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
			const int CPMDRenderer::m_kNumMaterialDescriptors = 4;
			const int CPMDRenderer::m_kNumCalculationsOnBezier = 12;


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

			void CPMDRenderer::Update()
			{
				m_debugRotY += 0.01f;
				nsMath::CMatrix mTrans;
				mTrans.MakeTranslation(m_debugPosX, 0.0f, 0.0f);
				nsMath::CMatrix mRot;
				mRot.MakeRotationY(m_debugRotY);
				nsMath::CMatrix mScale;
				mScale.MakeScaling(1.0f, 1.0f, 1.0f);
				m_mWorld = mScale * mRot * mTrans;
				m_mappedConstantBuff[0] = m_mWorld;
				auto mViewProj = CGraphicsEngine::GetInstance()->GetMatrixViewProj();
				m_mappedConstantBuff[1] = m_mWorld * mViewProj;

				UpdateAnimation();

				return;
			}

			void CPMDRenderer::Draw()
			{
				static auto commandList = CGraphicsEngine::GetInstance()->GetCommandList();
				static auto device = CGraphicsEngine::GetInstance()->GetDevice();

				// ○頂点バッファとインデックスバッファをセット
				commandList->IASetVertexBuffers(0, 1, &m_vertexBuffView);
				commandList->IASetIndexBuffer(&m_indexBuffView);

				// ○定数バッファのディスクリプタヒープをセット
				ID3D12DescriptorHeap* cbDescriptorHeaps[] = { m_cbDescriptorHeap };
				commandList->SetDescriptorHeaps(1, cbDescriptorHeaps);
				auto descriptorHeapH = m_cbDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
				// 0番は、グラフィックスエンジンで、シーンデータ用に使用している。
				// そのため1番からスタート。
				commandList->SetGraphicsRootDescriptorTable(1, descriptorHeapH);

				// ○マテリアルバッファのディスクリプタヒープをセット
				ID3D12DescriptorHeap* mbDescriptorHeaps[] = { m_mbDescriptorHeap };
				commandList->SetDescriptorHeaps(1, mbDescriptorHeaps);

				descriptorHeapH = m_mbDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
				unsigned int idxOffset = 0;
				const auto cbvsrvIncSize =
					device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
					* m_kNumMaterialDescriptors;

				for (const auto& m : m_materials)
				{
					commandList->SetGraphicsRootDescriptorTable(2, descriptorHeapH);
					commandList->DrawIndexedInstanced(m.indicesNum, 1, idxOffset, 0, 0);
					descriptorHeapH.ptr += cbvsrvIncSize;
					idxOffset += m.indicesNum;
				}
				return;
			}

			void CPMDRenderer::PlayAnimation()
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
				if (m_mbDescriptorHeap)
				{
					m_mbDescriptorHeap->Release();
				}
				if (m_materialBuff)
				{
					m_materialBuff->Release();
				}
				if (m_cbDescriptorHeap)
				{
					m_cbDescriptorHeap->Release();
				}
				if (m_constantBuff)
				{
					m_constantBuff->Release();
				}
				if (m_indexBuff)
				{
					m_indexBuff->Release();
				}
				if (m_vertexBuff)
				{
					m_vertexBuff->Release();
				}

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
					pmdVertices.resize(pmdVertNum * m_kPmdVertexSize);
					fread_s(pmdVertices.data(), pmdVertices.size(), pmdVertices.size(), 1, fileStream);

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
				// ○頂点バッファの生成
				auto result = device->CreateCommittedResource(
					&heapProp,
					D3D12_HEAP_FLAG_NONE,
					&resDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(&m_vertexBuff)
				);

				if (FAILED(result))
				{
					nsGameWindow::MessageBoxWarning(L"頂点バッファの生成に失敗しました。");
				}

				// 〇頂点情報のコピー（マップ）
				unsigned char* vertMap = nullptr;
				result = m_vertexBuff->Map(0, nullptr, reinterpret_cast<void**>(&vertMap));
				std::copy(std::begin(pmdVertices), std::end(pmdVertices), vertMap);
				m_vertexBuff->Unmap(0, nullptr);

				// 〇頂点バッファビューの作成
				m_vertexBuffView.BufferLocation = m_vertexBuff->GetGPUVirtualAddress();
				m_vertexBuffView.SizeInBytes = static_cast<UINT>(pmdVertices.size());
				m_vertexBuffView.StrideInBytes = static_cast<UINT>(m_kPmdVertexSize);

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
				resDesc.Width = pmdIndicesSize;

				// ○インデックスバッファの生成
				auto result = device->CreateCommittedResource(
					&heapProp,
					D3D12_HEAP_FLAG_NONE,
					&resDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(&m_indexBuff)
				);

				if (FAILED(result))
				{
					nsGameWindow::MessageBoxWarning(L"インデックスバッファの生成に失敗しました。");
				}

				// 〇インデックス情報のコピー（マップ）
				unsigned short* mappedIdx = nullptr;
				m_indexBuff->Map(0, nullptr, reinterpret_cast<void**>(&mappedIdx));
				std::copy(std::begin(pmdIndices), std::end(pmdIndices), mappedIdx);
				m_indexBuff->Unmap(0, nullptr);

				// 〇インデックスバッファビューの作成
				m_indexBuffView.BufferLocation = m_indexBuff->GetGPUVirtualAddress();
				// unsigned int でインデックス配列を使用しているため、DXGI_FORMAT_R16_UINTを使用。
				m_indexBuffView.Format = DXGI_FORMAT_R16_UINT;
				m_indexBuffView.SizeInBytes = static_cast<UINT>(pmdIndicesSize);

				return;
			}

			void CPMDRenderer::CreateConstantBuff(ID3D12Device5* const device)
			{
				D3D12_HEAP_PROPERTIES constBuffHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
				// mWorld + mWorldViewProj + mBones
				UINT64 constBuffSize = sizeof(nsMath::CMatrix) * (2 + m_boneMatrices.size());
				constBuffSize = (constBuffSize + 0xff) & ~0xff;
				D3D12_RESOURCE_DESC constBuffResDesc = CD3DX12_RESOURCE_DESC::Buffer(constBuffSize);
				auto result = device->CreateCommittedResource(
					&constBuffHeapProp,
					D3D12_HEAP_FLAG_NONE,
					&constBuffResDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(&m_constantBuff)
				);

				if (FAILED(result))
				{
					nsGameWindow::MessageBoxError(L"定数バッファの生成に失敗しました。");
					return;
				}

				result = m_constantBuff->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedConstantBuff));
				m_mappedConstantBuff[0] = m_mWorld;
				auto mViewProj = CGraphicsEngine::GetInstance()->GetMatrixViewProj();
				m_mappedConstantBuff[1] = m_mWorld * mViewProj;
				copy(m_boneMatrices.begin(), m_boneMatrices.end(), m_mappedConstantBuff + 2);

				D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
				descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
				descHeapDesc.NodeMask = 0;
				descHeapDesc.NumDescriptors = 1;
				descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

				result = device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&m_cbDescriptorHeap));
				if (FAILED(result))
				{
					nsGameWindow::MessageBoxError(L"定数バッファのディスクリプタヒープの生成に失敗しました。");
					return;
				}

				auto heapHandle = m_cbDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

				D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
				cbvDesc.BufferLocation = m_constantBuff->GetGPUVirtualAddress();
				cbvDesc.SizeInBytes = static_cast<UINT>(m_constantBuff->GetDesc().Width);

				device->CreateConstantBufferView(&cbvDesc, heapHandle);

				return;

			}


			void CPMDRenderer::CreateMaterialBuff(
				ID3D12Device5* const device,
				const D3D12_HEAP_PROPERTIES& heapProp,
				D3D12_RESOURCE_DESC& resDesc,
				unsigned int pmdMaterialNum
			)
			{
				// 〇マテリアルバッファの作成
				UINT materialBuffSize = sizeof(SMaterialForHlsl);
				// @todo マテリアルバッファのサイズに無駄な領域ができてしまっている。あとで別の方法を検証。
				// 無駄な領域は生じるが、バッファサイズを256アライメントにする
				materialBuffSize = (materialBuffSize + 0xff) & ~0xff;

				resDesc.Width =
					static_cast<UINT64>(materialBuffSize) * static_cast<UINT64>(pmdMaterialNum);

				auto result = device->CreateCommittedResource(
					&heapProp,
					D3D12_HEAP_FLAG_NONE,
					&resDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(&m_materialBuff)
				);

				// マップマテリアルにコピー
				char* mapMaterial = nullptr;
				result = m_materialBuff->Map(0, nullptr, reinterpret_cast<void**>(&mapMaterial));
				for (auto& m : m_materials)
				{
					// データのコピー
					*reinterpret_cast<SMaterialForHlsl*>(mapMaterial) = m.matForHlsl;
					// 次のアライメント位置まで進める。（256の倍数）
					mapMaterial += materialBuffSize;
				}
				m_materialBuff->Unmap(0, nullptr);

				// マテリアル用ディスクリプタヒープとビューの作成
				D3D12_DESCRIPTOR_HEAP_DESC matDescHeapDesc = {};
				matDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
				matDescHeapDesc.NodeMask = 0;
				// マテリアルデータとテクスチャ*3の4つ分。
				matDescHeapDesc.NumDescriptors = pmdMaterialNum * m_kNumMaterialDescriptors;
				matDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

				result = device->CreateDescriptorHeap(
					&matDescHeapDesc,
					IID_PPV_ARGS(&m_mbDescriptorHeap)
				);

				if (FAILED(result))
				{
					// マテリアル用のディスクリプタヒープの生成失敗。
					nsGameWindow::MessageBoxWarning(L"マテリアル用のディスクリプタヒープの生成に失敗しました。");
				}

				D3D12_CONSTANT_BUFFER_VIEW_DESC matCBVDesc = {};
				matCBVDesc.BufferLocation = m_materialBuff->GetGPUVirtualAddress();
				matCBVDesc.SizeInBytes = materialBuffSize;

				D3D12_SHADER_RESOURCE_VIEW_DESC matSRVDesc = {};
				matSRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				matSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				matSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				matSRVDesc.Texture2D.MipLevels = 1;

				// 先頭を記録
				auto matDescHeapH = m_mbDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
				const auto inc =
					device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				auto whiteTex = CGraphicsEngine::GetInstance()->GetWhiteTexture();
				auto blackTex = CGraphicsEngine::GetInstance()->GetBlackTexture();

				for (int i = 0; i < static_cast<int>(pmdMaterialNum); i++)
				{
					// マテリアルデータ用定数バッファビュー。
					device->CreateConstantBufferView(&matCBVDesc, matDescHeapH);

					matDescHeapH.ptr += inc;
					matCBVDesc.BufferLocation += materialBuffSize;

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


			void CPMDRenderer::UpdateAnimation()
			{
				const auto deltaTime = CYonemaEngine::GetInstance()->GetDeltaTime();
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

				copy(m_boneMatrices.begin(), m_boneMatrices.end(), m_mappedConstantBuff + 2);

				return;
			}

			float CPMDRenderer::GetYFromXOnBezier(
				float x, const nsMath::CVector2& a, const nsMath::CVector2& b, uint8_t n)
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