#include "YonemaEngine/Windows/Window.h"

/**
 * @brief 頂点データ構造体
*/
struct SVertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 uv;
};

/**
 * @brief 簡易テクスチャデータ生成用構造体
*/
struct STexRGBA
{
	unsigned char R = 0;
	unsigned char G = 0;
	unsigned char B = 0;
	unsigned char A = 0;
};

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

/**
 * @brief 定数バッファで送るためのマトリクスデータ
*/
struct SSceneMatrix
{
	DirectX::XMMATRIX mWorldViewProj = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX mWorld = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX mView = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX mProj = DirectX::XMMatrixIdentity();
	DirectX::XMFLOAT3 cameraPosWS;
};


// 1バイトパッキングにして、アライメントによるサイズのずれを防ぐ。
// 処理効率は落ちるが、読み込むのは最初だけであり、さらに後で別の構造体にコピーするから
// あまり問題はないと思う。
#pragma pack(1)
/**
 * @brief PMDファイルのマテリアルデータ
*/
struct SPMDMaterial
{
	DirectX::XMFLOAT3 diffuse;
	float alpha = 0.0f;
	float specularity = 0.0f;
	DirectX::XMFLOAT3 specular;
	DirectX::XMFLOAT3 ambient;
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
	DirectX::XMFLOAT3 diffuse;
	float alpha = 0.0f;
	DirectX::XMFLOAT3 specular;
	float specularity = 0.0f;
	DirectX::XMFLOAT3 ambient;
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

/**
 * @brief コンソール画面にフォーマット付き文字列を表示
 * @param format フォーマット（%dとか%fとかの）付き文字列
 * @param  可変長引数
 * @remarks この関数はデバック用です。デバック時にしか動作しません。
*/
void DebugOutputFormatString(const char* format, ...)
{
#ifdef _DEBUG
	va_list valist;
	va_start(valist, format);
	vprintf(format, valist);
	va_end(valist);
#endif

	return;
}

std::unordered_map<std::string, ID3D12Resource*> g_resourceTable;

/**
 * @brief アライメントにそろえたサイズを返す。
 * @param[in] size 元のサイズ
 * @param[in] alignment アライメントサイズ
 * @return アライメントをそろえたサイズ
*/
size_t AlignmentedSize(size_t size, size_t alignment)
{
	return size + alignment - size % alignment;
}


/**
 * @brief モデルのパスとテクスチャのパスから合成パスを得る
 * @param[in] modelPath アプリケーションから見たpmdモデルのパス
 * @param[in] texPath pmdモデルから見たテクスチャのパス
 * @return アプリケーションから見たテクスチャのパス
*/
std::string GetTexturePathFromModelAndTexPath(
	const std::string& modelPath,
	const char* const texPath
)
{
	// rfind()が、戻り値site_tのくせにsize_tで受け取ると、
	// 文字が見つからなかったときに-1（負の数）で受け取れない。
	// だから、キャストしてintで受け取る。
	int pathIndex1 = static_cast<int>(modelPath.rfind('/'));
	int pathIndex2 = static_cast<int>(modelPath.rfind('\\'));

	// '/'か'\\'のどちらか有効なほうを採用する。
	auto pathIndex = max(pathIndex1, pathIndex2);

	// そのままだと最後の'/'が入らないから、一個進める。
	pathIndex++;
	auto folderPath = modelPath.substr(0, pathIndex);

	return folderPath + texPath;
}

/**
 * @brief ファイル名から拡張子を取得する。
 * @param[in] path 対象のファイル名（ファイルパスでも可）
 * @return 拡張子
*/
std::string GetExtension(const std::string& fileName)
{
	int idx = static_cast<int>(fileName.rfind('.'));

	// そのままだと'.'が入ってしまうから、一個進める。
	idx++;
	return fileName.substr(idx, fileName.length() - idx);
}

/**
 * @brief ファイル名をセパレータ文字で分離する
 * @param[in] path 対象のファイル名（ファイルパスは不可）
 * @param[in] spliter 区切り文字
 * @return 分離前後の文字列ペア
*/
std::pair<std::string, std::string> SplitFilename(
	const std::string& texPath,
	const char spliter = '*'
)
{
	int idx = static_cast<int>(texPath.find(spliter));

	std::pair<std::string, std::string> ret;
	ret.first = texPath.substr(0, idx);
	// '*'を含めないために一個進める。
	idx++;
	ret.second = texPath.substr(idx, texPath.length() - idx);

	return ret;
}

/**
 * @brief strからwstrinに変換する
 * @param[in] str 変換するstr文字列
 * @return 変換されたwstr文字列
*/
std::wstring GetWideStringFromString(const std::string& str)
{
	// 呼び出し1回目
	// 文字数を得るため。
	auto num1 = MultiByteToWideChar(
		CP_ACP,
		MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
		str.c_str(),
		-1,
		nullptr,
		0
	);

	// 得られた文字数でリサイズ。
	std::wstring wstr;
	wstr.resize(num1);

	// 呼び出し2回目
	// 確保済みのwstrに変換文字列をコピー。
	auto num2 = MultiByteToWideChar(
		CP_ACP,
		MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
		str.c_str(),
		-1,
		&wstr[0],
		num1
	);

	// 一応チェック。
	assert(num1 == num2);

	return wstr;
}

/**
 * @brief テクスチャのロード
 * @param[in] texPath テクスチャのファイルパス
 * @return テクスチャのリソース。ロードできなければnullptr。
*/
ID3D12Resource* LoadTextureFromFile(
	std::string& texPath,
	ID3D12Device* const dev,
	ID3D12GraphicsCommandList* const cmdList,
	ID3D12CommandAllocator* const cmdAllocator,
	ID3D12CommandQueue* const cmdQueue,
	ID3D12Fence* const fence,
	UINT64* const fenceVal
)
{

	auto it = g_resourceTable.find(texPath);
	if (it != g_resourceTable.end())
	{
		return it->second;
	}

	// WICテクスチャのロード

	// 画像ファイルに関する情報。
	DirectX::TexMetadata metadata = {};
	// 実際のデータ。
	DirectX::ScratchImage scratchImg = {};

	auto extension = GetExtension(texPath);

	HRESULT result;

	if (extension == "sph" ||
		extension == "spa" ||
		extension == "bmp" ||
		extension == "png" ||
		extension == "jpg")
	{
		result = DirectX::LoadFromWICFile(
			GetWideStringFromString(texPath).c_str(),
			DirectX::WIC_FLAGS_NONE,
			&metadata,
			scratchImg
		);
	}
	else if (extension == "tga")
	{
		result = DirectX::LoadFromTGAFile(
			GetWideStringFromString(texPath).c_str(),
			&metadata,
			scratchImg
		);
	}
	else if (extension == "dds")
	{
		result = DirectX::LoadFromDDSFile(
			GetWideStringFromString(texPath).c_str(),
			DirectX::DDS_FLAGS_NONE,
			&metadata,
			scratchImg
		);
	}
	else
	{
		// 対応した拡張子が見つからない。
		DebugOutputFormatString("テクスチャのロードに失敗しました。");
		DebugOutputFormatString("対応した拡張子が見つかりません。");
		DebugOutputFormatString(texPath.c_str());
		return nullptr;
	}


	if (FAILED(result))
	{
		// テクスチャのロード失敗。
		DebugOutputFormatString("テクスチャのロードに失敗しました。");
		DebugOutputFormatString(texPath.c_str());
		return nullptr;
	}

	auto img = scratchImg.GetImage(0, 0, 0);	// 生データ抽出

	// バッファ用256の倍数でアライメントされたrowPitch。
	const auto alignmentedRowPitch = 
		AlignmentedSize(img->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);

	// 1.アップロード用リソースの作成

	// 中間バッファとしてのアップロードヒープの設定。
	D3D12_HEAP_PROPERTIES uploadHeapProp =
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	// リソースの設定。
	UINT64 resDescWidth = static_cast<UINT64>(alignmentedRowPitch * img->height);
	D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(resDescWidth);

	// 中間バッファの作成。
	ID3D12Resource* uploadBuff = nullptr;
	result = dev->CreateCommittedResource(
		&uploadHeapProp,
		D3D12_HEAP_FLAG_NONE,	// 特になし
		&resDesc,
		// CPUからマップするためリソースステートを、CPUから描き込み可能だがGPUからは読み取りのみにする。
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&uploadBuff)
	);

