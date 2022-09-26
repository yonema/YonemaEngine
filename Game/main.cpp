#include "YonemaEngine/Windows/Window.h"

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

/**
 * @brief �萔�o�b�t�@�ő��邽�߂̃}�g���N�X�f�[�^
*/
struct SSceneMatrix
{
	DirectX::XMMATRIX mWorldViewProj = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX mWorld = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX mView = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX mProj = DirectX::XMMatrixIdentity();
	DirectX::XMFLOAT3 cameraPosWS;
};


// 1�o�C�g�p�b�L���O�ɂ��āA�A���C�����g�ɂ��T�C�Y�̂����h���B
// ���������͗����邪�A�ǂݍ��ނ͍̂ŏ������ł���A����Ɍ�ŕʂ̍\���̂ɃR�s�[���邩��
// ���܂���͂Ȃ��Ǝv���B
#pragma pack(1)
/**
 * @brief PMD�t�@�C���̃}�e���A���f�[�^
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
	DirectX::XMFLOAT3 diffuse;
	float alpha = 0.0f;
	DirectX::XMFLOAT3 specular;
	float specularity = 0.0f;
	DirectX::XMFLOAT3 ambient;
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

std::unordered_map<std::string, ID3D12Resource*> g_resourceTable;

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
 * @brief ���f���̃p�X�ƃe�N�X�`���̃p�X���獇���p�X�𓾂�
 * @param[in] modelPath �A�v���P�[�V�������猩��pmd���f���̃p�X
 * @param[in] texPath pmd���f�����猩���e�N�X�`���̃p�X
 * @return �A�v���P�[�V�������猩���e�N�X�`���̃p�X
*/
std::string GetTexturePathFromModelAndTexPath(
	const std::string& modelPath,
	const char* const texPath
)
{
	// rfind()���A�߂�lsite_t�̂�����size_t�Ŏ󂯎��ƁA
	// ������������Ȃ������Ƃ���-1�i���̐��j�Ŏ󂯎��Ȃ��B
	// ������A�L���X�g����int�Ŏ󂯎��B
	int pathIndex1 = static_cast<int>(modelPath.rfind('/'));
	int pathIndex2 = static_cast<int>(modelPath.rfind('\\'));

	// '/'��'\\'�̂ǂ��炩�L���Ȃق����̗p����B
	auto pathIndex = max(pathIndex1, pathIndex2);

	// ���̂܂܂��ƍŌ��'/'������Ȃ�����A��i�߂�B
	pathIndex++;
	auto folderPath = modelPath.substr(0, pathIndex);

	return folderPath + texPath;
}

/**
 * @brief �t�@�C��������g���q���擾����B
 * @param[in] path �Ώۂ̃t�@�C�����i�t�@�C���p�X�ł��j
 * @return �g���q
*/
std::string GetExtension(const std::string& fileName)
{
	int idx = static_cast<int>(fileName.rfind('.'));

	// ���̂܂܂���'.'�������Ă��܂�����A��i�߂�B
	idx++;
	return fileName.substr(idx, fileName.length() - idx);
}

/**
 * @brief �t�@�C�������Z�p���[�^�����ŕ�������
 * @param[in] path �Ώۂ̃t�@�C�����i�t�@�C���p�X�͕s�j
 * @param[in] spliter ��؂蕶��
 * @return �����O��̕�����y�A
*/
std::pair<std::string, std::string> SplitFilename(
	const std::string& texPath,
	const char spliter = '*'
)
{
	int idx = static_cast<int>(texPath.find(spliter));

	std::pair<std::string, std::string> ret;
	ret.first = texPath.substr(0, idx);
	// '*'���܂߂Ȃ����߂Ɉ�i�߂�B
	idx++;
	ret.second = texPath.substr(idx, texPath.length() - idx);

	return ret;
}

