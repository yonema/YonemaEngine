#include "Sprite.h"
#include "../GraphicsEngine.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace ns2D
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


			void CSprite::Draw(nsDx12Wrappers::CCommandList* commandList)
			{
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
				commandList->SetDescriptorHeap(m_cbvDescriptorHeap);
				commandList->SetGraphicsRootDescriptorTable(0, m_cbvDescriptorHeap);
				commandList->SetDescriptorHeap(m_srvDescriptorHeap);
				commandList->SetGraphicsRootDescriptorTable(1, m_srvDescriptorHeap);
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
				m_srvDescriptorHeap.Release();
				m_cbvDescriptorHeap.Release();
				m_texture.Release();
				return;
			}


			bool CSprite::Init(const SSpriteInitData& initData)
			{
				m_spriteSize = initData.spriteSize;

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
				auto r1 = m_srvDescriptorHeap.InitAsCbvSrvUav(1, L"SpriteSRV");
				auto r2 = m_cbvDescriptorHeap.InitAsCbvSrvUav(1, L"SpriteCBV");
				return r1 && r2;
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
				m_constantBuffer.CreateConstantBufferView(m_cbvDescriptorHeap.GetCPUHandle());

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

				device->CreateShaderResourceView(m_texture.GetResource(), &srvDesc,
					m_srvDescriptorHeap.GetCPUHandle());


				return;
			}

			void CSprite::UpdateWorldMatrix(
				const nsMath::CVector2 position,
				const nsMath::CQuaternion& rotation,
				const nsMath::CVector3& scale,
				const nsMath::CVector2& pivot
			)
			{
				//�s�{�b�g�͐^�񒆂�0.0, 0.0�A���オ-1.0f, -1.0�A�E����1.0�A1.0�ɂȂ�悤�ɂ���B
				nsMath::CVector2 localPivot = pivot;
				localPivot -= {0.5f, 0.5f};
				localPivot *= -2.0f;

				nsMath::CVector2 halfSize = m_spriteSize;
				halfSize *= 0.5f;

				nsMath::CMatrix mPivotTrans;
				mPivotTrans.MakeTranslation(
					{ halfSize.x * localPivot.x, halfSize.y * localPivot.y, 0.0f });

				nsMath::CMatrix mTrans, mRot, mScale;
				mTrans.MakeTranslation({ position.x, position.y, 0.0f });
				mRot.MakeRotationFromQuaternion(rotation);
				mScale.MakeScaling(scale);
				m_worldMatrix = mPivotTrans * mScale * mRot * mTrans;

				return;
			}


		}

	}
}