	if (FAILED(result))
	{
		// テクスチャのアップロードリソースの生成失敗。
		DebugOutputFormatString("テクスチャのアップロードリソースの生成に失敗しました。");
		return nullptr;
	}

	// 2.読み出し用リソースの作成
	
	// テクスチャのためのヒープ設定。
	D3D12_HEAP_PROPERTIES texHeapProp =
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);	// テクスチャ用

	// リソース設定（変数はつかいまわし）
	resDesc.Format = metadata.format;
	resDesc.Width = static_cast<UINT64>(metadata.width);
	resDesc.Height = static_cast<UINT>(metadata.height);
	resDesc.DepthOrArraySize = static_cast<UINT16>(metadata.arraySize);
	resDesc.MipLevels = static_cast<UINT16>(metadata.mipLevels);
	resDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;	// レイアウトは決定しない

	// テクスチャバッファの作成。
	ID3D12Resource* texBuff = nullptr;
	result = dev->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE,	// 特に指定なし
		&resDesc,
		D3D12_RESOURCE_STATE_COPY_DEST, // コピー先
		nullptr,
		IID_PPV_ARGS(&texBuff)
	);


	if (FAILED(result))
	{
		// テクスチャバッファの生成失敗。
		DebugOutputFormatString("テクスチャバッファの生成に失敗しました。");

		uploadBuff->Release();
		return nullptr;
	}


	// 3.アップロード用リソースへテクスチャデータをMap()でコピー

	uint8_t* mapforImg = nullptr;	// image->pixelsと同じ型にする
	result = uploadBuff->Map(0, nullptr, reinterpret_cast<void**>(&mapforImg));

	// バッファのrowPitchは256の倍数でなければならないため、アライメントされている。
	// そのままコピーすると、バッファのrowPitchと元データのrowPitchと差でずれてしまう。
	// そのため、1行ごとにコピーして行頭が合うようにする。

	auto srcAddress = img->pixels;

	for (int y = 0; y < img->height; y++)
	{
		// 1行ずつコピーする。
		std::copy_n(srcAddress, alignmentedRowPitch, mapforImg);
		// 行頭を合わせる。
		srcAddress += img->rowPitch;	// 元データは実際のrowPitch分だけ進める
		mapforImg += alignmentedRowPitch;	// バッファはアライメントされたrowPitch分だけ進める
	}

	uploadBuff->Unmap(0, nullptr);


	// 4.アップロード用リソースから読み出し用リソースへCopyTextureRegion()でコピー

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
	src.PlacedFootprint.Footprint.RowPitch = static_cast<UINT>(alignmentedRowPitch);
	src.PlacedFootprint.Footprint.Format = img->format;

	// グラフィックボード上のコピー先アドレス。
	D3D12_TEXTURE_COPY_LOCATION dst = {};
	// コピー先（テクスチャバッファ）の設定。
	dst.pResource = texBuff;
	// テクスチャバッファには、インデックス指定。
	dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	dst.SubresourceIndex = 0;

	result = cmdAllocator->Reset();
	// 再びコマンドを溜める準備。
	result = cmdList->Reset(cmdAllocator, nullptr);

	cmdList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

	// テクスチャ用リソースが、コピー先のままのため、テクスチャ用指定に変更する。
	D3D12_RESOURCE_BARRIER barrierDesc = 
		CD3DX12_RESOURCE_BARRIER::Transition(
			texBuff,
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
		);

	cmdList->ResourceBarrier(1, &barrierDesc);
	cmdList->Close();

	ID3D12CommandList* cmdLists[] = { cmdList };
	cmdQueue->ExecuteCommandLists(1, cmdLists);

	cmdQueue->Signal(fence, ++(*fenceVal));
	if (fence->GetCompletedValue() != (*fenceVal))
	{
		auto eventH = CreateEvent(nullptr, false, false, nullptr);
		fence->SetEventOnCompletion((*fenceVal), eventH);
		WaitForSingleObject(eventH, INFINITE);
		CloseHandle(eventH);
	}

	// コピーしたからもういらない。
	uploadBuff->Release();

	g_resourceTable.emplace(texPath, texBuff);

	return texBuff;

}


/**
 * @brief DirectX12のデバックレイヤーを有効化
*/
void EnableDebugLayer()
{
	ID3D12Debug* debugLayer = nullptr;
	auto result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));

	// デバックレイヤーを有効にする。
	debugLayer->EnableDebugLayer();
	// 有効にしたら、インターフェースを開放する。
	debugLayer->Release();

	return;
}

#ifdef _DEBUG
int main()
{
#else
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
#endif


	constexpr int kWindowWidth = 1280;
	constexpr int kWindowHeight = 720;

	nsYMEngine::nsWindows::CWindows window(kWindowWidth, kWindowHeight);




#ifdef _DEBUG
	// デバックレイヤーをオンに。
	EnableDebugLayer();
#endif


	ID3D12Device* _dev = nullptr;
	IDXGIFactory6* _dxgiFactory = nullptr;
	IDXGISwapChain4* _swapChain = nullptr;


	// 〇アダプターの特定


	HRESULT result;
	// DXGIFactoryオブジェクトの生成。
#ifdef _DEBUG
	// DXGI周りのエラーメッセージを調べるため。
	result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&_dxgiFactory));
