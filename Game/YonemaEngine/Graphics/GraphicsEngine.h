#pragma once
#include "Camera.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDx12Wrappers
		{
			class CTexture;
		}
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
		class CGraphicsEngine : private nsUtils::SNoncopyable
		{
		private:
			enum EnGPUAdapterType
			{
				// 上から使用優先度順
				enNVidia,
				enAMD,
				enIntel,
				NumGPUAdapterType
			};
			static const wchar_t* m_kGPUAdapterNames[EnGPUAdapterType::NumGPUAdapterType];
			static const unsigned int m_kNumFeatureLevel = 4;
			static const D3D_FEATURE_LEVEL m_kFeatureLevels[m_kNumFeatureLevel];

			struct SSceneDataMatrix
			{
				nsMath::CMatrix mView;
				nsMath::CMatrix mProj;
				nsMath::CVector3 cameraPosWS;
			};

			struct SPeraVertex
			{
				nsMath::CVector3 pos;
				nsMath::CVector2 uv;
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
			constexpr static CGraphicsEngine* GetInstance() noexcept
			{
				return m_instance;
			}

			bool Init();

			void Update();

			/**
			 * @brief 描画開始処理。更新処理より後、描画処理より前に呼ぶこと。
			 * @return 
			*/
			void BeginDraw();

			void EndDraw();

			constexpr auto GetDevice() noexcept
			{
				return m_device;
			}

			constexpr auto GetCommandAllocator() noexcept
			{
				return m_commandAllocator;
			}

			constexpr auto GetCommandList() noexcept
			{
				return m_commandList;
			}

			constexpr auto GetCommandQueue() noexcept
			{
				return m_commandQueue;
			}

			constexpr auto GetFence() noexcept
			{
				return m_fence;
			}

			constexpr auto GetFenceVal() noexcept
			{
				return &m_fenceVal;
			}

			constexpr nsDx12Wrappers::CTexture* GetWhiteTexture() noexcept
			{
				return m_whiteTexture;
			}

			constexpr nsDx12Wrappers::CTexture* GetBlackTexture() noexcept
			{
				return m_blackTexture;
			}

			constexpr nsMath::CMatrix GetMatrixViewProj() const noexcept
			{
				return m_mainCamera.GetViewProjectionMatirx();
			}

			constexpr auto GetDescriptorSizeOfCbvSrvUav() const noexcept
			{
				return m_descriptorSizeOfCbvSrvUav;
			}

			constexpr auto GetDescriptorSizeOfRtv() const noexcept
			{
				return m_descriptorSizeOfRtv;
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

			bool CreateFence();

			bool CreatePeraRenderTarget();

			bool CreateRTVDescriptorHeapForPeraRenderTarget();

			bool CreateRTVForPeraRenderTarget();

			bool CreateSRVDescriptorHeapForPeraRenderTarget();

			bool CreateSRVForPeraRenderTarget();

			bool CreateVertexBufferForPeraRenderTarget();

			bool CreateRootSignatureForPeraRenderTaraget();

			bool CreatePipelineStateForPeraRenderTarget();

			bool CreateSeceneConstantBuff();

		private:
			static CGraphicsEngine* m_instance;
			ID3D12Device5* m_device = nullptr;
			ID3D12CommandAllocator* m_commandAllocator = nullptr;
			ID3D12GraphicsCommandList* m_commandList = nullptr;
			ID3D12CommandQueue* m_commandQueue = nullptr;
			//IDXGISwapChain4* m_swapChain = nullptr;
			//ID3D12DescriptorHeap* m_rtvDescHeapForFrameBuff = nullptr;
			//ID3D12Resource* m_frameBuffers[m_kFrameBufferCount] = { nullptr };
			//ID3D12DescriptorHeap* m_dsvDescHeapForFrameBuff = nullptr;
			//ID3D12Resource* m_depthStencilBuffer = nullptr;
			ID3D12Fence* m_fence = nullptr;
			short int m_fenceVal = 0;
			//D3D12_VIEWPORT m_viewport;
			//D3D12_RECT m_scissorRect;

			unsigned int m_descriptorSizeOfCbvSrvUav = 0;
			unsigned int m_descriptorSizeOfRtv = 0;
			nsDx12Wrappers::CFrameBuffer m_frameBuffer;
			nsDx12Wrappers::CTexture* m_whiteTexture;
			nsDx12Wrappers::CTexture* m_blackTexture;
			nsDx12Wrappers::CConstantBuffer m_sceneDataCB;
			nsDx12Wrappers::CDescriptorHeap m_sceneDataDH;



			CCamera m_mainCamera;
			nsPMDModels::CPMDGenericRenderer* m_pmdGenericRenderer = nullptr;

			ID3D12Resource* m_peraRenderTarget = nullptr;
			ID3D12DescriptorHeap* m_rtvDescHeapForPeraRT = nullptr;
			ID3D12DescriptorHeap* m_srvDescHeapForPeraRT = nullptr;
			ID3D12Resource* m_vertexBuffForPeraRT = nullptr;
			D3D12_VERTEX_BUFFER_VIEW m_vertexBuffViewForPeraRT = {};
			ID3D12RootSignature* m_rootSignatureForPeraRT = nullptr;
			ID3D12PipelineState* m_pipelineStateForPeraRT = nullptr;

		};
	}
}



