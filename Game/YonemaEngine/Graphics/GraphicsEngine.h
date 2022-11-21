#pragma once
#include "Camera.h"
#include "Sprites/Sprite.h"
#include "Renderers/RendererTable.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDx12Wrappers
		{
			class CTexture;
		}
		namespace nsFonts
		{
			class CFontEngine;
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
					m_instance = nullptr;
				}
			}
			constexpr static CGraphicsEngine* GetInstance() noexcept
			{
				return m_instance;
			}

			bool Init();

			void Update();

			/**
			 * @brief 描画処理。更新処理より後、描画処理より前に呼ぶこと。
			*/
			void ExecuteDraw();


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
				return &m_commandList;
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

			constexpr CCamera* GetMainCamera() noexcept
			{
				return &m_mainCamera;
			}

			constexpr auto GetDescriptorSizeOfCbvSrvUav() const noexcept
			{
				return m_descriptorSizeOfCbvSrvUav;
			}

			constexpr auto GetDescriptorSizeOfRtv() const noexcept
			{
				return m_descriptorSizeOfRtv;
			}

			constexpr auto* GetRendererTable() noexcept
			{
				return &m_rendererTable;
			}
			
			constexpr auto* GetFrameBuffer() noexcept
			{
				return &m_frameBuffer;
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

			bool CreateSeceneConstantBuff();

			void WaitForCommandExecutionToComplete();

			void BeginDraw();

			void DrawToMainRenderTarget();

			void DrawWithSimplePostEffect();

			void Draw2D();

			void EndDraw();

		private:
			static CGraphicsEngine* m_instance;

			ID3D12Device5* m_device = nullptr;
			ID3D12CommandAllocator* m_commandAllocator = nullptr;
			nsDx12Wrappers::CCommandList m_commandList;
			ID3D12CommandQueue* m_commandQueue = nullptr;

			DirectX::GraphicsMemory* m_gfxMemForDirectXTK = nullptr;

			ID3D12Fence* m_fence = nullptr;
			short int m_fenceVal = 0;

			nsDx12Wrappers::CFrameBuffer m_frameBuffer;
			nsDx12Wrappers::CRenderTarget m_mainRenderTarget;
			nsSprites::CSprite m_mainRenderTargetSprite;
			nsDx12Wrappers::CRenderTarget m_simplePostEffectRenderTarget;
			nsSprites::CSprite m_simplePostEffectRenderTargetSprite;
			nsSprites::CSprite* m_pBaseRenderTargetSprite = nullptr;
			nsDx12Wrappers::CConstantBuffer m_sceneDataCB;
			nsDx12Wrappers::CDescriptorHeap m_sceneDataDH;
			nsRenderers::CRendererTable m_rendererTable;
			CCamera m_mainCamera;
			nsDx12Wrappers::CTexture* m_whiteTexture = nullptr;
			nsDx12Wrappers::CTexture* m_blackTexture = nullptr;
			unsigned int m_descriptorSizeOfCbvSrvUav = 0;
			unsigned int m_descriptorSizeOfRtv = 0;

			nsFonts::CFontEngine* m_fontEngine = nullptr;

		};
	}
}



