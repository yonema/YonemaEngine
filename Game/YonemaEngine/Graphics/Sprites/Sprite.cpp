#include "Sprite.h"
#include "../GraphicsEngine.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsSprites
		{
			namespace
			{
				struct SSimpleVertex
				{
					nsMath::CVector3 pos;
					nsMath::CVector2 uv;
				};
				struct SPeraVertex
				{
					nsMath::CVector3 pos;
					nsMath::CVector2 uv;
				};
			}

			const nsMath::CMatrix CSprite::m_kViewMatrix = nsMath::CMatrix::ViewMatrix(
				nsMath::CVector3::Back(), nsMath::CVector3::Zero(), nsMath::CVector3::Up());
			nsMath::CVector2 CSprite::m_frameBufferHalfSize = { -1.0f,-1.0f };


			void CSprite::Draw(nsDx12Wrappers::CCommandList* commandList)
			{
				if (IsDrawingFlag() != true)
				{
					return;
				}

				static auto mainCamera = CGraphicsEngine::GetInstance()->GetMainCamera();
				const auto& viewport = commandList->GetCurrentViewport();

				nsMath::CMatrix projMatrix;
				projMatrix.MakeOrthoProjectionMatrix(viewport.Width, viewport.Height, 0.1f, 1.0f);

				m_constantBufferCPU.mWorldViewProj = m_worldMatrix * m_kViewMatrix * projMatrix;
				m_constantBufferCPU.screenParam.x = mainCamera->GetNearClip();
				m_constantBufferCPU.screenParam.y = mainCamera->GetFarClip();
				m_constantBufferCPU.screenParam.z = viewport.Width;
				m_constantBufferCPU.screenParam.w = viewport.Height;

				m_constantBuffer.CopyToMappedConstantBuffer(&m_constantBufferCPU, sizeof(m_constantBufferCPU));

				commandList->SetVertexBuffer(m_vertexBuffer);

				commandList->SetDescriptorHeap(m_descriptorHeap);
				auto handle = m_descHandle.GetGpuHandle(
					static_cast<unsigned int>(EnDescHeapLayout::enSpriteCBV));
				commandList->SetGraphicsRootDescriptorTable(0, handle);
				handle = m_descHandle.GetGpuHandle(
					static_cast<unsigned int>(EnDescHeapLayout::enImageSRV));
				commandList->SetGraphicsRootDescriptorTable(1, handle);

				if (m_pExpandConstantBuffer)
				{
					// �g���萔�o�b�t�@������΍X�V
					handle = m_descHandle.GetGpuHandle(
						static_cast<unsigned int>(EnDescHeapLayout::enNum));
					commandList->SetGraphicsRootDescriptorTable(2, handle);
				}

				// �C���f�b�N�X�Ȃ��B4���_�B
				commandList->Draw(4);

				return;
			}

			CSprite::~CSprite()
			{
				Terminate();
				return;
			}

			void CSprite::Terminate()
			{
				Release();
				return;
			}

			void CSprite::Release()
			{
				m_constantBuffer.Release();
				m_vertexBuffer.Release();
				m_descriptorHeap.Release();
				m_texture.Release();
				return;
			}


			bool CSprite::Init(const SSpriteInitData& initData)
			{
				m_spriteSize = initData.spriteSize;
				m_pExpandConstantBuffer = initData.pExpandConstantBuffer;

				if (InitTexture(initData) != true)
				{
					return false;
				}
				if (CreateDescriptorHeap() != true)
				{
					return false;
				}
				if (CreateVertexBuffer() != true)
				{
					return false;
				}
				if (CreateConstantBuffer() != true)
				{
					return false;
				}
				CreateSrvCbv();

				m_constantBufferCPU.mulColor = nsMath::CVector4::White();

				// ���������Ȃ珉��������
				if (m_frameBufferHalfSize.x <= 0.0f)
				{
					const auto& viewport = CGraphicsEngine::GetInstance()->GetFrameBuffer()->GetViewport();

					float width = viewport.Width - viewport.TopLeftX;
					float height = viewport.Height - viewport.TopLeftY;
					m_frameBufferHalfSize = { width, height };
					m_frameBufferHalfSize.Scale(0.5f);
				}

				SetDrawingFlag(true);

				return true;
			}

			bool CSprite::InitTexture(const SSpriteInitData& initData)
			{
				if (initData.filePath != nullptr)
				{
					m_texture.Init(initData.filePath);
				}
				else if (initData.texture != nullptr)
				{
					m_texture.InitFromTexture(initData.texture);
				}
				else
				{
					nsGameWindow::MessageBoxError(L"�������f�[�^�Ƀe�N�X�`���̏���ݒ肵�Ă��������B");
					return false;
				}

				return true;
			}

			bool CSprite::CreateDescriptorHeap()
			{
				const unsigned int kNumDescHeapsOfSprite = 
					static_cast<unsigned int>(EnDescHeapLayout::enNum);
				const unsigned int kNumDescHeaps = 
					kNumDescHeapsOfSprite + (m_pExpandConstantBuffer ? 1 : 0);

				auto res = m_descriptorHeap.InitAsCbvSrvUav(kNumDescHeaps, L"SpriteDH");

				m_descHandle.Init(
					kNumDescHeaps,
					m_descriptorHeap.GetCPUHandle(), 
					m_descriptorHeap.GetGPUHandle()
				);

				return res;
			}

			bool CSprite::CreateVertexBuffer()
			{
				nsMath::CVector2 halfSize = m_spriteSize;
				halfSize.Scale(0.5f);

				// �C���f�b�N�X�o�b�t�@���w�肵�Ȃ�����A���ԂɋC��t����B
				const SSimpleVertex vertices[] =
				{
					{ { -halfSize.x, -halfSize.y, 0.0f },{ 0.0f, 1.0f }},	// ����
					{ { -halfSize.x,  halfSize.y, 0.0f },{ 0.0f, 0.0f }},	// ����
					{ {  halfSize.x, -halfSize.y, 0.0f },{ 1.0f, 1.0f }},	// �E��
					{ {  halfSize.x,  halfSize.y, 0.0f },{ 1.0f, 0.0f }}	// �E��
				};

				return m_vertexBuffer.Init(sizeof(vertices), sizeof(vertices[0]), vertices);
			}

			bool CSprite::CreateConstantBuffer()
			{
				return m_constantBuffer.Init(
					sizeof(SConstantBufferCPU), nullptr, 1, &m_constantBufferCPU);
				return true;
			}

			void CSprite::CreateSrvCbv()
			{

				auto device = CGraphicsEngine::GetInstance()->GetDevice();

				// �Z�萔�o�b�t�@�r���[�̍쐬
				auto handle = m_descHandle.GetCpuHandle(
					static_cast<unsigned int>(EnDescHeapLayout::enSpriteCBV));
				m_constantBuffer.CreateConstantBufferView(handle);

				// �Z�V�F�[�_�[���\�[�X�r���[�̍쐬

				D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
				if (m_texture.IsCubemap())
				{
					srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
				}
				else
				{
					srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				}
				srvDesc.Format = m_texture.GetFormat();
				srvDesc.Texture2D.MipLevels = m_texture.GetMipLevels();
				srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

				handle = m_descHandle.GetCpuHandle(
					static_cast<unsigned int>(EnDescHeapLayout::enImageSRV));
				device->CreateShaderResourceView(m_texture.GetResource(), &srvDesc, handle);


				if (m_pExpandConstantBuffer != nullptr)
				{
					handle = m_descHandle.GetCpuHandle(
						static_cast<unsigned int>(EnDescHeapLayout::enNum));
					m_pExpandConstantBuffer->CreateConstantBufferView(handle);
				}


				return;
			}

			void CSprite::UpdateWorldMatrix(
				const nsMath::CVector2 position,
				const nsMath::CQuaternion& rotation,
				const nsMath::CVector3& scale,
				const nsMath::CVector2& pivot,
				EnAnchors anchor
			)
			{
				// position�����̂܂܍��W�ɂ���ƁA
				// �����ɕ\���B
				// ���W�n���AYUp�AXRight�ɂȂ�B
				// ���W�n�͑S�̓I�ɁAYDown�AXRight�ɂȂ�悤�ɂ���B

				nsMath::CVector2 finalPos;
				// �܂���Y���W�̃A���J�[�ɍ��킹��B
				switch (anchor)
				{
				case EnAnchors::enTopLeft:
				case EnAnchors::enTopCenter:
				case EnAnchors::enTopRight:
					finalPos.y = position.y - m_frameBufferHalfSize.y;
					break;
				case EnAnchors::enMiddleLeft:
				case EnAnchors::enMiddleCenter:
				case EnAnchors::enMiddleRight:
				default:
					finalPos.y = position.y;
					break;
				case EnAnchors::enBottonLeft:
				case EnAnchors::enBottonCenter:
				case EnAnchors::enBottonRight:
					finalPos.y = position.y + m_frameBufferHalfSize.y;
					break;
				}
				//����X���W�̃A���J�[�ɍ��킹��B
				switch (anchor)
				{
				case EnAnchors::enTopLeft:
				case EnAnchors::enMiddleLeft:
				case EnAnchors::enBottonLeft:
					finalPos.x = position.x - m_frameBufferHalfSize.x;
					break;
				case EnAnchors::enTopCenter:
				case EnAnchors::enMiddleCenter:
				case EnAnchors::enBottonCenter:
				default:
					finalPos.x = position.x;
					break;
				case EnAnchors::enTopRight:
				case EnAnchors::enMiddleRight:
				case EnAnchors::enBottonRight:
					finalPos.x = position.x + m_frameBufferHalfSize.x;
					break;
				}
				// �Ō��Y���W���t���b�v����B
				finalPos.y *= -1.0f;


				// �s�{�b�g�́A
				// { 0.0f, 0.0f }������A
				// { 1.0f, 1.0f }���E���A
				// { 0.5f, 0.5f }�������ɂȂ�悤�ɂ���B
				nsMath::CVector2 localPivot = pivot;
				localPivot -= {0.5f, 0.5f};
				localPivot *= -2.0f;

				nsMath::CVector2 halfSize = m_spriteSize;
				halfSize *= 0.5f;


				nsMath::CMatrix mTrans, mRot, mScale, mPivot;
				mTrans.MakeTranslation({ finalPos.x, finalPos.y, 0.0f });
				mRot.MakeRotationFromQuaternion(rotation);
				mScale.MakeScaling(scale);
				mPivot.MakeTranslation(
					{ halfSize.x * localPivot.x, halfSize.y * -localPivot.y, 0.0f });
				m_worldMatrix = mPivot * mScale * mRot * mTrans;

				return;
			}


		}

	}
}