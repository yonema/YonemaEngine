#include <Windows.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <d3dx12.h>

#include <DirectXMath.h>
#include <DirectXTex.h>

#include <vector>
#ifdef _DEBUG
#include	<iostream>
#endif

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "DirectXTex.lib")


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


/**
 * @brief ウィンドウプロシージャ。アプリケーションに送られてきたメッセージを処理する。
 * @param hwnd ウィンドウハンドル
 * @param msg メッセージコード
 * @param wparam w-パラメータ
 * @param lparam l-パラメータ
 * @return 
*/
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// 送られてきたメッセージで処理を分岐させる。
	switch (msg)
	{
	case WM_DESTROY:	// ウィンドウが破棄されたときに呼ばれる。
		// OSにアプリの終了を通知。
		PostQuitMessage(0);
		break;

	default:
		// 既定の処理を行う。
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}

	return 0;

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
	//DebugOutputFormatString("Show window test.");
	//char a = getchar();


	constexpr int kWindowWidth = 1280;
	constexpr int kWindowHeight = 720;

	// 〇ウィンドウクラスの作成と登録。

	// ウィンドウクラスのパラメータを設定。
	WNDCLASSEX w =
	{
		sizeof(WNDCLASSEX),			// 構造体のサイズ
		CS_CLASSDC,					// ウィンドウのスタイル。
		(WNDPROC)WindowProcedure,	// ウィンドウプロシージャ
		0,
		0,
		GetModuleHandle(nullptr),	// ハンドルの取得
		nullptr,					// アイコンのハンドル
		nullptr,					// マウスカーソルのハンドル
		nullptr,					// ウィンドウの背景色
		nullptr,					// メニュー名
		L"GameSample",				// ウィンドウクラス名（適当でよい）
		nullptr
	};

	// ウィンドウクラスの登録。
	RegisterClassEx(&w);

	// ウィンドウのサイズを決める。
	RECT wrc = { 0, 0, kWindowWidth, kWindowHeight };
	// 関数を使ってウィンドウのサイズを補正する。
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	// ウィンドウオブジェクトの生成。
	HWND hwnd = CreateWindow(
		w.lpszClassName,		// 使用するウィンドウクラス名の指定
		L"GameSample",			// タイトルバーの文字。クラス名と違っても大丈夫。
		WS_OVERLAPPEDWINDOW,	// ウィンドウのスタイル。タイトルバーと境界線があるウィンドウ。
		CW_USEDEFAULT,			// 表示x座標。OSにおまかせ。
		CW_USEDEFAULT,			// 表示y座標。OSにおまかせ。
		wrc.right - wrc.left,	// ウィンドウの幅
		wrc.bottom - wrc.top,	// ウィンドウの高さ
		nullptr,				// 親ウィンドウハンドル
		nullptr,				// メニューハンドル
		w.hInstance,			// 呼び出しアプリケーションハンドル
		nullptr					// 追加パラメータ
		);



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
	//_cmdList->Close();

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
		hwnd,
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
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	// ガンマ補正あり（sRGB）
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


	// 初期化
	result = CoInitializeEx(0, COINIT_MULTITHREADED);



	// 〇三角形の表示の準備
	// 〇四角形に変更する
	// 〇頂点情報にUVを追加

	SVertex vertices[] =
	{
		{{-0.4f, -0.7f, 0.0f},{0.0f, 1.0f}},	// 左下
		{{-0.4f,  0.7f, 0.0f},{0.0f, 0.0f}},	// 左上
		{{ 0.4f, -0.7f, 0.0f},{1.0f, 1.0f}},	// 右下
		{{ 0.4f,  0.7f, 0.0f},{1.0f, 0.0f}}		// 右上
	};

	// 〇テクスチャデータの作成
	//std::vector<STexRGBA> textureData(256 * 256);
	//for (auto& rgba : textureData)
	//{
	//	rgba.R = rand() % 256;
	//	rgba.G = rand() % 256;
	//	rgba.B = rand() % 256;
	//	rgba.A = 255;	// Aは1.0とする
	//}

	DirectX::XMMATRIX matrix = DirectX::XMMatrixIdentity();

	// 〇画像ファイルのロード

	// WICテクスチャのロード。

	// 画像ファイルに関する情報。
	DirectX::TexMetadata metaData = {};
	// 実際のデータ。
	DirectX::ScratchImage scratchImg = {};

	result = DirectX::LoadFromWICFile(
		L"Assets/Images/200x200PNG.png",
		DirectX::WIC_FLAGS_NONE,	// 特別なことをせずにロード
		&metaData,
		scratchImg
	);	

	auto img = scratchImg.GetImage(0, 0, 0);	// 生データ抽出

	// 〇CopyTextureRegion()メソッドによる転送
	// 〇アップロード用リソースの作成

	// 中間バッファとしてのアップロードヒープの設定。
	D3D12_HEAP_PROPERTIES uploadHeapProp = 
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);// マップ可能にするため、UPLOADにする
	//// マップ可能にするため、UPLOADにする。
	//uploadHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	//// アップロード用に使用すること前提のため、UNKNOWNで良い。
	//uploadHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	//uploadHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	//// 単一アダプタのため。
	//uploadHeapProp.CreationNodeMask = 0;
	//uploadHeapProp.VisibleNodeMask = 0;

	// リソースの設定。
	UINT64 texResDescWidth = static_cast<UINT64>(
		AlignmentedSize(img->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT) * img->height
		);
	D3D12_RESOURCE_DESC texResDesc = CD3DX12_RESOURCE_DESC::Buffer(texResDescWidth);;
	//texResDesc.Format = DXGI_FORMAT_UNKNOWN;	// 単なるデータの塊のためUNKNOWN
	//texResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;	// 単なるバッファとして指定
	//// データサイズ
	//// RowPitchは256の倍数でなければならないため、サイズも変更する。
	//texResDesc.Width = 
	//	static_cast<UINT64>(
	//		AlignmentedSize(img->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT) * img->height
	//		);
	//texResDesc.Height = 1;
	//texResDesc.DepthOrArraySize = 1;
	//texResDesc.MipLevels = 1;
	//texResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;	// 連続したデータ
	//texResDesc.Flags = D3D12_RESOURCE_FLAG_NONE;	// 特になし
	//texResDesc.SampleDesc.Count = 1;	// 通常テクスチャにアンチエイリアシングしない
	//texResDesc.SampleDesc.Quality = 0;

	// 中間バッファの作成。
	ID3D12Resource* uploadBuff = nullptr;
	result = _dev->CreateCommittedResource(
		&uploadHeapProp,
		D3D12_HEAP_FLAG_NONE,	// 特になし
		&texResDesc,
		// CPUからマップするためリソースステートを、CPUから描き込み可能だがGPUからは読み取りのみにする。
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&uploadBuff)
	);

	// 〇コピー先のリソースの作成

	// テクスチャのためのヒープ設定。
	D3D12_HEAP_PROPERTIES texHeapProp = 
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);	// テクスチャ用
	//texHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;	// テクスチャ用
	//texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	//texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	//// 単一アダプタのため。
	//texHeapProp.CreationNodeMask = 0;
	//texHeapProp.VisibleNodeMask = 0;

	// リソース設定（変数はつかいまわし）
	texResDesc.Format = metaData.format;
	texResDesc.Width = static_cast<UINT64>(metaData.width);	// 幅
	texResDesc.Height = static_cast<UINT>(metaData.height);	// 高さ
	texResDesc.DepthOrArraySize = static_cast<UINT16>(metaData.arraySize);	// 2Dで配列ではないため
	texResDesc.MipLevels = static_cast<UINT16>(metaData.mipLevels);	// ミップマップしないためミップ数は1
	texResDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metaData.dimension);
	texResDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;	// レイアウトは決定しない

	// テクスチャバッファの作成。
	ID3D12Resource* texBuff = nullptr;
	result = _dev->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE,	// 特に指定なし
		&texResDesc,
		D3D12_RESOURCE_STATE_COPY_DEST, // コピー先
		nullptr,
		IID_PPV_ARGS(&texBuff)
	);

	if (FAILED(result))
	{
		// テクスチャバッファの生成失敗。
		DebugOutputFormatString("テクスチャバッファの生成に失敗しました。");
	}

	// 〇アップロードリソースへのマップ
	uint8_t* mapforImg = nullptr;	// image->pixelsと同じ型にする
	result = uploadBuff->Map(0, nullptr, reinterpret_cast<void**>(&mapforImg));	// マップ
	// バッファのRowPitchは256の倍数でなければならないため、アライメントされている。
	// 元データのRowPitchとずれてしまうため、1行ごとにコピーして行頭が合うようにする。
	auto srcAddress = img->pixels;
	auto alignmentedRowPitch = AlignmentedSize(img->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
	for (int y = 0; y < img->height; y++)
	{
		// 1行ずつコピーする。
		std::copy_n(srcAddress, alignmentedRowPitch, mapforImg);
		// 行頭を合わせる。
		srcAddress += img->rowPitch;	// 元データは実際のrowPitch分だけ進める
		mapforImg += alignmentedRowPitch;	// バッファはアライメントされたrowPitch分だけ進める
	}
	//std::copy_n(img->pixels, img->slicePitch, mapforImg);	// コピー
	uploadBuff->Unmap(0, nullptr);

	// 〇CopyTextureRegion()メソッド

	// グラフィックボード上のコピー元アドレス。
	D3D12_TEXTURE_COPY_LOCATION src = {};

	// コピー元（アップロード側）の設定。
	src.pResource = uploadBuff;	// 中間バッファ
	// アップロードバッファには、フットプリント（メモリ占有領域に関する情報）指定。
	src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	src.PlacedFootprint.Offset = 0;
	src.PlacedFootprint.Footprint.Width = static_cast<UINT>(metaData.width);
	src.PlacedFootprint.Footprint.Height = static_cast<UINT>(metaData.height);
	src.PlacedFootprint.Footprint.Depth = static_cast<UINT>(metaData.depth);
	// RowPitchは256の倍数でなければならない。
	src.PlacedFootprint.Footprint.RowPitch = 
		static_cast<UINT>(AlignmentedSize(img->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT));
	src.PlacedFootprint.Footprint.Format = img->format;

	// グラフィックボード上のコピー先アドレス。
	D3D12_TEXTURE_COPY_LOCATION dst = {};
	// コピー先の設定。
	dst.pResource = texBuff;
	// テクスチャバッファには、インデックス指定。
	dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	dst.SubresourceIndex = 0;

	// コピー。
	_cmdList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

	// 〇バリアとフェンス設定
	D3D12_RESOURCE_BARRIER texBarrierDesc = {};
	texBarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	texBarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	texBarrierDesc.Transition.pResource = texBuff;
	texBarrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	// テクスチャ用リソースが、コピー先のままのため、テクスチャ用指定にする。
	texBarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	texBarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	_cmdList->ResourceBarrier(1, &texBarrierDesc);
	_cmdList->Close();
	// コマンドリストの実行。
	ID3D12CommandList* texCmdLists[] = { _cmdList };
	_cmdQueue->ExecuteCommandLists(1, texCmdLists);
	
	// フェンスで待ちを入れる。
	_cmdQueue->Signal(_fence, ++_fenceVal);
	if (_fence->GetCompletedValue() != _fenceVal)
	{
		auto eventH = CreateEvent(nullptr, false, false, nullptr);
		_fence->SetEventOnCompletion(_fenceVal, eventH);
		WaitForSingleObject(eventH, INFINITE);
		CloseHandle(eventH);
	}
	

	//// 〇テクスチャバッファの作成（WriteToSubresource()を使用する場合）

	//// WriteToSubresourceで転送するためのヒープ設定。
	//D3D12_HEAP_PROPERTIES texHeapProp = {};
	//// 特殊な設定のためDEFAULTでもUPLOADでもない。
	//texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	//// ライトバック。
	//texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	//// 転送はL0、つまりCPU側から直接行う。
	//texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	//// 単一アダプタのため。
	//texHeapProp.CreationNodeMask = 0;
	//texHeapProp.VisibleNodeMask = 0;

	//// リソースの設定。
	//D3D12_RESOURCE_DESC texResDesc = {};
	//texResDesc.Format = metaData.format;	// RGBAフォーマット
	//texResDesc.Width = static_cast<UINT64>(metaData.width);		// 幅
	//texResDesc.Height = static_cast<UINT>(metaData.height);	// 高さ
	//texResDesc.DepthOrArraySize = static_cast<UINT16>(metaData.arraySize);	// 2Dで配列ではないため
	//texResDesc.SampleDesc.Count = 1;	// 通常テクスチャのためアンチエイリアシングはしない
	//texResDesc.SampleDesc.Quality = 0;	// クオリティは最低
	//texResDesc.MipLevels = static_cast<UINT16>(metaData.mipLevels);	// ミップマップしないためミップ数は1
	//texResDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metaData.dimension);
	//texResDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;	// レイアウトは決定しない
	//texResDesc.Flags = D3D12_RESOURCE_FLAG_NONE;		// 特にフラグなし

	//// リソースの生成。
	//ID3D12Resource* texBuff = nullptr;
	//result = _dev->CreateCommittedResource(
	//	&texHeapProp,
	//	D3D12_HEAP_FLAG_NONE,	// 特に指定なし
	//	&texResDesc,
	//	D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,	// テクスチャ用指定
	//	nullptr,
	//	IID_PPV_ARGS(&texBuff)
	//);

	//if (FAILED(result))
	//{
	//	// テクスチャバッファの生成失敗。
	//	DebugOutputFormatString("テクスチャバッファの生成に失敗しました。");
	//}

	//// テクスチャデータの転送。
	//result = texBuff->WriteToSubresource(
	//	0,				// サブリソースのインデックス
	//	nullptr,		// 全領域へコピー
	//	img->pixels,	// 元データのアドレス
	//	static_cast<UINT>(img->rowPitch),	// 1ラインのサイズ
	//	static_cast<UINT>(img->slicePitch)	// 全サイズ
	//);
	//if (FAILED(result))
	//{
	//	// テクスチャデータの転送失敗。
	//	DebugOutputFormatString("テクスチャデータの転送に失敗しました。");
	//}

	// 〇定数バッファの生成。
	ID3D12Resource* constBuff = nullptr;
	// Mapで中身を書き換えるためUPLOAD。
	D3D12_HEAP_PROPERTIES constBuffHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// 必要なバイト数を256でアライメントしたバイト数が必要。
	// 0xff = 255
	// 最初に0xffを足して256以上にしてから、ビット反転した0xffとAND演算することで、
	// sizeを超えた最小の256の倍数を計算する。
	UINT64 constBuffSize = (sizeof(matrix) + 0xff) & ~0xff;
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
	DirectX::XMMATRIX* mapMatrix = nullptr;	// マップ先のポインター
	result = constBuff->Map(0, nullptr, reinterpret_cast<void**>(&mapMatrix));	// マップ
	*mapMatrix = matrix;	// 行列の内容をコピー

	// 〇シェーダーリソースビュー
	// 〇シェーダーリソースビュー用のディスクリプタヒープの作成。
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

	// 〇シェーダーリソースビューの生成。
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = metaData.format;
	// データのRGBAをどのようにマッピングするか。
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	// 2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;	// ミップマップは使用しないため1

	// ディスクリプタヒープの先頭アドレスを取得。
	// 先頭アドレスはシェーダーリソースビュー。
	auto basicHeapHandle = basicDescHeap->GetCPUDescriptorHandleForHeapStart();

	// 生成。
	_dev->CreateShaderResourceView(
		texBuff,	// ビューと関連付けしているバッファ
		&srvDesc,	// テクスチャの設定情報
		basicHeapHandle	// ヒープのどこに割り当てるか
	);

	// 〇定数バッファビューの生成。

	// 先頭アドレスはシェーダーリソースビューのため、ハンドルのptrをインクリメントし、
	// 定数バッファビューの位置まで移動。
	basicHeapHandle.ptr += 
		_dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = constBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = static_cast<UINT>(constBuff->GetDesc().Width);

	// ビューの作成。
	_dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);


	// 〇頂点バッファの作成

	// UPLOADヒープとしての設定
	D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	//heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	// CPUからアクセス可能。マップ可能）
	//heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;	// CPUのページング設定
	//// メモリプールの場所。システムメモリとかビデオメモリなど。
	//heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	// サイズに応じて適切な設定をしてくれる。
	D3D12_RESOURCE_DESC resdesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices));
	//resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;	// バッファーに使う
	//resdesc.Width = sizeof(vertices);	// 頂点情報が入るだけのサイズ
	//resdesc.Height = 1;
	//resdesc.DepthOrArraySize = 1;
	//resdesc.MipLevels = 1;
	//resdesc.Format = DXGI_FORMAT_UNKNOWN;	// 画像ではないためUNKNOWN
	//resdesc.SampleDesc.Count = 1;	// アンチエイリアシング使用時のパラメータ
	//resdesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	//// テクスチャではないため、UNKNOWNではない。
	//// メモリが最初から終わりまで連続していることを示すROW_MAJOR。
	//resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

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
	SVertex* vertMap = nullptr;
	// GPUのバッファーの（仮想）アドレスを取得。
	result = vertBuff->Map(0, nullptr, reinterpret_cast<void**>(&vertMap));
	// マップしてきたバッファーに、頂点データを書き込む。
	std::copy(std::begin(vertices), std::end(vertices), vertMap);
	// マップ解除。
	vertBuff->Unmap(0, nullptr);

	// 〇頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();	// バッファーの仮想アドレス
	vbView.SizeInBytes = sizeof(vertices);	// 全バイト数
	vbView.StrideInBytes = sizeof(vertices[0]);	// 1頂点あたりのバイト数

	// 〇インデックスバッファの作成

	unsigned short indeices[] =
	{
		// 時計回り
		0,1,2,
		2,1,3
	};

	ID3D12Resource* idxBuff = nullptr;
	// 設定は、バッファのサイズ以外、頂点バッファの設定を使いまわしてよい。
	resdesc.Width = sizeof(indeices);

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
	std::copy(std::begin(indeices), std::end(indeices), mappedIdx);
	idxBuff->Unmap(0, nullptr);

	// インデックスバッファビューを作成
	D3D12_INDEX_BUFFER_VIEW ibView = {};
	ibView.BufferLocation = idxBuff->GetGPUVirtualAddress();
	// unsigned int でインデックス配列を使用しているため、DXGI_FORMAT_R16_UINTを使用。
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeof(indeices);




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
		// UV
		{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		}
	};

	// 〇ルートシグネチャの生成
	ID3D12RootSignature* _rootsignature = nullptr;

	// ディスクリプタレンジ
	D3D12_DESCRIPTOR_RANGE descTblRange[2] = {};	// テクスチャと定数の2つ

	// テクスチャ用レジスタ0番
	descTblRange[0].NumDescriptors = 1;	// テクスチャ1つ
	descTblRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;	// 種別はテクスチャ
	descTblRange[0].BaseShaderRegister = 0;	// 0番スロットから
	// 連続したディスクリプタレンジが、前のディスクリプタレンジの直後に来る。
	descTblRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// 定数用レジスタ0番
	descTblRange[1].NumDescriptors = 1;	// 定数1つ
	descTblRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;	// 種別は定数
	descTblRange[1].BaseShaderRegister = 0;	// 0番スロットから
	descTblRange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// ルートパラメータの作成。
	D3D12_ROOT_PARAMETER rootparam[2] = {};
	rootparam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	// ピクセルシェーダーから見える。
	rootparam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootparam[0].DescriptorTable.pDescriptorRanges = &descTblRange[0];	// ディスクリプタレンジのアドレス
	rootparam[0].DescriptorTable.NumDescriptorRanges = 1;	// ディスクリプタレンジの数

	rootparam[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootparam[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;	// 頂点シェーダーから見える
	rootparam[1].DescriptorTable.pDescriptorRanges = & descTblRange[1];	// ディスクリプタレンジのアドレス
	rootparam[1].DescriptorTable.NumDescriptorRanges = 1;	// ディスクリプタレンジの数

	// サンプラの設定。
	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	// 横方向の繰り返し
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	// 縦方向の繰り返し
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	// 奥行きの繰り返し
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;	// ボーダーは黒
	//samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;	// 線形補完
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;	// 補間なし（ニアレストネイバー方）
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;	// ミップマップ最大値
	samplerDesc.MinLOD = 0.0f;	// ミップマップ最小値
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	// ピクセルシェーダーから見える
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;	// リサンプリングしない

	// ルートシグネチャの設定。
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	// 頂点情報（入力アセンブラ）がある。
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootparam;	// ルートパラメータの先頭アドレス
	rootSignatureDesc.NumParameters = 2;	// ルートパラメータの数
	rootSignatureDesc.pStaticSamplers = &samplerDesc;	// サンプラ
	rootSignatureDesc.NumStaticSamplers = 1;	// サンプラの数

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
	gpipeline.VS.pShaderBytecode = _vsBlob->GetBufferPointer();
	gpipeline.VS.BytecodeLength = _vsBlob->GetBufferSize();
	gpipeline.PS.pShaderBytecode = _psBlob->GetBufferPointer();
	gpipeline.PS.BytecodeLength = _psBlob->GetBufferSize();

	// 2.サンプルマスクとラスタライザステートの設定。
	// デフォルトのサンプルマスク。（0xffffffff）
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// まだ、アンチエイリアスは使わないためfalse
	gpipeline.RasterizerState.MultisampleEnable = false;
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;	// カリングしない
	gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;	// 中身を塗りつぶす
	gpipeline.RasterizerState.DepthClipEnable = true;	// 深度方向のクリッピングは有効に

	//残り
	gpipeline.RasterizerState.FrontCounterClockwise = false;
	gpipeline.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	gpipeline.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	gpipeline.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	gpipeline.RasterizerState.AntialiasedLineEnable = false;
	gpipeline.RasterizerState.ForcedSampleCount = 0;
	gpipeline.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	// 3.ブレンドステートの設定。
	// 全体のブレンドステートの設定。
	gpipeline.BlendState.AlphaToCoverageEnable = false;	// aテストを行うか？
	// 複数のレンダーターゲットにそれぞれ別のブレンドステートを割り当てるか？
	gpipeline.BlendState.IndependentBlendEnable = false;

	// レンダーターゲットごとのブレンドステートの設定。
	D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc = {};
	// BlendEnableとLogicOpEnableは、両方有効にすることはできない。どちらかのみtrueにする。
	renderTargetBlendDesc.BlendEnable = false;	// aブレンドなどを行うか？
	renderTargetBlendDesc.LogicOpEnable = false;	// 論理演算を行うか？
	renderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// 描き込む時のマスク値

	gpipeline.BlendState.RenderTarget[0] = renderTargetBlendDesc;

	// 4.入力レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = inputLayout;	// 入力レイアウトの戦闘アドレス
	gpipeline.InputLayout.NumElements = _countof(inputLayout);	// 入力レイアウトの要素数
	// トライアングルストリップの時に、特定のインデックスを切り離すための指定。
	gpipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;	// カットなし
	// 構成要素が「点」「線」「三角形」のどれかを指定。
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;	// 三角形で構成。

	// 5.レンダーターゲットの設定
	gpipeline.NumRenderTargets = 1;	// レンダーターゲットの数は今は1つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	// レンダーターゲットのカラーフォーマット

	// 6.アンチエイリアシングのためのサンプル数指定
	// AAしない設定。
	gpipeline.SampleDesc.Count = 1;		// サンプル数は1ピクセルにつき1
	gpipeline.SampleDesc.Quality = 0;	// クオリティは最低

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
	D3D12_VIEWPORT viewport = {};
	viewport.Width = kWindowWidth;		// 出力先の幅
	viewport.Height = kWindowHeight;	// 出力先の高さ
	viewport.TopLeftX = 0;				// 出力先の左上のX座標
	viewport.TopLeftY = 0;				// 出力先の左上のY座標
	viewport.MaxDepth = 1.0f;			// 最大深度値
	viewport.MinDepth = 0.0f;			// 最小深度値

	// 〇シザー矩形
	D3D12_RECT scissorRect = {};
	// ビューポートいっぱいに表示する。
	scissorRect.top = 0;
	scissorRect.left = 0;
	scissorRect.right = scissorRect.left + kWindowWidth;
	scissorRect.bottom = scissorRect.top + kWindowHeight;


	// ウィンドウ表示
#ifdef _DEBUG
	ShowWindow(hwnd, SW_SHOW);
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

		_cmdList->OMSetRenderTargets(1, &rtvH, true, nullptr);

		// 〇レンダーターゲットのクリア
		constexpr float clearColor[] = { 1.0f,1.0f,0.0f,1.0f };	// 黄色
		_cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

		// 〇溜めておいた命令の実行
		_cmdList->RSSetViewports(1, &viewport);
		_cmdList->RSSetScissorRects(1, &scissorRect);
		_cmdList->SetGraphicsRootSignature(_rootsignature);

		_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
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

		_cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);


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

	basicDescHeap->Release();
	constBuff->Release();
	texBuff->Release();
	uploadBuff->Release();
	scratchImg.Release();


	_psBlob->Release();
	_vsBlob->Release();
	idxBuff->Release();
	vertBuff->Release();


	_pipelinestate->Release();
	_rootsignature->Release();


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

	// もうクラスは使わないので、登録解除する。
	UnregisterClass(w.lpszClassName, w.hInstance);

	return 0;
}