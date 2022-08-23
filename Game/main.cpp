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
 * @brief ���_�f�[�^�\����
*/
struct SVertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 uv;
};

/**
 * @brief �ȈՃe�N�X�`���f�[�^�����p�\����
*/
struct STexRGBA
{
	unsigned char R = 0;
	unsigned char G = 0;
	unsigned char B = 0;
	unsigned char A = 0;
};

/**
 * @brief �A���C�����g�ɂ��낦���T�C�Y��Ԃ��B
 * @param[in] size ���̃T�C�Y
 * @param[in] alignment �A���C�����g�T�C�Y
 * @return �A���C�����g�����낦���T�C�Y
*/
size_t AlignmentedSize(size_t size, size_t alignment)
{
	return size + alignment - size % alignment;
}

/**
 * @brief �R���\�[����ʂɃt�H�[�}�b�g�t���������\��
 * @param format �t�H�[�}�b�g�i%d�Ƃ�%f�Ƃ��́j�t��������
 * @param  �ϒ�����
 * @remarks ���̊֐��̓f�o�b�N�p�ł��B�f�o�b�N���ɂ������삵�܂���B
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
 * @brief �E�B���h�E�v���V�[�W���B�A�v���P�[�V�����ɑ����Ă������b�Z�[�W����������B
 * @param hwnd �E�B���h�E�n���h��
 * @param msg ���b�Z�[�W�R�[�h
 * @param wparam w-�p�����[�^
 * @param lparam l-�p�����[�^
 * @return 
*/
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// �����Ă������b�Z�[�W�ŏ����𕪊򂳂���B
	switch (msg)
	{
	case WM_DESTROY:	// �E�B���h�E���j�����ꂽ�Ƃ��ɌĂ΂��B
		// OS�ɃA�v���̏I����ʒm�B
		PostQuitMessage(0);
		break;

	default:
		// ����̏������s���B
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}

	return 0;

}

/**
 * @brief DirectX12�̃f�o�b�N���C���[��L����
*/
void EnableDebugLayer()
{
	ID3D12Debug* debugLayer = nullptr;
	auto result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));

	// �f�o�b�N���C���[��L���ɂ���B
	debugLayer->EnableDebugLayer();
	// �L���ɂ�����A�C���^�[�t�F�[�X���J������B
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

	// �Z�E�B���h�E�N���X�̍쐬�Ɠo�^�B

	// �E�B���h�E�N���X�̃p�����[�^��ݒ�B
	WNDCLASSEX w =
	{
		sizeof(WNDCLASSEX),			// �\���̂̃T�C�Y
		CS_CLASSDC,					// �E�B���h�E�̃X�^�C���B
		(WNDPROC)WindowProcedure,	// �E�B���h�E�v���V�[�W��
		0,
		0,
		GetModuleHandle(nullptr),	// �n���h���̎擾
		nullptr,					// �A�C�R���̃n���h��
		nullptr,					// �}�E�X�J�[�\���̃n���h��
		nullptr,					// �E�B���h�E�̔w�i�F
		nullptr,					// ���j���[��
		L"GameSample",				// �E�B���h�E�N���X���i�K���ł悢�j
		nullptr
	};

	// �E�B���h�E�N���X�̓o�^�B
	RegisterClassEx(&w);

	// �E�B���h�E�̃T�C�Y�����߂�B
	RECT wrc = { 0, 0, kWindowWidth, kWindowHeight };
	// �֐����g���ăE�B���h�E�̃T�C�Y��␳����B
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	// �E�B���h�E�I�u�W�F�N�g�̐����B
	HWND hwnd = CreateWindow(
		w.lpszClassName,		// �g�p����E�B���h�E�N���X���̎w��
		L"GameSample",			// �^�C�g���o�[�̕����B�N���X���ƈ���Ă����v�B
		WS_OVERLAPPEDWINDOW,	// �E�B���h�E�̃X�^�C���B�^�C�g���o�[�Ƌ��E��������E�B���h�E�B
		CW_USEDEFAULT,			// �\��x���W�BOS�ɂ��܂����B
		CW_USEDEFAULT,			// �\��y���W�BOS�ɂ��܂����B
		wrc.right - wrc.left,	// �E�B���h�E�̕�
		wrc.bottom - wrc.top,	// �E�B���h�E�̍���
		nullptr,				// �e�E�B���h�E�n���h��
		nullptr,				// ���j���[�n���h��
		w.hInstance,			// �Ăяo���A�v���P�[�V�����n���h��
		nullptr					// �ǉ��p�����[�^
		);



#ifdef _DEBUG
	// �f�o�b�N���C���[���I���ɁB
	EnableDebugLayer();
#endif


	ID3D12Device* _dev = nullptr;
	IDXGIFactory6* _dxgiFactory = nullptr;
	IDXGISwapChain4* _swapChain = nullptr;


	// �Z�A�_�v�^�[�̓���

	
	HRESULT result;
	// DXGIFactory�I�u�W�F�N�g�̐����B
#ifdef _DEBUG
	// DXGI����̃G���[���b�Z�[�W�𒲂ׂ邽�߁B
	result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&_dxgiFactory));
#else
	result = CreateDXGIFactory1(IID_PPV_ARGS(&_dxgiFactory));