#else
	result = CreateDXGIFactory1(IID_PPV_ARGS(&_dxgiFactory));
#endif

	// ここに特定の名前を持つアダプターオブジェクトが入る。	
	IDXGIAdapter* tmpAdapter = nullptr;

	for (int i = 0; _dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; i++)
	{
		DXGI_ADAPTER_DESC adesc = {};
		tmpAdapter->GetDesc(&adesc);

		std::wstring strDesc = adesc.Description;

		// 探したいアダプターの名前を確認。
		if (strDesc.find(L"NVIDIA") != std::string::npos)
		{
			break;
		}
		else
		{
			tmpAdapter->Release();
			tmpAdapter = nullptr;
		}
	}


	// 〇D3D12デバイスの生成

	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	//D3D_FEATURE_LEVEL featureLevel;

	// デバイス生成が可能なフィーチャーレベルを探す。
	for (auto lv : levels)
	{
		// Direct3Dデバイスの生成
		auto hr = D3D12CreateDevice(
			tmpAdapter,	// nullptrだと自動でアダプター（グラフィックドライバー）が選択される。
			lv,			// 機能レベル。選択したドライバが対応していなかったら失敗する。
			IID_PPV_ARGS(&_dev)
		);

		if (SUCCEEDED(hr))
		{
			// 生成可能なバージョンを見つけたら、ループを打ち切り。
			break;
		}

		// どれもダメだったらデバイスがnullptrになるから、そこでアプリケーションを終了させる。
	}

	if (_dev == nullptr)
	{
		// デバイスの生成失敗。
		DebugOutputFormatString("デバイスの生成に失敗しました。");
	}


	// 〇コマンドアロケータの生成
	ID3D12CommandAllocator* _cmdAllocator = nullptr;
	result = _dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdAllocator));

	if (FAILED(result))
	{
		// コマンドアロケータの生成失敗。
		DebugOutputFormatString("コマンドアロケータの生成に失敗しました。");
	}

	// 〇コマンドリストの生成
	ID3D12GraphicsCommandList* _cmdList = nullptr;
	result = _dev->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		_cmdAllocator,
		nullptr,
		IID_PPV_ARGS(&_cmdList)
	);

	if (FAILED(result))
	{
		// コマンドリストの生成失敗。
		DebugOutputFormatString("コマンドリストの生成に失敗しました。");
	}

	// コマンドリストは、開かれている状態で生成されるため、閉じておく。

	// 画像をGPUにコピーするときにコマンドリストを使用するため、閉じない。
	// 通常はループ内でコピーされるため、閉じないのは今回の場合だけ。
	// 後で直す。
	_cmdList->Close();

	// 〇コマンドキューの生成
	ID3D12CommandQueue* _cmdQueue = nullptr;

	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
	// タイムアウトなし。
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	// アダプターを1つしか使わない時は、0でよい。
	cmdQueueDesc.NodeMask = 0;
	// プライオリティは特に指定なし。
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	// コマンドリストと合わせる。
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	// キューの作成
	result = _dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&_cmdQueue));
	if (FAILED(result))
	{
		// コマンドキューの作成失敗。
		DebugOutputFormatString("コマンドキューの生成に失敗しました。");
	}

	// 〇スワップチェーンの生成
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {  };
	swapChainDesc.Width = kWindowWidth;		// 画像解像度の幅
	swapChainDesc.Height = kWindowHeight;	// 画像解像度の高さ
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// ピクセルフォーマット
	swapChainDesc.Stereo = false;			// ステレオ表示フラグ（3Dディスプレイのステレオモード）
	swapChainDesc.SampleDesc.Count = 1;		// マルチサンプルの指定。
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;			// ダブルバッファ―なら2
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;	// バックバッファは伸び縮み可能
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	// フリップ後は速やかに破棄
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;	// 特に指定なし
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// ウィンドウ←→フルスクリーン切り替え可能

	result = _dxgiFactory->CreateSwapChainForHwnd(
		_cmdQueue,
		window.GetHWND(),
		&swapChainDesc,
		nullptr,
		nullptr,
		reinterpret_cast<IDXGISwapChain1**>(&_swapChain)
	);

	if (FAILED(result))
	{
		// スワップチェーンの作成失敗
		DebugOutputFormatString("スワップチェーンの生成に失敗しました。");
	}

	// 〇レンダーターゲットビューの作成
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	// レンダーターゲットビューのため。
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heapDesc.NodeMask = 0;
	// 裏表の2つ。
	heapDesc.NumDescriptors = 2;
	// 特に指定なし。
	// シェーダ―から参照する必要がないため。
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	ID3D12DescriptorHeap* rtvHeaps = nullptr;
	result = _dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeaps));

	if (FAILED(result))
	{
		// レンダーターゲットビューの作成失敗
		DebugOutputFormatString("レンダーターゲットビューの生成に失敗しました。");
	}

	// 〇スワップチェーンとメモリを紐づける。

	// バックバッファの数を調べるために、スワップチェインからディスクリプタを取ってくる。
	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	result = _swapChain->GetDesc(&swcDesc);
	if (FAILED(result))
	{
		// スワップチェインの取得失敗
		DebugOutputFormatString("スワップチェーンの取得に失敗しました。");
	}

	// sRGBレンダーターゲットビューの設定。
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	//rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	// ガンマ補正あり（sRGB）
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// ガンマ補正あり（sRGB）
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	std::vector<ID3D12Resource*> _backBuffers(swcDesc.BufferCount);
	for (int idx = 0; static_cast<unsigned int>(idx) < swcDesc.BufferCount; idx++)
	{
		// スワップチェイン内のバッファとビューを関連付け。
		result = _swapChain->GetBuffer(idx, IID_PPV_ARGS(&_backBuffers[idx]));

		// 先頭アドレスのハンドルを取得。
		D3D12_CPU_DESCRIPTOR_HANDLE handle = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
		// 複数のディスクリプタを取得するため、アドレスをずらす。
		handle.ptr += static_cast<unsigned long long int>(idx) *
			_dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		// レンダーターゲットビューを作成。
		_dev->CreateRenderTargetView(_backBuffers[idx], &rtvDesc, handle);
	}


	// 〇フェンスの生成
	ID3D12Fence* _fence = nullptr;
	UINT64 _fenceVal = 0;
	result = _dev->CreateFence(_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
	if (FAILED(result))
	{
		// フェンスの生成失敗。
		DebugOutputFormatString("フェンスの生成に失敗しました。");
	}

	// 〇深度バッファの作成
	D3D12_RESOURCE_DESC depthResDesc = {};
	depthResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResDesc.Width = kWindowWidth;
	depthResDesc.Height = kWindowHeight;
	depthResDesc.DepthOrArraySize = 1;
	depthResDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthResDesc.SampleDesc.Count = 1;
	depthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	// 深度値用ヒーププロパティ
	D3D12_HEAP_PROPERTIES depthHeapProp = {};
	// DEFAULTのため、後の設定はUNKNOWNでよい。
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	depthHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	depthHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	// このクリアバリューが重要な意味を持つ
	D3D12_CLEAR_VALUE depthClearValue = CD3DX12_CLEAR_VALUE(
		DXGI_FORMAT_D32_FLOAT,
		1.0f,
		0
	);

	ID3D12Resource* depthBuffer = nullptr;
	result = _dev->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(&depthBuffer)
	);

	if (FAILED(result))
	{
		// 深度ステンシルバッファの生成失敗。
		DebugOutputFormatString("深度ステンシルバッファの生成に失敗しました。");
	}

	// 〇深度バッファビューの作成
	
	// ディスクリプタヒープの作成
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	ID3D12DescriptorHeap* dsvHeap = nullptr;
	result = _dev->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));

	if (FAILED(result))
	{
		// 深度バッファビュー用のディスクリプタヒープの生成失敗。
		DebugOutputFormatString("深度バッファビュー用のディスクリプタヒープの生成に失敗しました。");
	}

	// 深度ビューの作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	_dev->CreateDepthStencilView(
		depthBuffer,
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart()
	);




	// 初期化
	result = CoInitializeEx(0, COINIT_MULTITHREADED);

	std::string whiteTexFilePath = "Assets/Models/white.jpg";
	auto whiteTex = LoadTextureFromFile(
		whiteTexFilePath,
		_dev,
		_cmdList,
		_cmdAllocator,
		_cmdQueue,
		_fence,
		&_fenceVal
	);
	std::string blackTexFilePath = "Assets/Models/black.jpg";
	auto blackTex = LoadTextureFromFile(
		blackTexFilePath,
		_dev,
		_cmdList,
		_cmdAllocator,
		_cmdQueue,
		_fence,
		&_fenceVal
	);

	// 〇PMDファイルの読み込み

	FILE* fileStream = nullptr;

	unsigned int pmdVertNum = 0;
	std::vector<unsigned char> pmdVertices;
	constexpr size_t kPmdVertexSize = 38;

	unsigned int pmdIndicesNum = 0;
	std::vector<unsigned short> pmdIndices;
	size_t pmdIndicesSize = 0;

	unsigned int pmdMaterialNum = 0;
	std::vector<SPMDMaterial> pmdMaterials;
	size_t pmdMaterialSize = 0;

	//std::string strModelPath = "Assets/Models/Samples/初音ミク.pmd";
	std::string strModelPath = "Assets/Models/Samples/初音ミクmetal.pmd";
	//std::string strModelPath = "Assets/Models/Samples/巡音ルカ.pmd";

	if (fopen_s(&fileStream, strModelPath.c_str(), "rb") == 0)
	{
		// ヘッダの読み込み
		char signature[3] = {};
		SPMDHeader pmdHeader = {};
		fread_s(signature, sizeof(signature), sizeof(signature), 1, fileStream);
		fread_s(&pmdHeader, sizeof(pmdHeader), sizeof(pmdHeader), 1, fileStream);

		// 頂点数の読み込み
		fread_s(&pmdVertNum, sizeof(pmdVertNum), sizeof(pmdVertNum), 1, fileStream);

		// 頂点データの読み込み
		pmdVertices.resize(pmdVertNum * kPmdVertexSize);
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


		fclose(fileStream);
	}
	else
	{
		// PMDファイルの読み込み失敗。
		DebugOutputFormatString("PMDファイルの読み込みに失敗しました。");
	}

	// 読み込んだマテリアルデータを転送用のマテリアルデータにコピーする。
	std::vector<SMaterial> materials(pmdMaterials.size());
	std::vector<ID3D12Resource*> textureResources(pmdMaterials.size());
	std::vector<ID3D12Resource*> sphResources(pmdMaterials.size());
	std::vector<ID3D12Resource*> spaResources(pmdMaterials.size());

	for (int i = 0; i < pmdMaterials.size(); i++)
	{
		// マテリアルデータのコピー。
		materials[i].indicesNum = pmdMaterials[i].indicesNum;
		materials[i].matForHlsl.diffuse = pmdMaterials[i].diffuse;
		materials[i].matForHlsl.alpha = pmdMaterials[i].alpha;
		materials[i].matForHlsl.specular = pmdMaterials[i].specular;
		materials[i].matForHlsl.specularity = pmdMaterials[i].specularity;
		materials[i].matForHlsl.ambient = pmdMaterials[i].ambient;

		// テクスチャのロード

		// 最初に全部のリソースにnullptrを入れておく。
		// リソースがあるなら、あとから入れる。
		textureResources[i] = nullptr;
		sphResources[i] = nullptr;
		spaResources[i] = nullptr;

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

			auto namePair = SplitFilename(texFileName);
			std::string otherTexFileName;

			// スフィアマップじゃないほうをテクスチャ名にする。
			if (GetExtension(namePair.first) == "sph" ||
				GetExtension(namePair.first) == "spa")
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

			auto otherTexFilePath = GetTexturePathFromModelAndTexPath(
				strModelPath,
				otherTexFileName.c_str()
			);

			auto res = LoadTextureFromFile(
				otherTexFilePath,
				_dev,
				_cmdList,
				_cmdAllocator,
				_cmdQueue,
				_fence,
				&_fenceVal
			);

			if (GetExtension(otherTexFileName) == "sph")
			{
				sphResources[i] = res;
			}
			else
			{
				spaResources[i] = res;
			}
		}

		auto texFilePath = GetTexturePathFromModelAndTexPath(
			strModelPath,
			texFileName.c_str()
		);

		auto res = LoadTextureFromFile(
			texFilePath,
			_dev,
			_cmdList,
			_cmdAllocator,
			_cmdQueue,
			_fence,
			&_fenceVal
		);

		if (GetExtension(texFileName) == "sph")
		{
			sphResources[i] = res;
		}
		else if (GetExtension(texFileName) == "spa")
		{
			spaResources[i] = res;
		}
		else
		{
			textureResources[i] = res;
		}

	}


	// ワールド行列計算
	DirectX::XMMATRIX mWorld = DirectX::XMMatrixRotationY(DirectX::XM_PIDIV4);

	// ビュー行列計算
	DirectX::XMFLOAT3 eyePos(0.0f, 10.0f, -15.0f);
	DirectX::XMFLOAT3 targetPos(0.0f, 10.0f, 0.0f);
	DirectX::XMFLOAT3 upVec(0.0f, 1.0f, 0.0f);
	DirectX::XMMATRIX mView = DirectX::XMMatrixLookAtLH(
		DirectX::XMLoadFloat3(&eyePos),
		DirectX::XMLoadFloat3(&targetPos),
		DirectX::XMLoadFloat3(&upVec)
	);

	// プロジェクション行列計算
	DirectX::XMMATRIX mProj = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XM_PIDIV2,
		static_cast<float>(kWindowWidth) / static_cast<float>(kWindowHeight),
		1.0f,
		100.0f
	);

	

	// 〇定数バッファの生成。
	ID3D12Resource* constBuff = nullptr;
	// Mapで中身を書き換えるためUPLOAD。
	D3D12_HEAP_PROPERTIES constBuffHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// 必要なバイト数を256でアライメントしたバイト数が必要。
	// 0xff = 255
	// 最初に0xffを足して256以上にしてから、ビット反転した0xffとAND演算することで、
	// sizeを超えた最小の256の倍数を計算する。
	UINT64 constBuffSize = (sizeof(SSceneMatrix) + 0xff) & ~0xff;
	D3D12_RESOURCE_DESC constBuffResDesc = CD3DX12_RESOURCE_DESC::Buffer(constBuffSize);
	result = _dev->CreateCommittedResource(
		&constBuffHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&constBuffResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff)
	);

	if (FAILED(result))
	{
		// 定数バッファの生成失敗。
		DebugOutputFormatString("定数バッファの生成に失敗しました。");
	}

	// 定数バッファのコピー
	SSceneMatrix* mapMatrix = nullptr;	// マップ先のポインター
	result = constBuff->Map(0, nullptr, reinterpret_cast<void**>(&mapMatrix));	// マップ
	// 行列の内容をコピー。
	mapMatrix->mWorldViewProj = mWorld * mView * mProj;
	mapMatrix->mWorld = mWorld;
	mapMatrix->mView = mView;
	mapMatrix->mProj = mProj;
	mapMatrix->cameraPosWS = eyePos;

	// 〇定数バッファビュー
	ID3D12DescriptorHeap* basicDescHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	// シェーダーから見えるように。
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	// マスクは0。
	descHeapDesc.NodeMask = 0;
	// ビューは今のところ1つだけ。
	// SRVとCBVの2つ。
	descHeapDesc.NumDescriptors = 2;
	// シェーダーリソースビュー用。
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	// 生成。
	result = _dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&basicDescHeap));
	if (FAILED(result))
	{
		// ディスクリプタヒープの生成失敗。
		DebugOutputFormatString("ディスクリプタヒープの生成に失敗しました。");
	}

	// ディスクリプタヒープの先頭アドレスを取得。
	// 先頭アドレスはシェーダーリソースビュー。
	auto basicHeapHandle = basicDescHeap->GetCPUDescriptorHandleForHeapStart();

	// 〇定数バッファビューの生成。

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = constBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = static_cast<UINT>(constBuff->GetDesc().Width);

	// ビューの作成。
	_dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);


	// 〇頂点バッファの作成

	// UPLOADヒープとしての設定
	D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	// サイズに応じて適切な設定をしてくれる。
	D3D12_RESOURCE_DESC resdesc = CD3DX12_RESOURCE_DESC::Buffer(pmdVertices.size());

	ID3D12Resource* vertBuff = nullptr;
	result = _dev->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff)
	);

	if (FAILED(result))
	{
		// 頂点バッファの生成失敗。
		DebugOutputFormatString("頂点バッファの生成に失敗しました。");
	}

	// 〇頂点情報のコピー（マップ）
	unsigned char* vertMap = nullptr;
	// GPUのバッファーの（仮想）アドレスを取得。
	result = vertBuff->Map(0, nullptr, reinterpret_cast<void**>(&vertMap));
	// マップしてきたバッファーに、頂点データを書き込む。
	std::copy(std::begin(pmdVertices), std::end(pmdVertices), vertMap);
	// マップ解除。
	vertBuff->Unmap(0, nullptr);

	// 〇頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();	// バッファーの仮想アドレス
	vbView.SizeInBytes = static_cast<UINT>(pmdVertices.size());	// 全バイト数
	vbView.StrideInBytes = static_cast<UINT>(kPmdVertexSize);	// 1頂点あたりのバイト数

	// 〇インデックスバッファの作成

	unsigned short indeices[] =
	{
		// 時計回り
		0,1,2,
		2,1,3
	};

	ID3D12Resource* idxBuff = nullptr;
	// 設定は、バッファのサイズ以外、頂点バッファの設定を使いまわしてよい。
	resdesc.Width = pmdIndicesSize;

	result = _dev->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&idxBuff)
	);
	if (FAILED(result))
	{
		// インデックスバッファの生成失敗。
		DebugOutputFormatString("インデックスバッファの生成に失敗しました。");
	}

	// 作ったバッファーにインデックスデータをコピー
	unsigned short* mappedIdx = nullptr;
	idxBuff->Map(0, nullptr, reinterpret_cast<void**>(&mappedIdx));
	std::copy(std::begin(pmdIndices), std::end(pmdIndices), mappedIdx);
	idxBuff->Unmap(0, nullptr);

	// インデックスバッファビューを作成
	D3D12_INDEX_BUFFER_VIEW ibView = {};
	ibView.BufferLocation = idxBuff->GetGPUVirtualAddress();
	// unsigned int でインデックス配列を使用しているため、DXGI_FORMAT_R16_UINTを使用。
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = static_cast<UINT>(pmdIndicesSize);


	// 〇マテリアルバッファの作成
	UINT materialBuffSize = sizeof(SMaterialForHlsl);
	materialBuffSize = (materialBuffSize + 0xff) & ~0xff;

	ID3D12Resource* materialBuff = nullptr;
	D3D12_HEAP_PROPERTIES matHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// もったいないが仕方がない。
	D3D12_RESOURCE_DESC matResDesc = 
		CD3DX12_RESOURCE_DESC::Buffer(
			static_cast<UINT64>(materialBuffSize) * static_cast<UINT64>(pmdMaterialNum)
		);
	result = _dev->CreateCommittedResource(
		&matHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&matResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&materialBuff)
	);

	// マップマテリアルにコピー
	char* mapMaterial = nullptr;
	result = materialBuff->Map(0, nullptr, reinterpret_cast<void**>(&mapMaterial));
	for (auto& m : materials)
	{
		// データのコピー
		*reinterpret_cast<SMaterialForHlsl*>(mapMaterial) = m.matForHlsl;
		// 次のアライメント位置まで進める。（256の倍数）
		mapMaterial += materialBuffSize;
	}
	materialBuff->Unmap(0, nullptr);

	// マテリアル用ディスクリプタヒープとビューの作成
	ID3D12DescriptorHeap* matDescHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC matDescHeapDesc = {};
	matDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	matDescHeapDesc.NodeMask = 0;
	// マテリアルデータとテクスチャ*3の4つ分。
	matDescHeapDesc.NumDescriptors = pmdMaterialNum * 4;
	matDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	result = _dev->CreateDescriptorHeap(
		&matDescHeapDesc,
		IID_PPV_ARGS(&matDescHeap)
	);

	if (FAILED(result))
	{
		// マテリアル用のディスクリプタヒープの生成失敗。
		DebugOutputFormatString("マテリアル用のディスクリプタヒープの生成に失敗しました。");
	}

	D3D12_CONSTANT_BUFFER_VIEW_DESC matCBVDesc = {};
	matCBVDesc.BufferLocation = materialBuff->GetGPUVirtualAddress();
	matCBVDesc.SizeInBytes = materialBuffSize;

	D3D12_SHADER_RESOURCE_VIEW_DESC matSRVDesc = {};
	matSRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	matSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	matSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	matSRVDesc.Texture2D.MipLevels = 1;


	{
		// 先頭を記録
		auto matDescHeapH = matDescHeap->GetCPUDescriptorHandleForHeapStart();
		const auto inc = 
			_dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		for (int i = 0; i < static_cast<int>(pmdMaterialNum); i++)
		{
			// マテリアルデータ用定数バッファビュー。
			_dev->CreateConstantBufferView(&matCBVDesc, matDescHeapH);

			matDescHeapH.ptr += inc;
			matCBVDesc.BufferLocation += materialBuffSize;

			// テクスチャ用シェーダーリソースビュー。
			if (textureResources[i] == nullptr)
			{
				matSRVDesc.Format = whiteTex->GetDesc().Format;
				_dev->CreateShaderResourceView(
					whiteTex,
					&matSRVDesc,
					matDescHeapH
				);
			}
			else
			{
				matSRVDesc.Format = textureResources[i]->GetDesc().Format;
				_dev->CreateShaderResourceView(
					textureResources[i],
					&matSRVDesc,
					matDescHeapH
				);
			}

			matDescHeapH.ptr += inc;

			if (sphResources[i] == nullptr)
			{
				matSRVDesc.Format = whiteTex->GetDesc().Format;
				_dev->CreateShaderResourceView(
					whiteTex,
					&matSRVDesc,
					matDescHeapH
				);
			}
			else
			{
				matSRVDesc.Format = sphResources[i]->GetDesc().Format;
				_dev->CreateShaderResourceView(
					sphResources[i],
					&matSRVDesc,
					matDescHeapH
				);
			}

			matDescHeapH.ptr += inc;

			if (spaResources[i] == nullptr)
			{
				matSRVDesc.Format = blackTex->GetDesc().Format;
				_dev->CreateShaderResourceView(
					blackTex,
					&matSRVDesc,
					matDescHeapH
				);
			}
			else
			{
				matSRVDesc.Format = spaResources[i]->GetDesc().Format;
				_dev->CreateShaderResourceView(
					spaResources[i],
					&matSRVDesc,
					matDescHeapH
				);
			}

			matDescHeapH.ptr += inc;

		}
	}



	// 〇シェーダ―の読み込みと生成

	ID3DBlob* _vsBlob = nullptr;
	ID3DBlob* _psBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	result = D3DCompileFromFile(
		L"Assets/Shaders/BasicVertexShader.hlsl",	// シェーダ―パス
		nullptr,							// defineはなし
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	// インクルード可能にしておく
		"BasicVS",							// エントリーポイント
		"vs_5_0",							// バージョン
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	// デバック用および最適化なし
		0,
		&_vsBlob,
		&errorBlob							// エラー時にメッセージが入る。なければnullptrが入る。
	);

	if (FAILED(result))
	{
		// 頂点シェーダ―の生成失敗。
		DebugOutputFormatString("頂点シェーダ―の生成に失敗しました。");

		// 頂点シェーダーのファイルが見つからない。
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		{
			::OutputDebugStringA("頂点シェーダーのファイルが見つかりません。");
		}
		// エラーメッセージの表示。
		else if (errorBlob)
		{
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n(
				static_cast<char*>(errorBlob->GetBufferPointer()),
				errorBlob->GetBufferSize(),
				errstr.begin()
				);
			errstr += "\n";

			::OutputDebugStringA(errstr.c_str());

			errorBlob->Release();
		}
	}

	result = D3DCompileFromFile(
		L"Assets/Shaders/BasicPixelShader.hlsl",	// シェーダ―パス
		nullptr,							// defineはなし
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	// インクルード可能にしておく
		"BasicPS",							// エントリーポイント
		"ps_5_0",							// バージョン
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	// デバック用および最適化なし
		0,
		&_psBlob,
		&errorBlob							// エラー時にメッセージが入る。なければnullptrが入る。
	);

	if (FAILED(result))
	{
		// ピクセルシェーダ―の生成失敗。
		DebugOutputFormatString("ピクセルシェーダ―の生成に失敗しました。");

		// ピクセルシェーダーのファイルが見つからない。
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		{
			::OutputDebugStringA("ピクセルシェーダーのファイルが見つかりません。");
		}
		// エラーメッセージの表示。
		else if (errorBlob)
		{
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n(
				static_cast<char*>(errorBlob->GetBufferPointer()),
				errorBlob->GetBufferSize(),
				errstr.begin()
			);
			errstr += "\n";

			::OutputDebugStringA(errstr.c_str());

			errorBlob->Release();
		}
	}

	// 〇頂点レイアウト
	//D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	//{
	//	// 座標情報
	//	{
	//		"POSITION",	// セマンティクス名
	//		0,
	//		DXGI_FORMAT_R32G32B32_FLOAT,	// フォーマット
	//		0,	// 入力スロットのインデックス
	//		D3D12_APPEND_ALIGNED_ELEMENT,	// データのオフセット位置
	//		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
	//		0	// 一度に描画するインスタンスの数
	//	},
	//	// UV
	//	{
	//		"TEXCOORD",
	//		0,
	//		DXGI_FORMAT_R32G32_FLOAT,
	//		0,
	//		D3D12_APPEND_ALIGNED_ELEMENT,
	//		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
	//		0
	//	}
	//};
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		// 座標情報
		{
			"POSITION",	// セマンティクス名
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,	// フォーマット
			0,	// 入力スロットのインデックス
			D3D12_APPEND_ALIGNED_ELEMENT,	// データのオフセット位置
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0	// 一度に描画するインスタンスの数
		},
		{
			"NORMAL",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		// UV
		{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		{
			"BONE_NO",
			0,
			DXGI_FORMAT_R16G16_UINT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		{
			"WEIGHT",
			0,
			DXGI_FORMAT_R8_UINT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		{
			"EDGE_FLG",
			0,
			DXGI_FORMAT_R8_UINT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		}
	};

	// 〇ルートシグネチャの生成
	ID3D12RootSignature* _rootsignature = nullptr;

	// ディスクリプタレンジ
	constexpr unsigned int descTableRangeNum = 3;
	D3D12_DESCRIPTOR_RANGE descTblRange[descTableRangeNum] = {};

	// 定数用レジスタ0番
	descTblRange[0] = CD3DX12_DESCRIPTOR_RANGE(
		D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
		1,
		0
	);

	// マテリアル用レジスタ1番
	// ディスクリプタヒープは複数あるが、一度に使うのは一つ。
	descTblRange[1] = CD3DX12_DESCRIPTOR_RANGE(
		D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
		1,
		1
	);

	// テクスチャ用（マテリアルとセット）レジスタ0番
	descTblRange[2] = CD3DX12_DESCRIPTOR_RANGE(
		D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
		3,
		0
	);

	// ルートパラメータの作成。
	constexpr unsigned int rootparamNum = 2;
	D3D12_ROOT_PARAMETER rootparam[rootparamNum] = {};
	CD3DX12_ROOT_PARAMETER::InitAsDescriptorTable(
		rootparam[0],
		1,
		&descTblRange[0],
		D3D12_SHADER_VISIBILITY_VERTEX
	);
	CD3DX12_ROOT_PARAMETER::InitAsDescriptorTable(
		rootparam[1],
		2,
		&descTblRange[1],
		D3D12_SHADER_VISIBILITY_PIXEL
	);


	// サンプラの設定。
	D3D12_STATIC_SAMPLER_DESC samplerDesc =
		CD3DX12_STATIC_SAMPLER_DESC(
			0,
			//D3D12_FILTER_MIN_MAG_MIP_LINEAR,	// 線形補完
			//D3D12_FILTER_MIN_MAG_MIP_POINT,	// 補間なし（ニアレストネイバー方）
			D3D12_FILTER_ANISOTROPIC,			// 異方性フィルタリング
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			0,
			16,
			//D3D12_COMPARISON_FUNC_LESS_EQUAL,
			D3D12_COMPARISON_FUNC_NEVER,	// リサンプリングしない
			D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK,
			0.0f,
			D3D12_FLOAT32_MAX,
			D3D12_SHADER_VISIBILITY_PIXEL,	// ピクセルシェーダーから見える
			0
		);

	// ルートシグネチャの設定。
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc =
		CD3DX12_ROOT_SIGNATURE_DESC(
			rootparamNum,
			rootparam,
			1,
			&samplerDesc,
			// 頂点情報（入力アセンブラ）がある。
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
		);

	// ルートシグネチャのバイナリコードの作成。
	ID3DBlob* rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob,
		&errorBlob
	);


	// ルートシグネチャオブジェクトの生成。
	result = _dev->CreateRootSignature(
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&_rootsignature)
	);

	// 不要になったため解放。
	rootSigBlob->Release();

	if (FAILED(result))
	{
		// ルートシグネチャの生成失敗。
		DebugOutputFormatString("ルートシグネチャの生成に失敗しました。");

		// エラーメッセージの表示。
		if (errorBlob)
		{
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());

			std::copy_n(
				static_cast<char*>(errorBlob->GetBufferPointer()),
				errorBlob->GetBufferSize(),
				errstr.begin()
			);
			errstr += "\n";

			::OutputDebugStringA(errstr.c_str());

			errorBlob->Release();
		}
	}


	// 〇グラフィックスパイプラインステートの作成

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = {};
	gpipeline.pRootSignature = _rootsignature;

	// 1.シェーダ―のセット。

	gpipeline.VS = CD3DX12_SHADER_BYTECODE(_vsBlob);
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(_psBlob);

	// 2.サンプルマスクとラスタライザステートの設定。
	// デフォルトのサンプルマスク。（0xffffffff）
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	D3D12_RASTERIZER_DESC rasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	gpipeline.RasterizerState = rasterizerDesc;


	// 3.ブレンドステートの設定。
	// 全体のブレンドステートの設定。
	D3D12_BLEND_DESC blendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	gpipeline.BlendState = blendDesc;

	// 4.入力レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = inputLayout;	// 入力レイアウトの戦闘アドレス
	gpipeline.InputLayout.NumElements = _countof(inputLayout);	// 入力レイアウトの要素数
	// トライアングルストリップの時に、特定のインデックスを切り離すための指定。
	gpipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;	// カットなし
	// 構成要素が「点」「線」「三角形」のどれかを指定。
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;	// 三角形で構成。

	// 5.レンダーターゲットの設定
	gpipeline.NumRenderTargets = 1;	// レンダーターゲットの数は今は1つ
	//gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	// レンダーターゲットのカラーフォーマット
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;	// レンダーターゲットのカラーフォーマット


	// 6.アンチエイリアシングのためのサンプル数指定
	// AAしない設定。
	gpipeline.SampleDesc.Count = 1;		// サンプル数は1ピクセルにつき1
	gpipeline.SampleDesc.Quality = 0;	// クオリティは最低

	// デプスステンシルバッファの設定
	gpipeline.DepthStencilState.DepthEnable = true;
	gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// 7.グラフィックスパイプラインステートオブジェクトの生成
	ID3D12PipelineState* _pipelinestate = nullptr;
	result = _dev->CreateGraphicsPipelineState(
		&gpipeline,
		IID_PPV_ARGS(&_pipelinestate)
	);

	if (FAILED(result))
	{
		// グラフィックスパイプラインステートの生成失敗。
		DebugOutputFormatString("グラフィックスパイプラインステートの生成に失敗しました。");
	}

	// 〇ビューポート
	D3D12_VIEWPORT viewport = CD3DX12_VIEWPORT(
		0.0f,
		0.0f,
		kWindowWidth,
		kWindowHeight
	);

	// 〇シザー矩形
	// ビューポートいっぱいに表示する。
	D3D12_RECT scissorRect = CD3DX12_RECT(
		0,
		0,
		0 + kWindowWidth,
		0 + kWindowHeight
	);

	float angle = 0.0f;

	// ウィンドウ表示