/**
 * @brief str����wstrin�ɕϊ�����
 * @param[in] str �ϊ�����str������
 * @return �ϊ����ꂽwstr������
*/
std::wstring GetWideStringFromString(const std::string& str)
{
	// �Ăяo��1���
	// �������𓾂邽�߁B
	auto num1 = MultiByteToWideChar(
		CP_ACP,
		MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
		str.c_str(),
		-1,
		nullptr,
		0
	);

	// ����ꂽ�������Ń��T�C�Y�B
	std::wstring wstr;
	wstr.resize(num1);

	// �Ăяo��2���
	// �m�ۍς݂�wstr�ɕϊ���������R�s�[�B
	auto num2 = MultiByteToWideChar(
		CP_ACP,
		MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
		str.c_str(),
		-1,
		&wstr[0],
		num1
	);

	// �ꉞ�`�F�b�N�B
	assert(num1 == num2);

	return wstr;
}

/**
 * @brief �e�N�X�`���̃��[�h
 * @param[in] texPath �e�N�X�`���̃t�@�C���p�X
 * @return �e�N�X�`���̃��\�[�X�B���[�h�ł��Ȃ����nullptr�B
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

	// WIC�e�N�X�`���̃��[�h

	// �摜�t�@�C���Ɋւ�����B
	DirectX::TexMetadata metadata = {};
	// ���ۂ̃f�[�^�B
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
		// �Ή������g���q��������Ȃ��B
		DebugOutputFormatString("�e�N�X�`���̃��[�h�Ɏ��s���܂����B");
		DebugOutputFormatString("�Ή������g���q��������܂���B");
		DebugOutputFormatString(texPath.c_str());
		return nullptr;
	}


	if (FAILED(result))
	{
		// �e�N�X�`���̃��[�h���s�B
		DebugOutputFormatString("�e�N�X�`���̃��[�h�Ɏ��s���܂����B");
		DebugOutputFormatString(texPath.c_str());
		return nullptr;
	}

	auto img = scratchImg.GetImage(0, 0, 0);	// ���f�[�^���o

	// �o�b�t�@�p256�̔{���ŃA���C�����g���ꂽrowPitch�B
	const auto alignmentedRowPitch = 
		AlignmentedSize(img->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);

	// 1.�A�b�v���[�h�p���\�[�X�̍쐬

	// ���ԃo�b�t�@�Ƃ��ẴA�b�v���[�h�q�[�v�̐ݒ�B
	D3D12_HEAP_PROPERTIES uploadHeapProp =
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	// ���\�[�X�̐ݒ�B
	UINT64 resDescWidth = static_cast<UINT64>(alignmentedRowPitch * img->height);
	D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(resDescWidth);

	// ���ԃo�b�t�@�̍쐬�B
	ID3D12Resource* uploadBuff = nullptr;
	result = dev->CreateCommittedResource(
		&uploadHeapProp,
		D3D12_HEAP_FLAG_NONE,	// ���ɂȂ�
		&resDesc,
		// CPU����}�b�v���邽�߃��\�[�X�X�e�[�g���ACPU����`�����݉\����GPU����͓ǂݎ��݂̂ɂ���B
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&uploadBuff)
	);

	if (FAILED(result))
	{
		// �e�N�X�`���̃A�b�v���[�h���\�[�X�̐������s�B
		DebugOutputFormatString("�e�N�X�`���̃A�b�v���[�h���\�[�X�̐����Ɏ��s���܂����B");
		return nullptr;
	}

	// 2.�ǂݏo���p���\�[�X�̍쐬
	
	// �e�N�X�`���̂��߂̃q�[�v�ݒ�B
	D3D12_HEAP_PROPERTIES texHeapProp =
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);	// �e�N�X�`���p

	// ���\�[�X�ݒ�i�ϐ��͂����܂킵�j
	resDesc.Format = metadata.format;
	resDesc.Width = static_cast<UINT64>(metadata.width);
	resDesc.Height = static_cast<UINT>(metadata.height);
	resDesc.DepthOrArraySize = static_cast<UINT16>(metadata.arraySize);
	resDesc.MipLevels = static_cast<UINT16>(metadata.mipLevels);
	resDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;	// ���C�A�E�g�͌��肵�Ȃ�

	// �e�N�X�`���o�b�t�@�̍쐬�B
	ID3D12Resource* texBuff = nullptr;
	result = dev->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE,	// ���Ɏw��Ȃ�
		&resDesc,
		D3D12_RESOURCE_STATE_COPY_DEST, // �R�s�[��
		nullptr,
		IID_PPV_ARGS(&texBuff)
	);


	if (FAILED(result))
	{
		// �e�N�X�`���o�b�t�@�̐������s�B
		DebugOutputFormatString("�e�N�X�`���o�b�t�@�̐����Ɏ��s���܂����B");

		uploadBuff->Release();
		return nullptr;
	}


	// 3.�A�b�v���[�h�p���\�[�X�փe�N�X�`���f�[�^��Map()�ŃR�s�[

	uint8_t* mapforImg = nullptr;	// image->pixels�Ɠ����^�ɂ���
	result = uploadBuff->Map(0, nullptr, reinterpret_cast<void**>(&mapforImg));

	// �o�b�t�@��rowPitch��256�̔{���łȂ���΂Ȃ�Ȃ����߁A�A���C�����g����Ă���B
	// ���̂܂܃R�s�[����ƁA�o�b�t�@��rowPitch�ƌ��f�[�^��rowPitch�ƍ��ł���Ă��܂��B
	// ���̂��߁A1�s���ƂɃR�s�[���čs���������悤�ɂ���B

	auto srcAddress = img->pixels;

	for (int y = 0; y < img->height; y++)
	{
		// 1�s���R�s�[����B
		std::copy_n(srcAddress, alignmentedRowPitch, mapforImg);
		// �s�������킹��B
		srcAddress += img->rowPitch;	// ���f�[�^�͎��ۂ�rowPitch�������i�߂�
		mapforImg += alignmentedRowPitch;	// �o�b�t�@�̓A���C�����g���ꂽrowPitch�������i�߂�
	}

	uploadBuff->Unmap(0, nullptr);


	// 4.�A�b�v���[�h�p���\�[�X����ǂݏo���p���\�[�X��CopyTextureRegion()�ŃR�s�[

	// �O���t�B�b�N�{�[�h��̃R�s�[���A�h���X�B
	D3D12_TEXTURE_COPY_LOCATION src = {};
	// �R�s�[���i�A�b�v���[�h���j�̐ݒ�B
	src.pResource = uploadBuff;	// ���ԃo�b�t�@
	// �A�b�v���[�h�o�b�t�@�ɂ́A�t�b�g�v�����g�i��������L�̈�Ɋւ�����j�w��B
	src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	src.PlacedFootprint.Offset = 0;
	src.PlacedFootprint.Footprint.Width = static_cast<UINT>(metadata.width);
	src.PlacedFootprint.Footprint.Height = static_cast<UINT>(metadata.height);
	src.PlacedFootprint.Footprint.Depth = static_cast<UINT>(metadata.depth);
	// RowPitch��256�̔{���łȂ���΂Ȃ�Ȃ��B
	src.PlacedFootprint.Footprint.RowPitch = static_cast<UINT>(alignmentedRowPitch);
	src.PlacedFootprint.Footprint.Format = img->format;

	// �O���t�B�b�N�{�[�h��̃R�s�[��A�h���X�B
	D3D12_TEXTURE_COPY_LOCATION dst = {};
	// �R�s�[��i�e�N�X�`���o�b�t�@�j�̐ݒ�B
	dst.pResource = texBuff;
	// �e�N�X�`���o�b�t�@�ɂ́A�C���f�b�N�X�w��B
	dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	dst.SubresourceIndex = 0;

	result = cmdAllocator->Reset();
	// �ĂуR�}���h�𗭂߂鏀���B
	result = cmdList->Reset(cmdAllocator, nullptr);

	cmdList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

	// �e�N�X�`���p���\�[�X���A�R�s�[��̂܂܂̂��߁A�e�N�X�`���p�w��ɕύX����B
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

	// �R�s�[���������������Ȃ��B
	uploadBuff->Release();

	g_resourceTable.emplace(texPath, texBuff);

	return texBuff;

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


	constexpr int kWindowWidth = 1280;
	constexpr int kWindowHeight = 720;

	nsYMEngine::nsWindows::CWindows window(kWindowWidth, kWindowHeight);




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
	_cmdList->Close();

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
		window.GetHWND(),
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
	//rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	// �K���}�␳����isRGB�j
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// �K���}�␳����isRGB�j
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

	// �Z�[�x�o�b�t�@�̍쐬
	D3D12_RESOURCE_DESC depthResDesc = {};
	depthResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResDesc.Width = kWindowWidth;
	depthResDesc.Height = kWindowHeight;
	depthResDesc.DepthOrArraySize = 1;
	depthResDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthResDesc.SampleDesc.Count = 1;
	depthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	// �[�x�l�p�q�[�v�v���p�e�B
	D3D12_HEAP_PROPERTIES depthHeapProp = {};
	// DEFAULT�̂��߁A��̐ݒ��UNKNOWN�ł悢�B
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	depthHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	depthHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	// ���̃N���A�o�����[���d�v�ȈӖ�������
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
		// �[�x�X�e���V���o�b�t�@�̐������s�B
		DebugOutputFormatString("�[�x�X�e���V���o�b�t�@�̐����Ɏ��s���܂����B");
	}

	// �Z�[�x�o�b�t�@�r���[�̍쐬
	
	// �f�B�X�N���v�^�q�[�v�̍쐬
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	ID3D12DescriptorHeap* dsvHeap = nullptr;
	result = _dev->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));

	if (FAILED(result))
	{
		// �[�x�o�b�t�@�r���[�p�̃f�B�X�N���v�^�q�[�v�̐������s�B
		DebugOutputFormatString("�[�x�o�b�t�@�r���[�p�̃f�B�X�N���v�^�q�[�v�̐����Ɏ��s���܂����B");
	}

	// �[�x�r���[�̍쐬
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	_dev->CreateDepthStencilView(
		depthBuffer,
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart()
	);




	// ������
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

	// �ZPMD�t�@�C���̓ǂݍ���

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

	//std::string strModelPath = "Assets/Models/Samples/�����~�N.pmd";
	std::string strModelPath = "Assets/Models/Samples/�����~�Nmetal.pmd";
	//std::string strModelPath = "Assets/Models/Samples/�������J.pmd";

	if (fopen_s(&fileStream, strModelPath.c_str(), "rb") == 0)
	{
		// �w�b�_�̓ǂݍ���
		char signature[3] = {};
		SPMDHeader pmdHeader = {};
		fread_s(signature, sizeof(signature), sizeof(signature), 1, fileStream);
		fread_s(&pmdHeader, sizeof(pmdHeader), sizeof(pmdHeader), 1, fileStream);

		// ���_���̓ǂݍ���
		fread_s(&pmdVertNum, sizeof(pmdVertNum), sizeof(pmdVertNum), 1, fileStream);

		// ���_�f�[�^�̓ǂݍ���
		pmdVertices.resize(pmdVertNum * kPmdVertexSize);
		fread_s(pmdVertices.data(), pmdVertices.size(), pmdVertices.size(), 1, fileStream);

		// �C���f�b�N�X���̓ǂݍ���
		fread_s(&pmdIndicesNum, sizeof(pmdIndicesNum), sizeof(pmdIndicesNum), 1, fileStream);

		// �C���f�b�N�X�f�[�^�̓ǂݍ���
		pmdIndices.resize(pmdIndicesNum);
		pmdIndicesSize = pmdIndices.size() * sizeof(pmdIndices[0]);
		fread_s(pmdIndices.data(), pmdIndicesSize, pmdIndicesSize, 1, fileStream);

		// �}�e���A�����̓ǂݍ���
		fread_s(&pmdMaterialNum, sizeof(pmdMaterialNum), sizeof(pmdMaterialNum), 1, fileStream);

		// �}�e���A���f�[�^�̓ǂݍ���
		pmdMaterials.resize(pmdMaterialNum);
		pmdMaterialSize = pmdMaterials.size() * sizeof(SPMDMaterial);
		fread_s(pmdMaterials.data(), pmdMaterialSize, pmdMaterialSize, 1, fileStream);


		fclose(fileStream);
	}
	else
	{
		// PMD�t�@�C���̓ǂݍ��ݎ��s�B
		DebugOutputFormatString("PMD�t�@�C���̓ǂݍ��݂Ɏ��s���܂����B");
	}

	// �ǂݍ��񂾃}�e���A���f�[�^��]���p�̃}�e���A���f�[�^�ɃR�s�[����B
	std::vector<SMaterial> materials(pmdMaterials.size());
	std::vector<ID3D12Resource*> textureResources(pmdMaterials.size());
	std::vector<ID3D12Resource*> sphResources(pmdMaterials.size());
	std::vector<ID3D12Resource*> spaResources(pmdMaterials.size());

	for (int i = 0; i < pmdMaterials.size(); i++)
	{
		// �}�e���A���f�[�^�̃R�s�[�B
		materials[i].indicesNum = pmdMaterials[i].indicesNum;
		materials[i].matForHlsl.diffuse = pmdMaterials[i].diffuse;
		materials[i].matForHlsl.alpha = pmdMaterials[i].alpha;
		materials[i].matForHlsl.specular = pmdMaterials[i].specular;
		materials[i].matForHlsl.specularity = pmdMaterials[i].specularity;
		materials[i].matForHlsl.ambient = pmdMaterials[i].ambient;

		// �e�N�X�`���̃��[�h

		// �ŏ��ɑS���̃��\�[�X��nullptr�����Ă����B
		// ���\�[�X������Ȃ�A���Ƃ�������B
		textureResources[i] = nullptr;
		sphResources[i] = nullptr;
		spaResources[i] = nullptr;

		if (strlen(pmdMaterials[i].texFilePath) == 0)
		{
			// �e�N�X�`���Ȃ��B
			continue;
		}

		// �e�N�X�`������B

		std::string texFileName = pmdMaterials[i].texFilePath;
		if (std::count(texFileName.begin(), texFileName.end(), '*') > 0)
		{
			// �X�v���b�^����B

			auto namePair = SplitFilename(texFileName);
			std::string otherTexFileName;

			// �X�t�B�A�}�b�v����Ȃ��ق����e�N�X�`�����ɂ���B
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

			// �X�t�B�A�}�b�v�̕��̃��[�h�B

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


	// ���[���h�s��v�Z
	DirectX::XMMATRIX mWorld = DirectX::XMMatrixRotationY(DirectX::XM_PIDIV4);

	// �r���[�s��v�Z
	DirectX::XMFLOAT3 eyePos(0.0f, 10.0f, -15.0f);
	DirectX::XMFLOAT3 targetPos(0.0f, 10.0f, 0.0f);
	DirectX::XMFLOAT3 upVec(0.0f, 1.0f, 0.0f);
	DirectX::XMMATRIX mView = DirectX::XMMatrixLookAtLH(
		DirectX::XMLoadFloat3(&eyePos),
		DirectX::XMLoadFloat3(&targetPos),
		DirectX::XMLoadFloat3(&upVec)
	);

	// �v���W�F�N�V�����s��v�Z
	DirectX::XMMATRIX mProj = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XM_PIDIV2,
		static_cast<float>(kWindowWidth) / static_cast<float>(kWindowHeight),
		1.0f,
		100.0f
	);

	

	// �Z�萔�o�b�t�@�̐����B
	ID3D12Resource* constBuff = nullptr;
	// Map�Œ��g�����������邽��UPLOAD�B
	D3D12_HEAP_PROPERTIES constBuffHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// �K�v�ȃo�C�g����256�ŃA���C�����g�����o�C�g�����K�v�B
	// 0xff = 255
	// �ŏ���0xff�𑫂���256�ȏ�ɂ��Ă���A�r�b�g���]����0xff��AND���Z���邱�ƂŁA
	// size�𒴂����ŏ���256�̔{�����v�Z����B
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
		// �萔�o�b�t�@�̐������s�B
		DebugOutputFormatString("�萔�o�b�t�@�̐����Ɏ��s���܂����B");
	}

	// �萔�o�b�t�@�̃R�s�[
	SSceneMatrix* mapMatrix = nullptr;	// �}�b�v��̃|�C���^�[
	result = constBuff->Map(0, nullptr, reinterpret_cast<void**>(&mapMatrix));	// �}�b�v
	// �s��̓��e���R�s�[�B
	mapMatrix->mWorldViewProj = mWorld * mView * mProj;
	mapMatrix->mWorld = mWorld;
	mapMatrix->mView = mView;
	mapMatrix->mProj = mProj;
	mapMatrix->cameraPosWS = eyePos;

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

	// �f�B�X�N���v�^�q�[�v�̐擪�A�h���X���擾�B
	// �擪�A�h���X�̓V�F�[�_�[���\�[�X�r���[�B
	auto basicHeapHandle = basicDescHeap->GetCPUDescriptorHandleForHeapStart();

	// �Z�萔�o�b�t�@�r���[�̐����B

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = constBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = static_cast<UINT>(constBuff->GetDesc().Width);

	// �r���[�̍쐬�B
	_dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);


	// �Z���_�o�b�t�@�̍쐬

	// UPLOAD�q�[�v�Ƃ��Ă̐ݒ�
	D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	// �T�C�Y�ɉ����ēK�؂Ȑݒ�����Ă����B
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
		// ���_�o�b�t�@�̐������s�B
		DebugOutputFormatString("���_�o�b�t�@�̐����Ɏ��s���܂����B");
	}

	// �Z���_���̃R�s�[�i�}�b�v�j
	unsigned char* vertMap = nullptr;
	// GPU�̃o�b�t�@�[�́i���z�j�A�h���X���擾�B
	result = vertBuff->Map(0, nullptr, reinterpret_cast<void**>(&vertMap));
	// �}�b�v���Ă����o�b�t�@�[�ɁA���_�f�[�^���������ށB
	std::copy(std::begin(pmdVertices), std::end(pmdVertices), vertMap);
	// �}�b�v�����B
	vertBuff->Unmap(0, nullptr);

	// �Z���_�o�b�t�@�r���[�̍쐬
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();	// �o�b�t�@�[�̉��z�A�h���X
	vbView.SizeInBytes = static_cast<UINT>(pmdVertices.size());	// �S�o�C�g��
	vbView.StrideInBytes = static_cast<UINT>(kPmdVertexSize);	// 1���_������̃o�C�g��

	// �Z�C���f�b�N�X�o�b�t�@�̍쐬

	unsigned short indeices[] =
	{
		// ���v���
		0,1,2,
		2,1,3
	};

	ID3D12Resource* idxBuff = nullptr;
	// �ݒ�́A�o�b�t�@�̃T�C�Y�ȊO�A���_�o�b�t�@�̐ݒ���g���܂킵�Ă悢�B
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
		// �C���f�b�N�X�o�b�t�@�̐������s�B
		DebugOutputFormatString("�C���f�b�N�X�o�b�t�@�̐����Ɏ��s���܂����B");
	}

	// ������o�b�t�@�[�ɃC���f�b�N�X�f�[�^���R�s�[
	unsigned short* mappedIdx = nullptr;
	idxBuff->Map(0, nullptr, reinterpret_cast<void**>(&mappedIdx));
	std::copy(std::begin(pmdIndices), std::end(pmdIndices), mappedIdx);
	idxBuff->Unmap(0, nullptr);

	// �C���f�b�N�X�o�b�t�@�r���[���쐬
	D3D12_INDEX_BUFFER_VIEW ibView = {};
	ibView.BufferLocation = idxBuff->GetGPUVirtualAddress();
	// unsigned int �ŃC���f�b�N�X�z����g�p���Ă��邽�߁ADXGI_FORMAT_R16_UINT���g�p�B
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = static_cast<UINT>(pmdIndicesSize);


	// �Z�}�e���A���o�b�t�@�̍쐬
	UINT materialBuffSize = sizeof(SMaterialForHlsl);
	materialBuffSize = (materialBuffSize + 0xff) & ~0xff;

	ID3D12Resource* materialBuff = nullptr;
	D3D12_HEAP_PROPERTIES matHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// ���������Ȃ����d�����Ȃ��B
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

	// �}�b�v�}�e���A���ɃR�s�[
	char* mapMaterial = nullptr;
	result = materialBuff->Map(0, nullptr, reinterpret_cast<void**>(&mapMaterial));
	for (auto& m : materials)
	{
		// �f�[�^�̃R�s�[
		*reinterpret_cast<SMaterialForHlsl*>(mapMaterial) = m.matForHlsl;
		// ���̃A���C�����g�ʒu�܂Ői�߂�B�i256�̔{���j
		mapMaterial += materialBuffSize;
	}
	materialBuff->Unmap(0, nullptr);

	// �}�e���A���p�f�B�X�N���v�^�q�[�v�ƃr���[�̍쐬
	ID3D12DescriptorHeap* matDescHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC matDescHeapDesc = {};
	matDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	matDescHeapDesc.NodeMask = 0;
	// �}�e���A���f�[�^�ƃe�N�X�`��*3��4���B
	matDescHeapDesc.NumDescriptors = pmdMaterialNum * 4;
	matDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	result = _dev->CreateDescriptorHeap(
		&matDescHeapDesc,
		IID_PPV_ARGS(&matDescHeap)
	);

	if (FAILED(result))
	{
		// �}�e���A���p�̃f�B�X�N���v�^�q�[�v�̐������s�B
		DebugOutputFormatString("�}�e���A���p�̃f�B�X�N���v�^�q�[�v�̐����Ɏ��s���܂����B");
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
		// �擪���L�^
		auto matDescHeapH = matDescHeap->GetCPUDescriptorHandleForHeapStart();
		const auto inc = 
			_dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		for (int i = 0; i < static_cast<int>(pmdMaterialNum); i++)
		{
			// �}�e���A���f�[�^�p�萔�o�b�t�@�r���[�B
			_dev->CreateConstantBufferView(&matCBVDesc, matDescHeapH);

			matDescHeapH.ptr += inc;
			matCBVDesc.BufferLocation += materialBuffSize;

			// �e�N�X�`���p�V�F�[�_�[���\�[�X�r���[�B
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
	//D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	//{
	//	// ���W���
	//	{
	//		"POSITION",	// �Z�}���e�B�N�X��
	//		0,
	//		DXGI_FORMAT_R32G32B32_FLOAT,	// �t�H�[�}�b�g
	//		0,	// ���̓X���b�g�̃C���f�b�N�X
	//		D3D12_APPEND_ALIGNED_ELEMENT,	// �f�[�^�̃I�t�Z�b�g�ʒu
	//		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
	//		0	// ��x�ɕ`�悷��C���X�^���X�̐�
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

	// �Z���[�g�V�O�l�`���̐���
	ID3D12RootSignature* _rootsignature = nullptr;

	// �f�B�X�N���v�^�����W
	constexpr unsigned int descTableRangeNum = 3;
	D3D12_DESCRIPTOR_RANGE descTblRange[descTableRangeNum] = {};

	// �萔�p���W�X�^0��
	descTblRange[0] = CD3DX12_DESCRIPTOR_RANGE(
		D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
		1,
		0
	);

	// �}�e���A���p���W�X�^1��
	// �f�B�X�N���v�^�q�[�v�͕������邪�A��x�Ɏg���͈̂�B
	descTblRange[1] = CD3DX12_DESCRIPTOR_RANGE(
		D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
		1,
		1
	);

	// �e�N�X�`���p�i�}�e���A���ƃZ�b�g�j���W�X�^0��
	descTblRange[2] = CD3DX12_DESCRIPTOR_RANGE(
		D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
		3,
		0
	);

	// ���[�g�p�����[�^�̍쐬�B
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


	// �T���v���̐ݒ�B
	D3D12_STATIC_SAMPLER_DESC samplerDesc =
		CD3DX12_STATIC_SAMPLER_DESC(
			0,
			//D3D12_FILTER_MIN_MAG_MIP_LINEAR,	// ���`�⊮
			//D3D12_FILTER_MIN_MAG_MIP_POINT,	// ��ԂȂ��i�j�A���X�g�l�C�o�[���j
			D3D12_FILTER_ANISOTROPIC,			// �ٕ����t�B���^�����O
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			0,
			16,
			//D3D12_COMPARISON_FUNC_LESS_EQUAL,
			D3D12_COMPARISON_FUNC_NEVER,	// ���T���v�����O���Ȃ�
			D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK,
			0.0f,
			D3D12_FLOAT32_MAX,
			D3D12_SHADER_VISIBILITY_PIXEL,	// �s�N�Z���V�F�[�_�[���猩����
			0
		);

	// ���[�g�V�O�l�`���̐ݒ�B
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc =
		CD3DX12_ROOT_SIGNATURE_DESC(
			rootparamNum,
			rootparam,
			1,
			&samplerDesc,
			// ���_���i���̓A�Z���u���j������B
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
		);

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

	gpipeline.VS = CD3DX12_SHADER_BYTECODE(_vsBlob);
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(_psBlob);

	// 2.�T���v���}�X�N�ƃ��X�^���C�U�X�e�[�g�̐ݒ�B
	// �f�t�H���g�̃T���v���}�X�N�B�i0xffffffff�j
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	D3D12_RASTERIZER_DESC rasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	gpipeline.RasterizerState = rasterizerDesc;


	// 3.�u�����h�X�e�[�g�̐ݒ�B
	// �S�̂̃u�����h�X�e�[�g�̐ݒ�B
	D3D12_BLEND_DESC blendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	gpipeline.BlendState = blendDesc;

	// 4.���̓��C�A�E�g�̐ݒ�
	gpipeline.InputLayout.pInputElementDescs = inputLayout;	// ���̓��C�A�E�g�̐퓬�A�h���X
	gpipeline.InputLayout.NumElements = _countof(inputLayout);	// ���̓��C�A�E�g�̗v�f��
	// �g���C�A���O���X�g���b�v�̎��ɁA����̃C���f�b�N�X��؂藣�����߂̎w��B
	gpipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;	// �J�b�g�Ȃ�
	// �\���v�f���u�_�v�u���v�u�O�p�`�v�̂ǂꂩ���w��B
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;	// �O�p�`�ō\���B

	// 5.�����_�[�^�[�Q�b�g�̐ݒ�
	gpipeline.NumRenderTargets = 1;	// �����_�[�^�[�Q�b�g�̐��͍���1��
	//gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	// �����_�[�^�[�Q�b�g�̃J���[�t�H�[�}�b�g
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;	// �����_�[�^�[�Q�b�g�̃J���[�t�H�[�}�b�g


	// 6.�A���`�G�C���A�V���O�̂��߂̃T���v�����w��
	// AA���Ȃ��ݒ�B
	gpipeline.SampleDesc.Count = 1;		// �T���v������1�s�N�Z���ɂ�1
	gpipeline.SampleDesc.Quality = 0;	// �N�I���e�B�͍Œ�

	// �f�v�X�X�e���V���o�b�t�@�̐ݒ�
	gpipeline.DepthStencilState.DepthEnable = true;
	gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

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
	D3D12_VIEWPORT viewport = CD3DX12_VIEWPORT(
		0.0f,
		0.0f,
		kWindowWidth,
		kWindowHeight
	);

	// �Z�V�U�[��`
	// �r���[�|�[�g�����ς��ɕ\������B
	D3D12_RECT scissorRect = CD3DX12_RECT(
		0,
		0,
		0 + kWindowWidth,
		0 + kWindowHeight
	);

	float angle = 0.0f;

	// �E�B���h�E�\��
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

		auto dsvH = dsvHeap->GetCPUDescriptorHandleForHeapStart();


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

		_cmdList->OMSetRenderTargets(1, &rtvH, true, &dsvH);

		// �Z�����_�[�^�[�Q�b�g�̃N���A
		constexpr float clearColor[] = { 0.5f,0.5f,0.5f,1.0f };
		_cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
		// �Z�f�v�X�X�e���V���o�b�t�@�̃N���A
		_cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		// �Z���߂Ă��������߂̎��s
		_cmdList->RSSetViewports(1, &viewport);
		_cmdList->RSSetScissorRects(1, &scissorRect);
		_cmdList->SetGraphicsRootSignature(_rootsignature);

		_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
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

		_cmdList->SetDescriptorHeaps(1, &matDescHeap);

		{
			// �q�[�v�擪�B
			auto materialH = matDescHeap->GetGPUDescriptorHandleForHeapStart();
			// �ŏ��̓I�t�Z�b�g�Ȃ��B
			unsigned int idxOffset = 0;
			const auto CBVAndSRVInc = 
				_dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
				* 4;

			for (auto& m : materials)
			{

				_cmdList->SetGraphicsRootDescriptorTable(1, materialH);
				_cmdList->DrawIndexedInstanced(m.indicesNum, 1, idxOffset, 0, 0);

				// �q�[�v�|�C���^�ƃC���f�b�N�X�����ɐi�߂�B
				materialH.ptr += CBVAndSRVInc;					
				idxOffset += m.indicesNum;
			}
		}

		

		
		//_cmdList->DrawInstanced(pmdVertNum, 1, 0, 0);


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