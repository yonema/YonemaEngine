#include "GaussianBlur.h"
#include "../GraphicsEngine.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsImageProcessing
		{
			CGaussianBlur::~CGaussianBlur()
			{
				Terminate();

				return;
			}

			void CGaussianBlur::Terminate()
			{
				Release();

				return;
			}


			void CGaussianBlur::Release()
			{
				m_descHeap.Release();
				m_constantBuffer.Release();
				m_xBlurRenderTarget.Release();
				m_yBlurRenderTarget.Release();
				m_xBlurSprite.Release();
				m_yBlurSprite.Release();

				return;
			}


			void CGaussianBlur::Init(nsDx12Wrappers::CTexture* originalTexture, float blurPower)
			{
				m_originalTexture = originalTexture;

				UpdateWeightsTable(blurPower);

				InitRenderTargets();

				InitSprites();

				InitCBV();

				return;
			}

			void CGaussianBlur::ExecuteOnGPU(nsDx12Wrappers::CCommandList* commandList)
			{
				//// X�u���[�����s ////

				// �`����ݒ�
				commandList->TransitionFromShaderResourceToRenderTarget(m_xBlurRenderTarget);
				commandList->SetRenderTarget(m_xBlurRenderTarget);

				// �r���[�|�[�g�ƃV�U�����O��`��ݒ�
				commandList->SetViewportAndScissorRect(m_xBlurRenderTarget);

				// �C���f�b�N�X�o�b�t�@���w�肵�Ă��Ȃ����߁ATRIANGLELIST�ł͂Ȃ��A
				// TRIANGLESTRIP���w�肷��B
				commandList->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

				using EnRendererType = nsRenderers::CRendererTable::EnRendererType;
				// ���[�g�V�O�l�`���ƃp�C�v���C���X�e�[�g�̐ݒ�
				auto* rendererTable = CGraphicsEngine::GetInstance()->GetRendererTable();
				commandList->SetGraphicsRootSignatureAndPipelineState(
					rendererTable->GetRootSignature(EnRendererType::enGaussianBlurXForShadowMap),
					rendererTable->GetPipelineState(EnRendererType::enGaussianBlurXForShadowMap)
				);

				// �u���[�p�̒萔�o�b�t�@��ݒ�
				commandList->SetDescriptorHeap(m_descHeap);
				commandList->SetGraphicsRootDescriptorTable(2, m_descHeap);

				// X�u���[���s
				m_xBlurSprite.Draw(commandList);

				// X�u���[�I��
				commandList->TransitionFromRenderTargetToShaderResource(m_xBlurRenderTarget);


				//// Y�u���[���s ////

				// �`����ݒ�
				commandList->TransitionFromShaderResourceToRenderTarget(m_yBlurRenderTarget);
				commandList->SetRenderTarget(m_yBlurRenderTarget);

				// �r���[�|�[�g�ƃV�U�����O��`��ݒ�
				commandList->SetViewportAndScissorRect(m_yBlurRenderTarget);

				// �C���f�b�N�X�o�b�t�@���w�肵�Ă��Ȃ����߁ATRIANGLELIST�ł͂Ȃ��A
				// TRIANGLESTRIP���w�肷��B
				// �������w�肵��������Ȃ��Ă������͂��B
				//commandList->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

				// ���[�g�V�O�l�`���ƃp�C�v���C���X�e�[�g�̐ݒ�
				rendererTable = CGraphicsEngine::GetInstance()->GetRendererTable();
				commandList->SetGraphicsRootSignatureAndPipelineState(
					rendererTable->GetRootSignature(EnRendererType::enGaussianBlurYForShadowMap),
					rendererTable->GetPipelineState(EnRendererType::enGaussianBlurYForShadowMap)
				);

				// �u���[�p�̒萔�o�b�t�@��ݒ�
				commandList->SetDescriptorHeap(m_descHeap);
				commandList->SetGraphicsRootDescriptorTable(2, m_descHeap);

				// Y�u���[���s
				m_yBlurSprite.Draw(commandList);


				// Y�u���[�I��
				commandList->TransitionFromRenderTargetToShaderResource(m_yBlurRenderTarget);



				return;
			}

			void CGaussianBlur::InitRenderTargets()
			{
				const auto& texSize = m_originalTexture->GetTextureSize();
				//X�u���[�p�̃����_�����O�^�[�Q�b�g���쐬����B
				m_xBlurRenderTarget.Init(
					static_cast<unsigned int>(texSize.x / 2.0f),	// x�������Ƀ_�E���T���v�����O
					static_cast<unsigned int>(texSize.y),
					m_originalTexture->GetFormat(),
					nsMath::CVector4::Black(),
					DXGI_FORMAT_UNKNOWN,
					L"XBlur"
				);

				//Y�u���[�p�̃����_�����O�^�[�Q�b�g���쐬����B
				m_yBlurRenderTarget.Init(
					static_cast<unsigned int>(texSize.x / 2.0f),	// x��������
					static_cast<unsigned int>(texSize.y / 2.0f),	// y�����������Ƀ_�E���T���v�����O
					m_originalTexture->GetFormat(),
					nsMath::CVector4::Black(),
					DXGI_FORMAT_UNKNOWN,
					L"YBlur"
				);

				return;
			}

			void CGaussianBlur::InitSprites()
			{
				//X�u���[�p�̃X�v���C�g������������B
				{
					nsSprites::SSpriteInitData initData;
					//�X�v���C�g�̉𑜓x��m_xBlurRenderTarget�Ɠ����B
					initData.spriteSize.x = static_cast<float>(m_xBlurRenderTarget.GetWidth());
					initData.spriteSize.y = static_cast<float>(m_xBlurRenderTarget.GetHeight());
					initData.texture = m_originalTexture;
					//�`�����ރ����_�����O�^�[�Q�b�g�̃t�H�[�}�b�g���w�肷��B
					initData.colorFormat = m_xBlurRenderTarget.Get()->GetDesc().Format;

					m_xBlurSprite.Init(initData);
				}

				//Y�u���[�p�̃X�v���C�g������������B
				{
					nsSprites::SSpriteInitData initData;
					//�X�v���C�g�̉𑜓x��m_yBlurRenderTarget�Ɠ����B
					initData.spriteSize.x = static_cast<float>(m_yBlurRenderTarget.GetWidth());
					initData.spriteSize.y = static_cast<float>(m_yBlurRenderTarget.GetHeight());
					//�e�N�X�`���͉��u���[�����������́B
					initData.texture = m_xBlurRenderTarget.GetRenderTargetTexture();
					//�`�����ރ����_�����O�^�[�Q�b�g�̃t�H�[�}�b�g���w�肷��B
					initData.colorFormat = m_yBlurRenderTarget.Get()->GetDesc().Format;

					m_yBlurSprite.Init(initData);
				}

				return;
			}

			void CGaussianBlur::InitCBV()
			{
				m_constantBuffer.Init(static_cast<unsigned int>(sizeof(m_weights)), L"GaussianBlurCB");

				m_constantBuffer.CopyToMappedConstantBuffer(m_weights, sizeof(m_weights));

				m_descHeap.InitAsCbvSrvUav(1, L"GaussianBlueDH");

				m_constantBuffer.CreateConstantBufferView(m_descHeap.GetCPUHandle());

				return;
			}


			void CGaussianBlur::UpdateWeightsTable(float blurPower)
			{
				float total = 0;
				for (int i = 0; i < m_kNumWeights; i++)
				{
					m_weights[i] = expf(-0.5f * (float)(i * i) / blurPower);
					total += 2.0f * m_weights[i];

				}

				// �K�i��
				for (int i = 0; i < m_kNumWeights; i++)
				{
					m_weights[i] /= total;
				}

				return;
			}



		}
	}
}