#endif

	// �����ɓ���̖��O�����A�_�v�^�[�I�u�W�F�N�g������B	
	IDXGIAdapter* tmpAdapter = nullptr;

	for (int i = 0; _dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; i++)
	{
		DXGI_ADAPTER_DESC adesc = {};
		tmpAdapter->GetDesc(&adesc);

		std::wstring strDesc = adesc.Description;

		// �T�������A�_�v�^�[�̖��O���m�F�B
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


	// �ZD3D12�f�o�C�X�̐���

	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	//D3D_FEATURE_LEVEL featureLevel;

	// �f�o�C�X�������\�ȃt�B�[�`���[���x����T���B
	for (auto lv : levels)
	{
		// Direct3D�f�o�C�X�̐���
		auto hr = D3D12CreateDevice(
			tmpAdapter,	// nullptr���Ǝ����ŃA�_�v�^�[�i�O���t�B�b�N�h���C�o�[�j���I�������B
			lv,			// �@�\���x���B�I�������h���C�o���Ή����Ă��Ȃ������玸�s����B
			IID_PPV_ARGS(&_dev)
		);

		if (SUCCEEDED(hr))
		{
			// �����\�ȃo�[�W��������������A���[�v��ł��؂�B
			break;
		}

		// �ǂ���_����������f�o�C�X��nullptr�ɂȂ邩��A�����ŃA�v���P�[�V�������I��������B
	}

	if (_dev == nullptr)
	{
		// �f�o�C�X�̐������s�B
		DebugOutputFormatString("�f�o�C�X�̐����Ɏ��s���܂����B");
	}


	// �Z�R�}���h�A���P�[�^�̐���
	ID3D12CommandAllocator* _cmdAllocator = nullptr;
	result = _dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdAllocator));

	if (FAILED(result))
	{
		// �R�}���h�A���P�[�^�̐������s�B
		DebugOutputFormatString("�R�}���h�A���P�[�^�̐����Ɏ��s���܂����B");
	}

	// �Z�R�}���h���X�g�̐���
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
		// �R�}���h���X�g�̐������s�B
		DebugOutputFormatString("�R�}���h���X�g�̐����Ɏ��s���܂����B");
	}

	// �R�}���h���X�g�́A�J����Ă����ԂŐ�������邽�߁A���Ă����B

	// �摜��GPU�ɃR�s�[����Ƃ��ɃR�}���h���X�g���g�p���邽�߁A���Ȃ��B
	// �ʏ�̓��[�v���ŃR�s�[����邽�߁A���Ȃ��͍̂���̏ꍇ�����B
	// ��Œ����B
	//_cmdList->Close();

	// �Z�R�}���h�L���[�̐���
	ID3D12CommandQueue* _cmdQueue = nullptr;

	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
	// �^�C���A�E�g�Ȃ��B
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	// �A�_�v�^�[��1�����g��Ȃ����́A0�ł悢�B
	cmdQueueDesc.NodeMask = 0;
	// �v���C�I���e�B�͓��Ɏw��Ȃ��B
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	// �R�}���h���X�g�ƍ��킹��B
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	// �L���[�̍쐬
	result = _dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&_cmdQueue));
	if (FAILED(result))
	{
		// �R�}���h�L���[�̍쐬���s�B
		DebugOutputFormatString("�R�}���h�L���[�̐����Ɏ��s���܂����B");
	}

	// �Z�X���b�v�`�F�[���̐���
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {  };
	swapChainDesc.Width = kWindowWidth;		// �摜�𑜓x�̕�
	swapChainDesc.Height = kWindowHeight;	// �摜�𑜓x�̍���
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// �s�N�Z���t�H�[�}�b�g
	swapChainDesc.Stereo = false;			// �X�e���I�\���t���O�i3D�f�B�X�v���C�̃X�e���I���[�h�j
	swapChainDesc.SampleDesc.Count = 1;		// �}���`�T���v���̎w��B
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;			// �_�u���o�b�t�@�\�Ȃ�2
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;	// �o�b�N�o�b�t�@�͐L�яk�݉\
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	// �t���b�v��͑��₩�ɔj��
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;	// ���Ɏw��Ȃ�
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// �E�B���h�E�����t���X�N���[���؂�ւ��\

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
		// �X���b�v�`�F�[���̍쐬���s
		DebugOutputFormatString("�X���b�v�`�F�[���̐����Ɏ��s���܂����B");
	}

	// �Z�����_�[�^�[�Q�b�g�r���[�̍쐬
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	// �����_�[�^�[�Q�b�g�r���[�̂��߁B
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heapDesc.NodeMask = 0;
	// ���\��2�B
	heapDesc.NumDescriptors = 2;
	// ���Ɏw��Ȃ��B
	// �V�F�[�_�\����Q�Ƃ���K�v���Ȃ����߁B
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	ID3D12DescriptorHeap* rtvHeaps = nullptr;
	result = _dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeaps));

	if (FAILED(result))
	{
		// �����_�[�^�[�Q�b�g�r���[�̍쐬���s
		DebugOutputFormatString("�����_�[�^�[�Q�b�g�r���[�̐����Ɏ��s���܂����B");
	}

	// �Z�X���b�v�`�F�[���ƃ�������R�Â���B

	// �o�b�N�o�b�t�@�̐��𒲂ׂ邽�߂ɁA�X���b�v�`�F�C������f�B�X�N���v�^������Ă���B
	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	result = _swapChain->GetDesc(&swcDesc);
	if (FAILED(result))
	{
		// �X���b�v�`�F�C���̎擾���s
		DebugOutputFormatString("�X���b�v�`�F�[���̎擾�Ɏ��s���܂����B");
	}

	// sRGB�����_�[�^�[�Q�b�g�r���[�̐ݒ�B
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	// �K���}�␳����isRGB�j
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	std::vector<ID3D12Resource*> _backBuffers(swcDesc.BufferCount);
	for (int idx = 0; static_cast<unsigned int>(idx) < swcDesc.BufferCount; idx++)
	{
		// �X���b�v�`�F�C�����̃o�b�t�@�ƃr���[���֘A�t���B
		result = _swapChain->GetBuffer(idx, IID_PPV_ARGS(&_backBuffers[idx]));

		// �擪�A�h���X�̃n���h�����擾�B
		D3D12_CPU_DESCRIPTOR_HANDLE handle = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
		// �����̃f�B�X�N���v�^���擾���邽�߁A�A�h���X�����炷�B
		handle.ptr += static_cast<unsigned long long int>(idx) * 
			_dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		// �����_�[�^�[�Q�b�g�r���[���쐬�B
		_dev->CreateRenderTargetView(_backBuffers[idx], &rtvDesc, handle);
	}


	// �Z�t�F���X�̐���
	ID3D12Fence* _fence = nullptr;
	UINT64 _fenceVal = 0;
	result = _dev->CreateFence(_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
	if (FAILED(result))
	{
		// �t�F���X�̐������s�B
		DebugOutputFormatString("�t�F���X�̐����Ɏ��s���܂����B");
	}


	// ������
	result = CoInitializeEx(0, COINIT_MULTITHREADED);



	// �Z�O�p�`�̕\���̏���
	// �Z�l�p�`�ɕύX����
	// �Z���_����UV��ǉ�

	SVertex vertices[] =
	{
		{{-0.4f, -0.7f, 0.0f},{0.0f, 1.0f}},	// ����
		{{-0.4f,  0.7f, 0.0f},{0.0f, 0.0f}},	// ����
		{{ 0.4f, -0.7f, 0.0f},{1.0f, 1.0f}},	// �E��
		{{ 0.4f,  0.7f, 0.0f},{1.0f, 0.0f}}		// �E��
	};

	// �Z�e�N�X�`���f�[�^�̍쐬
	//std::vector<STexRGBA> textureData(256 * 256);
	//for (auto& rgba : textureData)
	//{
	//	rgba.R = rand() % 256;
	//	rgba.G = rand() % 256;
	//	rgba.B = rand() % 256;
	//	rgba.A = 255;	// A��1.0�Ƃ���
	//}

	DirectX::XMMATRIX matrix = DirectX::XMMatrixIdentity();

	// �Z�摜�t�@�C���̃��[�h

	// WIC�e�N�X�`���̃��[�h�B

	// �摜�t�@�C���Ɋւ�����B
	DirectX::TexMetadata metaData = {};
	// ���ۂ̃f�[�^�B
	DirectX::ScratchImage scratchImg = {};

	result = DirectX::LoadFromWICFile(
		L"Assets/Images/200x200PNG.png",
		DirectX::WIC_FLAGS_NONE,	// ���ʂȂ��Ƃ������Ƀ��[�h
		&metaData,
		scratchImg
	);	

	auto img = scratchImg.GetImage(0, 0, 0);	// ���f�[�^���o

	// �ZCopyTextureRegion()���\�b�h�ɂ��]��
	// �Z�A�b�v���[�h�p���\�[�X�̍쐬

	// ���ԃo�b�t�@�Ƃ��ẴA�b�v���[�h�q�[�v�̐ݒ�B
	D3D12_HEAP_PROPERTIES uploadHeapProp = 
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);// �}�b�v�\�ɂ��邽�߁AUPLOAD�ɂ���
	//// �}�b�v�\�ɂ��邽�߁AUPLOAD�ɂ���B
	//uploadHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	//// �A�b�v���[�h�p�Ɏg�p���邱�ƑO��̂��߁AUNKNOWN�ŗǂ��B
	//uploadHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	//uploadHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	//// �P��A�_�v�^�̂��߁B
	//uploadHeapProp.CreationNodeMask = 0;
	//uploadHeapProp.VisibleNodeMask = 0;

	// ���\�[�X�̐ݒ�B
	UINT64 texResDescWidth = static_cast<UINT64>(
		AlignmentedSize(img->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT) * img->height
		);
	D3D12_RESOURCE_DESC texResDesc = CD3DX12_RESOURCE_DESC::Buffer(texResDescWidth);;
	//texResDesc.Format = DXGI_FORMAT_UNKNOWN;	// �P�Ȃ�f�[�^�̉�̂���UNKNOWN
	//texResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;	// �P�Ȃ�o�b�t�@�Ƃ��Ďw��
	//// �f�[�^�T�C�Y
	//// RowPitch��256�̔{���łȂ���΂Ȃ�Ȃ����߁A�T�C�Y���ύX����B
	//texResDesc.Width = 
	//	static_cast<UINT64>(
	//		AlignmentedSize(img->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT) * img->height
	//		);
	//texResDesc.Height = 1;
	//texResDesc.DepthOrArraySize = 1;
	//texResDesc.MipLevels = 1;
	//texResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;	// �A�������f�[�^
	//texResDesc.Flags = D3D12_RESOURCE_FLAG_NONE;	// ���ɂȂ�
	//texResDesc.SampleDesc.Count = 1;	// �ʏ�e�N�X�`���ɃA���`�G�C���A�V���O���Ȃ�
	//texResDesc.SampleDesc.Quality = 0;

	// ���ԃo�b�t�@�̍쐬�B
	ID3D12Resource* uploadBuff = nullptr;
	result = _dev->CreateCommittedResource(
		&uploadHeapProp,
		D3D12_HEAP_FLAG_NONE,	// ���ɂȂ�
		&texResDesc,
		// CPU����}�b�v���邽�߃��\�[�X�X�e�[�g���ACPU����`�����݉\����GPU����͓ǂݎ��݂̂ɂ���B
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&uploadBuff)
	);

	// �Z�R�s�[��̃��\�[�X�̍쐬

	// �e�N�X�`���̂��߂̃q�[�v�ݒ�B
	D3D12_HEAP_PROPERTIES texHeapProp = 
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);	// �e�N�X�`���p
	//texHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;	// �e�N�X�`���p
	//texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	//texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	//// �P��A�_�v�^�̂��߁B
	//texHeapProp.CreationNodeMask = 0;
	//texHeapProp.VisibleNodeMask = 0;

	// ���\�[�X�ݒ�i�ϐ��͂����܂킵�j
	texResDesc.Format = metaData.format;
	texResDesc.Width = static_cast<UINT64>(metaData.width);	// ��
	texResDesc.Height = static_cast<UINT>(metaData.height);	// ����
	texResDesc.DepthOrArraySize = static_cast<UINT16>(metaData.arraySize);	// 2D�Ŕz��ł͂Ȃ�����
	texResDesc.MipLevels = static_cast<UINT16>(metaData.mipLevels);	// �~�b�v�}�b�v���Ȃ����߃~�b�v����1
	texResDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metaData.dimension);
	texResDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;	// ���C�A�E�g�͌��肵�Ȃ�

	// �e�N�X�`���o�b�t�@�̍쐬�B
	ID3D12Resource* texBuff = nullptr;
	result = _dev->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE,	// ���Ɏw��Ȃ�
		&texResDesc,
		D3D12_RESOURCE_STATE_COPY_DEST, // �R�s�[��
		nullptr,
		IID_PPV_ARGS(&texBuff)
	);

	if (FAILED(result))
	{
		// �e�N�X�`���o�b�t�@�̐������s�B
		DebugOutputFormatString("�e�N�X�`���o�b�t�@�̐����Ɏ��s���܂����B");
	}

	// �Z�A�b�v���[�h���\�[�X�ւ̃}�b�v
	uint8_t* mapforImg = nullptr;	// image->pixels�Ɠ����^�ɂ���
	result = uploadBuff->Map(0, nullptr, reinterpret_cast<void**>(&mapforImg));	// �}�b�v
	// �o�b�t�@��RowPitch��256�̔{���łȂ���΂Ȃ�Ȃ����߁A�A���C�����g����Ă���B
	// ���f�[�^��RowPitch�Ƃ���Ă��܂����߁A1�s���ƂɃR�s�[���čs���������悤�ɂ���B
	auto srcAddress = img->pixels;
	auto alignmentedRowPitch = AlignmentedSize(img->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
	for (int y = 0; y < img->height; y++)
	{
		// 1�s���R�s�[����B
		std::copy_n(srcAddress, alignmentedRowPitch, mapforImg);
		// �s�������킹��B
		srcAddress += img->rowPitch;	// ���f�[�^�͎��ۂ�rowPitch�������i�߂�
		mapforImg += alignmentedRowPitch;	// �o�b�t�@�̓A���C�����g���ꂽrowPitch�������i�߂�
	}
	//std::copy_n(img->pixels, img->slicePitch, mapforImg);	// �R�s�[
	uploadBuff->Unmap(0, nullptr);

	// �ZCopyTextureRegion()���\�b�h

	// �O���t�B�b�N�{�[�h��̃R�s�[���A�h���X�B
	D3D12_TEXTURE_COPY_LOCATION src = {};

	// �R�s�[���i�A�b�v���[�h���j�̐ݒ�B
	src.pResource = uploadBuff;	// ���ԃo�b�t�@
	// �A�b�v���[�h�o�b�t�@�ɂ́A�t�b�g�v�����g�i��������L�̈�Ɋւ�����j�w��B
	src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	src.PlacedFootprint.Offset = 0;
	src.PlacedFootprint.Footprint.Width = static_cast<UINT>(metaData.width);
	src.PlacedFootprint.Footprint.Height = static_cast<UINT>(metaData.height);
	src.PlacedFootprint.Footprint.Depth = static_cast<UINT>(metaData.depth);
	// RowPitch��256�̔{���łȂ���΂Ȃ�Ȃ��B
	src.PlacedFootprint.Footprint.RowPitch = 
		static_cast<UINT>(AlignmentedSize(img->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT));
	src.PlacedFootprint.Footprint.Format = img->format;

	// �O���t�B�b�N�{�[�h��̃R�s�[��A�h���X�B
	D3D12_TEXTURE_COPY_LOCATION dst = {};
	// �R�s�[��̐ݒ�B
	dst.pResource = texBuff;
	// �e�N�X�`���o�b�t�@�ɂ́A�C���f�b�N�X�w��B
	dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	dst.SubresourceIndex = 0;

	// �R�s�[�B
	_cmdList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

	// �Z�o���A�ƃt�F���X�ݒ�
	D3D12_RESOURCE_BARRIER texBarrierDesc = {};
	texBarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	texBarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	texBarrierDesc.Transition.pResource = texBuff;
	texBarrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	// �e�N�X�`���p���\�[�X���A�R�s�[��̂܂܂̂��߁A�e�N�X�`���p�w��ɂ���B
	texBarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	texBarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	_cmdList->ResourceBarrier(1, &texBarrierDesc);
	_cmdList->Close();
	// �R�}���h���X�g�̎��s�B
	ID3D12CommandList* texCmdLists[] = { _cmdList };
	_cmdQueue->ExecuteCommandLists(1, texCmdLists);
	
	// �t�F���X�ő҂�������B
	_cmdQueue->Signal(_fence, ++_fenceVal);
	if (_fence->GetCompletedValue() != _fenceVal)
	{
		auto eventH = CreateEvent(nullptr, false, false, nullptr);
		_fence->SetEventOnCompletion(_fenceVal, eventH);
		WaitForSingleObject(eventH, INFINITE);
		CloseHandle(eventH);
	}
	

	//// �Z�e�N�X�`���o�b�t�@�̍쐬�iWriteToSubresource()���g�p����ꍇ�j

	//// WriteToSubresource�œ]�����邽�߂̃q�[�v�ݒ�B
	//D3D12_HEAP_PROPERTIES texHeapProp = {};
	//// ����Ȑݒ�̂���DEFAULT�ł�UPLOAD�ł��Ȃ��B
	//texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	//// ���C�g�o�b�N�B
	//texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	//// �]����L0�A�܂�CPU�����璼�ڍs���B
	//texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	//// �P��A�_�v�^�̂��߁B
	//texHeapProp.CreationNodeMask = 0;
	//texHeapProp.VisibleNodeMask = 0;

	//// ���\�[�X�̐ݒ�B
	//D3D12_RESOURCE_DESC texResDesc = {};
	//texResDesc.Format = metaData.format;	// RGBA�t�H�[�}�b�g
	//texResDesc.Width = static_cast<UINT64>(metaData.width);		// ��
	//texResDesc.Height = static_cast<UINT>(metaData.height);	// ����
	//texResDesc.DepthOrArraySize = static_cast<UINT16>(metaData.arraySize);	// 2D�Ŕz��ł͂Ȃ�����
	//texResDesc.SampleDesc.Count = 1;	// �ʏ�e�N�X�`���̂��߃A���`�G�C���A�V���O�͂��Ȃ�
	//texResDesc.SampleDesc.Quality = 0;	// �N�I���e�B�͍Œ�
	//texResDesc.MipLevels = static_cast<UINT16>(metaData.mipLevels);	// �~�b�v�}�b�v���Ȃ����߃~�b�v����1
	//texResDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metaData.dimension);
	//texResDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;	// ���C�A�E�g�͌��肵�Ȃ�
	//texResDesc.Flags = D3D12_RESOURCE_FLAG_NONE;		// ���Ƀt���O�Ȃ�

	//// ���\�[�X�̐����B
	//ID3D12Resource* texBuff = nullptr;
	//result = _dev->CreateCommittedResource(
	//	&texHeapProp,
	//	D3D12_HEAP_FLAG_NONE,	// ���Ɏw��Ȃ�
	//	&texResDesc,
	//	D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,	// �e�N�X�`���p�w��
	//	nullptr,
	//	IID_PPV_ARGS(&texBuff)
	//);

	//if (FAILED(result))
	//{
	//	// �e�N�X�`���o�b�t�@�̐������s�B
	//	DebugOutputFormatString("�e�N�X�`���o�b�t�@�̐����Ɏ��s���܂����B");
	//}

	//// �e�N�X�`���f�[�^�̓]���B
	//result = texBuff->WriteToSubresource(
	//	0,				// �T�u���\�[�X�̃C���f�b�N�X
	//	nullptr,		// �S�̈�փR�s�[
	//	img->pixels,	// ���f�[�^�̃A�h���X
	//	static_cast<UINT>(img->rowPitch),	// 1���C���̃T�C�Y
	//	static_cast<UINT>(img->slicePitch)	// �S�T�C�Y
	//);
	//if (FAILED(result))
	//{
	//	// �e�N�X�`���f�[�^�̓]�����s�B
	//	DebugOutputFormatString("�e�N�X�`���f�[�^�̓]���Ɏ��s���܂����B");
	//}

	// �Z�萔�o�b�t�@�̐����B
	ID3D12Resource* constBuff = nullptr;
	// Map�Œ��g�����������邽��UPLOAD�B
	D3D12_HEAP_PROPERTIES constBuffHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// �K�v�ȃo�C�g����256�ŃA���C�����g�����o�C�g�����K�v�B
	// 0xff = 255
	// �ŏ���0xff�𑫂���256�ȏ�ɂ��Ă���A�r�b�g���]����0xff��AND���Z���邱�ƂŁA
	// size�𒴂����ŏ���256�̔{�����v�Z����B
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
		// �萔�o�b�t�@�̐������s�B
		DebugOutputFormatString("�萔�o�b�t�@�̐����Ɏ��s���܂����B");
	}

	// �萔�o�b�t�@�̃R�s�[
	DirectX::XMMATRIX* mapMatrix = nullptr;	// �}�b�v��̃|�C���^�[
	result = constBuff->Map(0, nullptr, reinterpret_cast<void**>(&mapMatrix));	// �}�b�v
	*mapMatrix = matrix;	// �s��̓��e���R�s�[

	// �Z�V�F�[�_�[���\�[�X�r���[
	// �Z�V�F�[�_�[���\�[�X�r���[�p�̃f�B�X�N���v�^�q�[�v�̍쐬�B
	// �Z�萔�o�b�t�@�r���[
	ID3D12DescriptorHeap* basicDescHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	// �V�F�[�_�[���猩����悤�ɁB
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	// �}�X�N��0�B
	descHeapDesc.NodeMask = 0;
	// �r���[�͍��̂Ƃ���1�����B
	// SRV��CBV��2�B
	descHeapDesc.NumDescriptors = 2;
	// �V�F�[�_�[���\�[�X�r���[�p�B
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	// �����B
	result = _dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&basicDescHeap));
	if (FAILED(result))
	{
		// �f�B�X�N���v�^�q�[�v�̐������s�B
		DebugOutputFormatString("�f�B�X�N���v�^�q�[�v�̐����Ɏ��s���܂����B");
	}

	// �Z�V�F�[�_�[���\�[�X�r���[�̐����B
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = metaData.format;
	// �f�[�^��RGBA���ǂ̂悤�Ƀ}�b�s���O���邩�B
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	// 2D�e�N�X�`��
	srvDesc.Texture2D.MipLevels = 1;	// �~�b�v�}�b�v�͎g�p���Ȃ�����1

	// �f�B�X�N���v�^�q�[�v�̐擪�A�h���X���擾�B
	// �擪�A�h���X�̓V�F�[�_�[���\�[�X�r���[�B
	auto basicHeapHandle = basicDescHeap->GetCPUDescriptorHandleForHeapStart();

	// �����B
	_dev->CreateShaderResourceView(
		texBuff,	// �r���[�Ɗ֘A�t�����Ă���o�b�t�@
		&srvDesc,	// �e�N�X�`���̐ݒ���
		basicHeapHandle	// �q�[�v�̂ǂ��Ɋ��蓖�Ă邩
	);

	// �Z�萔�o�b�t�@�r���[�̐����B

	// �擪�A�h���X�̓V�F�[�_�[���\�[�X�r���[�̂��߁A�n���h����ptr���C���N�������g���A
	// �萔�o�b�t�@�r���[�̈ʒu�܂ňړ��B
	basicHeapHandle.ptr += 
		_dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = constBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = static_cast<UINT>(constBuff->GetDesc().Width);

	// �r���[�̍쐬�B
	_dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);


	// �Z���_�o�b�t�@�̍쐬

	// UPLOAD�q�[�v�Ƃ��Ă̐ݒ�
	D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	//heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	// CPU����A�N�Z�X�\�B�}�b�v�\�j
	//heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;	// CPU�̃y�[�W���O�ݒ�
	//// �������v�[���̏ꏊ�B�V�X�e���������Ƃ��r�f�I�������ȂǁB
	//heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	// �T�C�Y�ɉ����ēK�؂Ȑݒ�����Ă����B
	D3D12_RESOURCE_DESC resdesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices));
	//resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;	// �o�b�t�@�[�Ɏg��
	//resdesc.Width = sizeof(vertices);	// ���_��񂪓��邾���̃T�C�Y
	//resdesc.Height = 1;
	//resdesc.DepthOrArraySize = 1;
	//resdesc.MipLevels = 1;
	//resdesc.Format = DXGI_FORMAT_UNKNOWN;	// �摜�ł͂Ȃ�����UNKNOWN
	//resdesc.SampleDesc.Count = 1;	// �A���`�G�C���A�V���O�g�p���̃p�����[�^
	//resdesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	//// �e�N�X�`���ł͂Ȃ����߁AUNKNOWN�ł͂Ȃ��B
	//// ���������ŏ�����I���܂ŘA�����Ă��邱�Ƃ�����ROW_MAJOR�B
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
		// ���_�o�b�t�@�̐������s�B
		DebugOutputFormatString("���_�o�b�t�@�̐����Ɏ��s���܂����B");
	}

	// �Z���_���̃R�s�[�i�}�b�v�j
	SVertex* vertMap = nullptr;
	// GPU�̃o�b�t�@�[�́i���z�j�A�h���X���擾�B
	result = vertBuff->Map(0, nullptr, reinterpret_cast<void**>(&vertMap));
	// �}�b�v���Ă����o�b�t�@�[�ɁA���_�f�[�^���������ށB
	std::copy(std::begin(vertices), std::end(vertices), vertMap);
	// �}�b�v�����B
	vertBuff->Unmap(0, nullptr);

	// �Z���_�o�b�t�@�r���[�̍쐬
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();	// �o�b�t�@�[�̉��z�A�h���X
	vbView.SizeInBytes = sizeof(vertices);	// �S�o�C�g��
	vbView.StrideInBytes = sizeof(vertices[0]);	// 1���_������̃o�C�g��

	// �Z�C���f�b�N�X�o�b�t�@�̍쐬

	unsigned short indeices[] =
	{
		// ���v���
		0,1,2,
		2,1,3
	};

	ID3D12Resource* idxBuff = nullptr;
	// �ݒ�́A�o�b�t�@�̃T�C�Y�ȊO�A���_�o�b�t�@�̐ݒ���g���܂킵�Ă悢�B
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
		// �C���f�b�N�X�o�b�t�@�̐������s�B
		DebugOutputFormatString("�C���f�b�N�X�o�b�t�@�̐����Ɏ��s���܂����B");
	}

	// ������o�b�t�@�[�ɃC���f�b�N�X�f�[�^���R�s�[
	unsigned short* mappedIdx = nullptr;
	idxBuff->Map(0, nullptr, reinterpret_cast<void**>(&mappedIdx));
	std::copy(std::begin(indeices), std::end(indeices), mappedIdx);
	idxBuff->Unmap(0, nullptr);

	// �C���f�b�N�X�o�b�t�@�r���[���쐬
	D3D12_INDEX_BUFFER_VIEW ibView = {};
	ibView.BufferLocation = idxBuff->GetGPUVirtualAddress();
	// unsigned int �ŃC���f�b�N�X�z����g�p���Ă��邽�߁ADXGI_FORMAT_R16_UINT���g�p�B
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeof(indeices);




	// �Z�V�F�[�_�\�̓ǂݍ��݂Ɛ���

	ID3DBlob* _vsBlob = nullptr;
	ID3DBlob* _psBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	result = D3DCompileFromFile(
		L"Assets/Shaders/BasicVertexShader.hlsl",	// �V�F�[�_�\�p�X
		nullptr,							// define�͂Ȃ�
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	// �C���N���[�h�\�ɂ��Ă���
		"BasicVS",							// �G���g���[�|�C���g
		"vs_5_0",							// �o�[�W����
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	// �f�o�b�N�p����эœK���Ȃ�
		0,
		&_vsBlob,
		&errorBlob							// �G���[���Ƀ��b�Z�[�W������B�Ȃ����nullptr������B
	);

	if (FAILED(result))
	{
		// ���_�V�F�[�_�\�̐������s�B
		DebugOutputFormatString("���_�V�F�[�_�\�̐����Ɏ��s���܂����B");

		// ���_�V�F�[�_�[�̃t�@�C����������Ȃ��B
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		{
			::OutputDebugStringA("���_�V�F�[�_�[�̃t�@�C����������܂���B");
		}
		// �G���[���b�Z�[�W�̕\���B
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
		L"Assets/Shaders/BasicPixelShader.hlsl",	// �V�F�[�_�\�p�X
		nullptr,							// define�͂Ȃ�
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	// �C���N���[�h�\�ɂ��Ă���
		"BasicPS",							// �G���g���[�|�C���g
		"ps_5_0",							// �o�[�W����
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	// �f�o�b�N�p����эœK���Ȃ�
		0,
		&_psBlob,
		&errorBlob							// �G���[���Ƀ��b�Z�[�W������B�Ȃ����nullptr������B
	);

	if (FAILED(result))
	{
		// �s�N�Z���V�F�[�_�\�̐������s�B
		DebugOutputFormatString("�s�N�Z���V�F�[�_�\�̐����Ɏ��s���܂����B");

		// �s�N�Z���V�F�[�_�[�̃t�@�C����������Ȃ��B
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		{
			::OutputDebugStringA("�s�N�Z���V�F�[�_�[�̃t�@�C����������܂���B");
		}
		// �G���[���b�Z�[�W�̕\���B
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

	// �Z���_���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		// ���W���
		{
			"POSITION",	// �Z�}���e�B�N�X��
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,	// �t�H�[�}�b�g
			0,	// ���̓X���b�g�̃C���f�b�N�X
			D3D12_APPEND_ALIGNED_ELEMENT,	// �f�[�^�̃I�t�Z�b�g�ʒu
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0	// ��x�ɕ`�悷��C���X�^���X�̐�
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

	// �Z���[�g�V�O�l�`���̐���
	ID3D12RootSignature* _rootsignature = nullptr;

	// �f�B�X�N���v�^�����W
	D3D12_DESCRIPTOR_RANGE descTblRange[2] = {};	// �e�N�X�`���ƒ萔��2��

	// �e�N�X�`���p���W�X�^0��
	descTblRange[0].NumDescriptors = 1;	// �e�N�X�`��1��
	descTblRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;	// ��ʂ̓e�N�X�`��
	descTblRange[0].BaseShaderRegister = 0;	// 0�ԃX���b�g����
	// �A�������f�B�X�N���v�^�����W���A�O�̃f�B�X�N���v�^�����W�̒���ɗ���B
	descTblRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// �萔�p���W�X�^0��
	descTblRange[1].NumDescriptors = 1;	// �萔1��
	descTblRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;	// ��ʂ͒萔
	descTblRange[1].BaseShaderRegister = 0;	// 0�ԃX���b�g����
	descTblRange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// ���[�g�p�����[�^�̍쐬�B
	D3D12_ROOT_PARAMETER rootparam[2] = {};
	rootparam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	// �s�N�Z���V�F�[�_�[���猩����B
	rootparam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootparam[0].DescriptorTable.pDescriptorRanges = &descTblRange[0];	// �f�B�X�N���v�^�����W�̃A�h���X
	rootparam[0].DescriptorTable.NumDescriptorRanges = 1;	// �f�B�X�N���v�^�����W�̐�

	rootparam[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootparam[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;	// ���_�V�F�[�_�[���猩����
	rootparam[1].DescriptorTable.pDescriptorRanges = & descTblRange[1];	// �f�B�X�N���v�^�����W�̃A�h���X
	rootparam[1].DescriptorTable.NumDescriptorRanges = 1;	// �f�B�X�N���v�^�����W�̐�

	// �T���v���̐ݒ�B
	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	// �������̌J��Ԃ�
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	// �c�����̌J��Ԃ�
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	// ���s���̌J��Ԃ�
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;	// �{�[�_�[�͍�
	//samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;	// ���`�⊮
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;	// ��ԂȂ��i�j�A���X�g�l�C�o�[���j
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;	// �~�b�v�}�b�v�ő�l
	samplerDesc.MinLOD = 0.0f;	// �~�b�v�}�b�v�ŏ��l
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	// �s�N�Z���V�F�[�_�[���猩����
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;	// ���T���v�����O���Ȃ�

	// ���[�g�V�O�l�`���̐ݒ�B
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	// ���_���i���̓A�Z���u���j������B
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootparam;	// ���[�g�p�����[�^�̐擪�A�h���X
	rootSignatureDesc.NumParameters = 2;	// ���[�g�p�����[�^�̐�
	rootSignatureDesc.pStaticSamplers = &samplerDesc;	// �T���v��
	rootSignatureDesc.NumStaticSamplers = 1;	// �T���v���̐�

	// ���[�g�V�O�l�`���̃o�C�i���R�[�h�̍쐬�B
	ID3DBlob* rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob,
		&errorBlob
	);


	// ���[�g�V�O�l�`���I�u�W�F�N�g�̐����B
	result = _dev->CreateRootSignature(
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&_rootsignature)
	);

	// �s�v�ɂȂ������߉���B
	rootSigBlob->Release();

	if (FAILED(result))
	{
		// ���[�g�V�O�l�`���̐������s�B
		DebugOutputFormatString("���[�g�V�O�l�`���̐����Ɏ��s���܂����B");

		// �G���[���b�Z�[�W�̕\���B
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


	// �Z�O���t�B�b�N�X�p�C�v���C���X�e�[�g�̍쐬

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = {};
	gpipeline.pRootSignature = _rootsignature;

	// 1.�V�F�[�_�\�̃Z�b�g�B
	gpipeline.VS.pShaderBytecode = _vsBlob->GetBufferPointer();
	gpipeline.VS.BytecodeLength = _vsBlob->GetBufferSize();
	gpipeline.PS.pShaderBytecode = _psBlob->GetBufferPointer();
	gpipeline.PS.BytecodeLength = _psBlob->GetBufferSize();

	// 2.�T���v���}�X�N�ƃ��X�^���C�U�X�e�[�g�̐ݒ�B
	// �f�t�H���g�̃T���v���}�X�N�B�i0xffffffff�j
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// �܂��A�A���`�G�C���A�X�͎g��Ȃ�����false
	gpipeline.RasterizerState.MultisampleEnable = false;
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;	// �J�����O���Ȃ�
	gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;	// ���g��h��Ԃ�
	gpipeline.RasterizerState.DepthClipEnable = true;	// �[�x�����̃N���b�s���O�͗L����

	//�c��
	gpipeline.RasterizerState.FrontCounterClockwise = false;
	gpipeline.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	gpipeline.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	gpipeline.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	gpipeline.RasterizerState.AntialiasedLineEnable = false;
	gpipeline.RasterizerState.ForcedSampleCount = 0;
	gpipeline.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	// 3.�u�����h�X�e�[�g�̐ݒ�B
	// �S�̂̃u�����h�X�e�[�g�̐ݒ�B
	gpipeline.BlendState.AlphaToCoverageEnable = false;	// a�e�X�g���s�����H
	// �����̃����_�[�^�[�Q�b�g�ɂ��ꂼ��ʂ̃u�����h�X�e�[�g�����蓖�Ă邩�H
	gpipeline.BlendState.IndependentBlendEnable = false;

	// �����_�[�^�[�Q�b�g���Ƃ̃u�����h�X�e�[�g�̐ݒ�B
	D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc = {};
	// BlendEnable��LogicOpEnable�́A�����L���ɂ��邱�Ƃ͂ł��Ȃ��B�ǂ��炩�̂�true�ɂ���B
	renderTargetBlendDesc.BlendEnable = false;	// a�u�����h�Ȃǂ��s�����H
	renderTargetBlendDesc.LogicOpEnable = false;	// �_�����Z���s�����H
	renderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// �`�����ގ��̃}�X�N�l

	gpipeline.BlendState.RenderTarget[0] = renderTargetBlendDesc;

	// 4.���̓��C�A�E�g�̐ݒ�
	gpipeline.InputLayout.pInputElementDescs = inputLayout;	// ���̓��C�A�E�g�̐퓬�A�h���X
	gpipeline.InputLayout.NumElements = _countof(inputLayout);	// ���̓��C�A�E�g�̗v�f��
	// �g���C�A���O���X�g���b�v�̎��ɁA����̃C���f�b�N�X��؂藣�����߂̎w��B
	gpipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;	// �J�b�g�Ȃ�
	// �\���v�f���u�_�v�u���v�u�O�p�`�v�̂ǂꂩ���w��B
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;	// �O�p�`�ō\���B

	// 5.�����_�[�^�[�Q�b�g�̐ݒ�
	gpipeline.NumRenderTargets = 1;	// �����_�[�^�[�Q�b�g�̐��͍���1��
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	// �����_�[�^�[�Q�b�g�̃J���[�t�H�[�}�b�g

	// 6.�A���`�G�C���A�V���O�̂��߂̃T���v�����w��
	// AA���Ȃ��ݒ�B
	gpipeline.SampleDesc.Count = 1;		// �T���v������1�s�N�Z���ɂ�1
	gpipeline.SampleDesc.Quality = 0;	// �N�I���e�B�͍Œ�

	// 7.�O���t�B�b�N�X�p�C�v���C���X�e�[�g�I�u�W�F�N�g�̐���
	ID3D12PipelineState* _pipelinestate = nullptr;
	result = _dev->CreateGraphicsPipelineState(
		&gpipeline,
		IID_PPV_ARGS(&_pipelinestate)
	);

	if (FAILED(result))
	{
		// �O���t�B�b�N�X�p�C�v���C���X�e�[�g�̐������s�B
		DebugOutputFormatString("�O���t�B�b�N�X�p�C�v���C���X�e�[�g�̐����Ɏ��s���܂����B");
	}

	// �Z�r���[�|�[�g
	D3D12_VIEWPORT viewport = {};
	viewport.Width = kWindowWidth;		// �o�͐�̕�
	viewport.Height = kWindowHeight;	// �o�͐�̍���
	viewport.TopLeftX = 0;				// �o�͐�̍����X���W
	viewport.TopLeftY = 0;				// �o�͐�̍����Y���W
	viewport.MaxDepth = 1.0f;			// �ő�[�x�l
	viewport.MinDepth = 0.0f;			// �ŏ��[�x�l

	// �Z�V�U�[��`
	D3D12_RECT scissorRect = {};
	// �r���[�|�[�g�����ς��ɕ\������B
	scissorRect.top = 0;
	scissorRect.left = 0;
	scissorRect.right = scissorRect.left + kWindowWidth;
	scissorRect.bottom = scissorRect.top + kWindowHeight;


	// �E�B���h�E�\��
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

		// �Z�R�}���h�A���P�[�^�ƃR�}���h���X�g���N���A�B
		// �L���[���N���A�B
		result = _cmdAllocator->Reset();
		// �ĂуR�}���h�𗭂߂鏀���B
		result = _cmdList->Reset(_cmdAllocator, nullptr);

		// �Z�����_�[�^�[�Q�b�g�̐ݒ�
		auto bbIdx = _swapChain->GetCurrentBackBufferIndex();
		auto rtvH = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
		rtvH.ptr += static_cast<long long unsigned int>(bbIdx) * 
			_dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		// ���\�[�X�o���A�̐ݒ�
		// ���\�[�X�o���A���A�����_�[�^�[�Q�b�g�Ƃ��Ďg���悤�Ɏw��B
		D3D12_RESOURCE_BARRIER BarrierDesc =
			CD3DX12_RESOURCE_BARRIER::Transition(	// �J��
				_backBuffers[bbIdx],				// �o�b�N�o�b�t�@�\���\�[�X
				D3D12_RESOURCE_STATE_PRESENT,		// ���O��PRESENT���
				D3D12_RESOURCE_STATE_RENDER_TARGET	// �����烌���_�[�^�[�Q�b�g���
			);
		//BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;	// �J��
		//BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		//BarrierDesc.Transition.pResource = _backBuffers[bbIdx];	// �o�b�N�o�b�t�@�\���\�[�X
		//BarrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		//// ���O��PRESENT���
		//BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		//// �����烌���_�[�^�[�Q�b�g���
		//BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

		// �o���A�w����s
		_cmdList->ResourceBarrier(1, &BarrierDesc);
		
		_cmdList->SetPipelineState(_pipelinestate);

		_cmdList->OMSetRenderTargets(1, &rtvH, true, nullptr);

		// �Z�����_�[�^�[�Q�b�g�̃N���A
		constexpr float clearColor[] = { 1.0f,1.0f,0.0f,1.0f };	// ���F
		_cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

		// �Z���߂Ă��������߂̎��s
		_cmdList->RSSetViewports(1, &viewport);
		_cmdList->RSSetScissorRects(1, &scissorRect);
		_cmdList->SetGraphicsRootSignature(_rootsignature);

		_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		_cmdList->IASetVertexBuffers(0, 1, &vbView);
		_cmdList->IASetIndexBuffer(&ibView);

		_cmdList->SetGraphicsRootSignature(_rootsignature);
		_cmdList->SetDescriptorHeaps(1, &basicDescHeap);

		// �q�[�v�̐擪�A�h���X���擾�B
		auto heapHandle = basicDescHeap->GetGPUDescriptorHandleForHeapStart();
		_cmdList->SetGraphicsRootDescriptorTable(
			0,	// ���[�g�p�����[�^�C���f�b�N�X
			heapHandle	// �q�[�v�A�h���X
		);

		// ���̔ԍ��̃��[�g�p�����[�^�p�̃q�[�v�A�h���X�֐i�߂�B
		heapHandle.ptr += 
			_dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		_cmdList->SetGraphicsRootDescriptorTable(
			1,	// ���[�g�p�����[�^�C���f�b�N�X
			heapHandle	// �q�[�v�A�h���X
		);

		_cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);


		// ���\�[�X�o���A��PRESENT�ɂ���B
		BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		// �o���A�w����s
		_cmdList->ResourceBarrier(1, &BarrierDesc);

		// ���߂̎��s�O�ɁA�K���R�}���h���X�g�̃N���[�Y���s���B
		_cmdList->Close();

		// �R�}���h���X�g�̎��s�B
		ID3D12CommandList* cmdLists[] = { _cmdList };
		_cmdQueue->ExecuteCommandLists(1, cmdLists);

		// ���s����������܂ő҂B
		_cmdQueue->Signal(_fence, ++_fenceVal);
		if (_fence->GetCompletedValue() != _fenceVal)
		{
			// �C�x���g�n���h�����擾�B
			auto eventH = CreateEvent(nullptr, false, false, nullptr);

			_fence->SetEventOnCompletion(_fenceVal, eventH);

			// �C�x���g����������܂ő҂�������B
			WaitForSingleObject(eventH, INFINITE);

			CloseHandle(eventH);
		}

		// �Z��ʂ̃X���b�v
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

	// �����N���X�͎g��Ȃ��̂ŁA�o�^��������B
	UnregisterClass(w.lpszClassName, w.hInstance);

	return 0;
}