#ifdef _DEBUG
	ShowWindow(window.GetHWND(), SW_SHOW);
#else
	ShowWindow(hwnd, nCmdShow);
#endif

	MSG msg = {};

	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			break;
		}

		angle += 0.01f;
		auto qRot = DirectX::XMQuaternionRotationAxis({ 0.0f,1.0f,0.0f }, angle);
		auto vec = DirectX::XMVECTOR({ 0.0f, 20.0f, -15.0f });
		vec = DirectX::XMVector3Rotate(vec, qRot);
		//eyePos = { vec.m128_f32[0], vec.m128_f32[1], vec.m128_f32[2] };

		mWorld = DirectX::XMMatrixRotationY(angle);
		mView = DirectX::XMMatrixLookAtLH(
			DirectX::XMLoadFloat3(&eyePos),
			DirectX::XMLoadFloat3(&targetPos),
			DirectX::XMLoadFloat3(&upVec)
		);

		mapMatrix->mWorldViewProj = mWorld * mView * mProj;
		mapMatrix->mWorld = mWorld;
		mapMatrix->mView = mView;
		mapMatrix->mProj = mProj;
		mapMatrix->cameraPosWS = eyePos;

		// 〇コマンドアロケータとコマンドリストをクリア。
		// キューをクリア。
		result = _cmdAllocator->Reset();
		// 再びコマンドを溜める準備。
		result = _cmdList->Reset(_cmdAllocator, nullptr);

		// 〇レンダーターゲットの設定
		auto bbIdx = _swapChain->GetCurrentBackBufferIndex();
		auto rtvH = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
		rtvH.ptr += static_cast<long long unsigned int>(bbIdx) * 
			_dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		auto dsvH = dsvHeap->GetCPUDescriptorHandleForHeapStart();


		// リソースバリアの設定
		// リソースバリアを、レンダーターゲットとして使うように指定。
		D3D12_RESOURCE_BARRIER BarrierDesc =
			CD3DX12_RESOURCE_BARRIER::Transition(	// 遷移
				_backBuffers[bbIdx],				// バックバッファ―リソース
				D3D12_RESOURCE_STATE_PRESENT,		// 直前はPRESENT状態
				D3D12_RESOURCE_STATE_RENDER_TARGET	// 今からレンダーターゲット状態
			);
		//BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;	// 遷移
		//BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		//BarrierDesc.Transition.pResource = _backBuffers[bbIdx];	// バックバッファ―リソース
		//BarrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		//// 直前はPRESENT状態
		//BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		//// 今からレンダーターゲット状態
		//BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

		// バリア指定実行
		_cmdList->ResourceBarrier(1, &BarrierDesc);
		
		_cmdList->SetPipelineState(_pipelinestate);

		_cmdList->OMSetRenderTargets(1, &rtvH, true, &dsvH);

		// 〇レンダーターゲットのクリア
		constexpr float clearColor[] = { 0.5f,0.5f,0.5f,1.0f };
		_cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
		// 〇デプスステンシルバッファのクリア
		_cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		// 〇溜めておいた命令の実行
		_cmdList->RSSetViewports(1, &viewport);
		_cmdList->RSSetScissorRects(1, &scissorRect);
		_cmdList->SetGraphicsRootSignature(_rootsignature);

		_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		_cmdList->IASetVertexBuffers(0, 1, &vbView);
		_cmdList->IASetIndexBuffer(&ibView);

		_cmdList->SetGraphicsRootSignature(_rootsignature);
		_cmdList->SetDescriptorHeaps(1, &basicDescHeap);

		// ヒープの先頭アドレスを取得。
		auto heapHandle = basicDescHeap->GetGPUDescriptorHandleForHeapStart();
		_cmdList->SetGraphicsRootDescriptorTable(
			0,	// ルートパラメータインデックス
			heapHandle	// ヒープアドレス
		);

		// 次の番号のルートパラメータ用のヒープアドレスへ進める。
		heapHandle.ptr += 
			_dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		_cmdList->SetGraphicsRootDescriptorTable(
			1,	// ルートパラメータインデックス
			heapHandle	// ヒープアドレス
		);

		_cmdList->SetDescriptorHeaps(1, &matDescHeap);

		{
			// ヒープ先頭。
			auto materialH = matDescHeap->GetGPUDescriptorHandleForHeapStart();
			// 最初はオフセットなし。
			unsigned int idxOffset = 0;
			const auto CBVAndSRVInc = 
				_dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
				* 4;

			for (auto& m : materials)
			{

				_cmdList->SetGraphicsRootDescriptorTable(1, materialH);
				_cmdList->DrawIndexedInstanced(m.indicesNum, 1, idxOffset, 0, 0);

				// ヒープポインタとインデックスを次に進める。
				materialH.ptr += CBVAndSRVInc;					
				idxOffset += m.indicesNum;
			}
		}

		

		
		//_cmdList->DrawInstanced(pmdVertNum, 1, 0, 0);


		// リソースバリアをPRESENTにする。
		BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		// バリア指定実行
		_cmdList->ResourceBarrier(1, &BarrierDesc);

		// 命令の実行前に、必ずコマンドリストのクローズを行う。
		_cmdList->Close();

		// コマンドリストの実行。
		ID3D12CommandList* cmdLists[] = { _cmdList };
		_cmdQueue->ExecuteCommandLists(1, cmdLists);

		// 実行が完了するまで待つ。
		_cmdQueue->Signal(_fence, ++_fenceVal);
		if (_fence->GetCompletedValue() != _fenceVal)
		{
			// イベントハンドルを取得。
			auto eventH = CreateEvent(nullptr, false, false, nullptr);

			_fence->SetEventOnCompletion(_fenceVal, eventH);

			// イベントが発生するまで待ち続ける。
			WaitForSingleObject(eventH, INFINITE);

			CloseHandle(eventH);
		}

		// 〇画面のスワップ
		_swapChain->Present(1, 0);

	}



	for (auto res : g_resourceTable)
	{
		res.second->Release();
	}


	basicDescHeap->Release();
	constBuff->Release();


	matDescHeap->Release();
	materialBuff->Release();

	_psBlob->Release();
	_vsBlob->Release();
	idxBuff->Release();
	vertBuff->Release();


	_pipelinestate->Release();
	_rootsignature->Release();

	dsvHeap->Release();
	depthBuffer->Release();
	_fence->Release();
	for (int i = 0; static_cast<unsigned int>(i) < swcDesc.BufferCount; i++)
	{
		_backBuffers[i]->Release();
	}
	rtvHeaps->Release();
	_swapChain->Release();
	_cmdQueue->Release();
	_cmdList->Release();
	_cmdAllocator->Release();
	_dev->Release();
	tmpAdapter->Release();
	_dxgiFactory->Release();



	return 0;
}