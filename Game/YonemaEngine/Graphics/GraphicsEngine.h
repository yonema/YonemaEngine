#pragma once
namespace nsYMEngine
{
	namespace nsGraphics
	{
		class CTexture;
		namespace nsPMDModels
		{
			class CPMDGenericRenderer;
		}
	}
}

namespace nsYMEngine
{
	namespace nsGraphics
	{
		class CGraphicsEngine : private nsUtil::SNoncopyable
		{
		private:
			enum EnGPUAdapterType
			{
				// è„Ç©ÇÁégópóDêÊìxèá
				enNVidia,
				enAMD,
				enIntel,
				NumGPUAdapterType
			};
			static const wchar_t* m_kGPUAdapterNames[EnGPUAdapterType::NumGPUAdapterType];
			static const unsigned int m_kNumFeatureLevel = 4;
			static const D3D_FEATURE_LEVEL m_kFeatureLevels[m_kNumFeatureLevel];
			static const unsigned int m_kFrameBufferCount = 2;
			static const DXGI_FORMAT m_kDepthFormat;
			static const float m_kRTVClearColor[4];

			struct SSceneDataMatrix
			{
				DirectX::XMFLOAT4X4 mView;
				DirectX::XMFLOAT4X4 mProj;
				DirectX::XMFLOAT3 cameraPosWS;
			};

		private:
			CGraphicsEngine() = default;
			~CGraphicsEngine();

		public:
			
			inline static CGraphicsEngine* CreateInstance()
			{
				if (m_instance == nullptr)
				{
					m_instance = new CGraphicsEngine();
				}

				return m_instance;
			}
			inline static void DeleteInstance()
			{
				if (m_instance != nullptr)
				{
					delete m_instance;
				}
			}
			inline static CGraphicsEngine* GetInstance()
			{
				return m_instance;
			}

			bool Init();

			void Update();

			void BeginDraw();

			void EndDraw();

			inline ID3D12Device5* GetDevice()
			{
				return m_device;
			}

			inline ID3D12CommandAllocator* GetCommandAllocator()
			{
				return m_commandAllocator;
			}

			inline ID3D12GraphicsCommandList* GetCommandList()
			{
				return m_commandList;
			}

			inline ID3D12CommandQueue* GetCommandQueue()
			{
				return m_commandQueue;
			}

			inline ID3D12Fence* GetFence()
			{
				return m_fence;
			}

			inline UINT64* GetFenceVal()
			{
				return &m_fenceVal;
			}

			CTexture* GetWhiteTexture()
			{
				return m_whiteTexture;
			}

			CTexture* GetBlackTexture()
			{
				return m_blackTexture;
			}

			DirectX::XMFLOAT4X4 GetMatrixViewProj() const
			{
				DirectX::XMFLOAT4X4 lm;
				auto mView = DirectX::XMLoadFloat4x4(&m_mView);
				auto mProj = DirectX::XMLoadFloat4x4(&m_mProj);
				DirectX::XMStoreFloat4x4(
					&lm,
					DirectX::XMMatrixMultiply(mView, mProj)
				);
				return lm;
			}

		private:

			void Terminate();

			bool CreateDXGIFactory(IDXGIFactory6** dxgiFactory);

			void EnableDebugLayer();

			bool CreateDevice(IDXGIFactory6* dxgiFactory);

			void FindAdapter(IDXGIFactory6* dxgiFactory, IDXGIAdapter** adapterToUse);

			bool CreateCommandAllocator();

			bool CreateCommandList(D3D12_COMMAND_LIST_TYPE commandListType);

			bool CreateCommandQueue(D3D12_COMMAND_LIST_TYPE commandListType);

			bool CreateSwapChain(IDXGIFactory6* dxgiFactory);

			bool CreateDescriptorHeapForRTV();

			bool CreateRTVForFrameBuffer();

			bool CreateDescriptorHeapForDSV();

			bool CreateDSVForFrameBuffer();

			bool CreateFence();

			bool CreateSeceneConstantBuff();

		private:
			static CGraphicsEngine* m_instance;
			ID3D12Device5* m_device = nullptr;
			ID3D12CommandAllocator* m_commandAllocator = nullptr;
			ID3D12GraphicsCommandList* m_commandList = nullptr;
			ID3D12CommandQueue* m_commandQueue = nullptr;
			IDXGISwapChain4* m_swapChain = nullptr;
			ID3D12DescriptorHeap* m_rtvHeap = nullptr;
			ID3D12Resource* m_renderTargets[m_kFrameBufferCount] = { nullptr };
			ID3D12DescriptorHeap* m_dsvHeap = nullptr;
			ID3D12Resource* m_depthStencilBuffer = nullptr;
			ID3D12Fence* m_fence = nullptr;
			UINT64 m_fenceVal = 0;
			D3D12_VIEWPORT m_viewport;
			D3D12_RECT m_scissorRect;

			CTexture* m_whiteTexture;
			CTexture* m_blackTexture;


			DirectX::XMFLOAT3 m_eyePos;
			DirectX::XMFLOAT3 m_targetPos;
			DirectX::XMFLOAT3 m_upVec;
			DirectX::XMFLOAT4X4 m_mView;
			DirectX::XMFLOAT4X4 m_mProj;

			ID3D12Resource* m_sceneDataConstantBuff = nullptr;
			ID3D12DescriptorHeap* m_sceneDataDescriptorHeap = nullptr;
			SSceneDataMatrix* m_mappedSceneDataMatrix = nullptr;

			nsPMDModels::CPMDGenericRenderer* m_pmdGenericRenderer = nullptr;

		};
	}
}



