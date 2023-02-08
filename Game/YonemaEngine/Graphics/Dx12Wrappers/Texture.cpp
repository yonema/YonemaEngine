#include "Texture.h"
#include "../GraphicsEngine.h"
#include "../../Utils/StringManipulation.h"
#include "../../Utils/AlignSize.h"
#include <ResourceUploadBatch.h>
#include <DDSTextureLoader.h>

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDx12Wrappers
		{
			const char* CTexture::m_kWICFileExtensions[m_kUpAndLowExtensions][m_kNumWICFileExtensions] =
			{
				{ "sph", "spa", "bmp", "png", "jpg"	},
				{ "SPH", "SAP", "BMP", "PNG", "JPG" }
			};
			const char* CTexture::m_kTGAFileExtensions[m_kUpAndLowExtensions] = { "tga", "TGA" };
			const char* CTexture::m_kDDSFileExtensions[m_kUpAndLowExtensions] = { "dds",	"DDS" };
			const wchar_t* const CTexture::m_kNamePrefix = L"Texture: ";


			CTexture::~CTexture()
			{
				Terminate();

				return;
			}

			void CTexture::Release()
			{
				if (m_texture)
				{
					m_texture->Release();
					m_texture = nullptr;
				}
				return;
			}


			void CTexture::Init(const char* filePath)
			{
				Release();
				m_texture = LoadTextureFromFile(filePath);

				if (m_texture == nullptr)
				{
					this->InitFromTexture(
						CGraphicsEngine::GetInstance()->GetDefaultTextures().GetTexture(
							CDefaultTextures::EnTexType::enWhite)
					);
				}

				SetDefaultName(filePath);

				return;
			}

			void CTexture::InitFromD3DResource(ID3D12Resource* texture)
			{
				Release();
				CopyTextureParam(texture);

				return;
			}

			void CTexture::InitFromTexture(CTexture* texture)
			{
				Release();
				CopyTextureParam(texture->GetResource());
				m_isCubemap = texture->IsCubemap();


				return;
			}

			void CTexture::InitFromDDSFile(const char* filePath)
			{
				Release();

				
				auto* device = CGraphicsEngine::GetInstance()->GetDevice();
				DirectX::ResourceUploadBatch re(device);
				re.Begin();
				ID3D12Resource* texture;
				auto hr = DirectX::CreateDDSTextureFromFileEx(
					device,
					re,
					nsUtils::GetWideStringFromString(filePath).c_str(),
					0,
					D3D12_RESOURCE_FLAG_NONE,
					DirectX::DDS_LOADER_DEFAULT,
					&texture,
					nullptr,
					&m_isCubemap
				);
				re.End(CGraphicsEngine::GetInstance()->GetCommandQueue());

				if (FAILED(hr)) {
					//テクスチャの作成に失敗しました。
					return;
				}

				//m_texture = texture;
				CopyTextureParam(texture);
				texture->Release();

				//m_textureDesc = m_texture->GetDesc();

				return;
			}



			void CTexture::Terminate()
			{
				Release();

				return;
			}

			ID3D12Resource* CTexture::LoadTextureFromFile(const char* filePath)
			{
				const auto& graphicsEngine = CGraphicsEngine::GetInstance();

				// 画像ファイルに関する情報。
				DirectX::TexMetadata metadata = {};
				// 実際のデータ。
				DirectX::ScratchImage scratchImage = {};

				if (LoadMetadataAndScratchImage(filePath, &metadata, &scratchImage) != true)
				{
					return nullptr;
				}

				m_isCubemap = metadata.IsCubemap();
				m_textureSize.x = static_cast<float>(metadata.width);
				m_textureSize.y = static_cast<float>(metadata.height);
				m_format = metadata.format;
				m_mipLevels = static_cast<UINT16>(metadata.mipLevels);

				auto image = scratchImage.GetImage(0, 0, 0);	// 生データ抽出

				// バッファ用256の倍数でアライメントされたrowPitch。
				const auto alignedRowPitch =
					nsUtils::AlignSize(image->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);

				// 1.中間バッファとしての、アップロードリソースを作成
				ID3D12Resource* uploadBuff = nullptr;
				if (CreateUploadBuff(
					graphicsEngine->GetDevice(),
					&uploadBuff,
					alignedRowPitch,
					image->height
				) != true)
				{
					return nullptr;
				}


				// 2.読み出し用リソースの作成
				ID3D12Resource* textureBuff = nullptr;
				if (CreateTextureBuff(
					graphicsEngine->GetDevice(),
					&textureBuff,
					metadata
				) != true)
				{
					uploadBuff->Release();
					return nullptr;
				}

				// 3.アップロード用リソースへテクスチャデータをMap()でコピー

				if (CopyToUploadBuffFromImage(uploadBuff, alignedRowPitch, image) != true)
				{
					uploadBuff->Release();
					textureBuff->Release();
					return nullptr;
				}



				// 4.アップロード用リソースから読み出し用リソースへCopyTextureRegion()でコピー

				CopyToTextureBuffFromUploadBuff(
					textureBuff,
					uploadBuff,
					metadata,
					image,
					alignedRowPitch,
					graphicsEngine
				);


				// コピーしたからもういらない。
				uploadBuff->Release();

				return textureBuff;

			}

			bool CTexture::LoadMetadataAndScratchImage(
				const char* filePath,
				DirectX::TexMetadata* metadata,
				DirectX::ScratchImage* scratchImage
			)
			{
				auto extension = nsUtils::GetExtension(filePath);
				HRESULT result;
				bool roaded = false;

				for (const auto& WICExtensions : m_kWICFileExtensions)
				{
					for (const auto& WICExtension : WICExtensions)
					{
						if (strcmp(extension, WICExtension) != 0)
						{
							continue;
						}

						result = DirectX::LoadFromWICFile(
							nsUtils::GetWideStringFromString(filePath).c_str(),
							DirectX::WIC_FLAGS_NONE,
							metadata,
							*scratchImage
						);
						roaded = true;
						break;
					}
					if (roaded)
					{
						break;
					}
				}

				if (roaded != true)
				{
					for (const auto& TGAFileExtension : m_kTGAFileExtensions)
					{
						if (strcmp(extension, TGAFileExtension) == 0)
						{
							result = DirectX::LoadFromTGAFile(
								nsUtils::GetWideStringFromString(filePath).c_str(),
								metadata,
								*scratchImage
							);
							roaded = true;
							break;
						}

					}
				}
				if (roaded != true)
				{
					for (const auto& DDSFileExtension : m_kDDSFileExtensions)
					{
						if (strcmp(extension, DDSFileExtension) == 0)
						{
							result = DirectX::LoadFromDDSFile(
								nsUtils::GetWideStringFromString(filePath).c_str(),
								DirectX::DDS_FLAGS_NONE,
								metadata,
								*scratchImage
							);
							roaded = true;
							break;
						}
					}
				}

				if (roaded != true)
				{
					std::string msg = filePath;
					msg.erase(msg.end() - 1);
					msg += "のテクスチャのロードに失敗しました。拡張子を確認してください。";
					nsGameWindow::MessageBoxError(nsUtils::GetWideStringFromString(msg).c_str());
					return false;
				}

				if (FAILED(result))
				{
					std::string msg = filePath;
					msg.erase(msg.end() - 1);
					msg += "のテクスチャのロードに失敗しました。ファイルパスを確認してください。";
					nsGameWindow::MessageBoxError(nsUtils::GetWideStringFromString(msg).c_str());
					return false;
				}

				return true;
			}

			bool CTexture::CreateUploadBuff(
				ID3D12Device5* device,
				ID3D12Resource** pUploadBuff,
				const size_t alignedRowPitch,
				const size_t imageHeight
			)
			{
				D3D12_HEAP_PROPERTIES uploadHeapProp =
					CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

				UINT64 resDescWidth = static_cast<UINT64>(alignedRowPitch * imageHeight);
				D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(resDescWidth);

				auto result = device->CreateCommittedResource(
					&uploadHeapProp,
					D3D12_HEAP_FLAG_NONE,
					&resDesc,
					// CPUからマップするためリソースステートを、
					// CPUから描き込み可能だがGPUからは読み取りのみにする。
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(pUploadBuff)
				);

				if (FAILED(result))
				{
					nsGameWindow::MessageBoxError(L"テクスチャロード時の、中間バッファとしてのアップロードリソースの生成に失敗しました。");
					return false;
				}
				return true;
			}

			bool CTexture::CreateTextureBuff(
				ID3D12Device5* device,
				ID3D12Resource** pTextureBuff,
				const DirectX::TexMetadata& metadata
			)
			{
				D3D12_HEAP_PROPERTIES texHeapProp =
					CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

				D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(0);
				resDesc.Format = metadata.format;
				resDesc.Width = static_cast<UINT64>(metadata.width);
				resDesc.Height = static_cast<UINT>(metadata.height);
				resDesc.DepthOrArraySize = static_cast<UINT16>(metadata.arraySize);
				resDesc.MipLevels = static_cast<UINT16>(metadata.mipLevels);
				resDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);
				resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;	// レイアウトは決定しない

				auto result = device->CreateCommittedResource(
					&texHeapProp,
					D3D12_HEAP_FLAG_NONE,	// 特に指定なし
					&resDesc,
					D3D12_RESOURCE_STATE_COPY_DEST, // コピー先
					nullptr,
					IID_PPV_ARGS(pTextureBuff)
				);

				if (FAILED(result))
				{
					nsGameWindow::MessageBoxError(L"テクスチャバッファの生成に失敗しました。");

					return false;
				}

				return true;
			}

			bool CTexture::CopyToUploadBuffFromImage(
				ID3D12Resource* uploadBuff,
				const size_t alignedRowPitch,
				const DirectX::Image* image
			)
			{
				uint8_t* mapforImg = nullptr;	// image->pixelsと同じ型にする
				auto result = uploadBuff->Map(0, nullptr, reinterpret_cast<void**>(&mapforImg));

				if (FAILED(result))
				{
					nsGameWindow::MessageBoxError(L"テクスチャのアップロードバッファのマップに失敗しました。");

					return false;
				}

				// バッファのrowPitchは256の倍数でなければならないため、アライメントされている。
				// そのままコピーすると、バッファのrowPitchと元データのrowPitchと差でずれてしまう。
				// そのため、1行ごとにコピーして行頭が合うようにする。

				auto srcAddress = image->pixels;

				for (int y = 0; y < image->height; y++)
				{
					// 1行ずつコピーする。
					std::copy_n(srcAddress, alignedRowPitch, mapforImg);
					// 行頭を合わせる。
					srcAddress += image->rowPitch;	// 元データは実際のrowPitch分だけ進める
					mapforImg += alignedRowPitch;	// バッファはアライメントされたrowPitch分だけ進める
				}

				uploadBuff->Unmap(0, nullptr);

				return true;
			}

			void CTexture::CopyToTextureBuffFromUploadBuff(
				ID3D12Resource* textureBuff,
				ID3D12Resource* uploadBuff,
				const DirectX::TexMetadata& metadata,
				const DirectX::Image* image,
				const size_t alignedRowPitch,
				CGraphicsEngine* graphicsEngine)
			{
				// グラフィックボード上のコピー元アドレス。
				D3D12_TEXTURE_COPY_LOCATION src = {};
				// コピー元（アップロード側）の設定。
				src.pResource = uploadBuff;	// 中間バッファ
				// アップロードバッファには、フットプリント（メモリ占有領域に関する情報）指定。
				src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
				src.PlacedFootprint.Offset = 0;
				src.PlacedFootprint.Footprint.Width = static_cast<UINT>(metadata.width);
				src.PlacedFootprint.Footprint.Height = static_cast<UINT>(metadata.height);
				src.PlacedFootprint.Footprint.Depth = static_cast<UINT>(metadata.depth);
				// RowPitchは256の倍数でなければならない。
				src.PlacedFootprint.Footprint.RowPitch = static_cast<UINT>(alignedRowPitch);
				src.PlacedFootprint.Footprint.Format = image->format;

				// グラフィックボード上のコピー先アドレス。
				D3D12_TEXTURE_COPY_LOCATION dst = {};
				// コピー先（テクスチャバッファ）の設定。
				dst.pResource = textureBuff;
				// テクスチャバッファには、インデックス指定。
				dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
				dst.SubresourceIndex = 0;

				auto commandAllocator = graphicsEngine->GetCommandAllocator();
				auto commandList = graphicsEngine->GetCommandList()->Get();
				auto commandQueue = graphicsEngine->GetCommandQueue();
				auto fence = graphicsEngine->GetFence();
				auto fenceVal = graphicsEngine->GetFenceVal();

				auto result = commandAllocator->Reset();
				result = commandList->Reset(commandAllocator, nullptr);
				commandList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

				// テクスチャ用リソースが、コピー先のままのため、テクスチャ用指定に変更する。
				D3D12_RESOURCE_BARRIER barrierDesc =
					CD3DX12_RESOURCE_BARRIER::Transition(
						textureBuff,
						D3D12_RESOURCE_STATE_COPY_DEST,
						D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
					);

				commandList->ResourceBarrier(1, &barrierDesc);
				commandList->Close();

				ID3D12CommandList* cmdLists[] = { commandList };
				commandQueue->ExecuteCommandLists(1, cmdLists);

				commandQueue->Signal(fence, ++(*fenceVal));
				if (fence->GetCompletedValue() != (*fenceVal))
				{
					auto eventH = CreateEvent(nullptr, false, false, nullptr);
					fence->SetEventOnCompletion((*fenceVal), eventH);
					WaitForSingleObject(eventH, INFINITE);
					CloseHandle(eventH);
				}


				return;
			}

			void CTexture::CopyTextureParam(ID3D12Resource* texture)
			{
				m_texture = texture;
				m_texture->AddRef();
				const auto& desc = m_texture->GetDesc();
				m_mipLevels = desc.MipLevels;
				m_format = desc.Format;
				m_textureSize =
				{ static_cast<float>(desc.Width), static_cast<float>(desc.Height) };

				return;
			}



			void CTexture::SetDefaultName(const char* filePath)
			{
#ifdef _DEBUG
				std::wstring wstr(nsUtils::GetWideStringFromString(filePath));

				SetName(wstr.c_str());
#endif
				return;
			}

		}
	}
}