#pragma once
namespace nsYMEngine
{
	namespace nsAI
	{
		namespace nsNavigations
		{
			class CNavMesh;
		}
	}
}

namespace nsYMEngine
{
	namespace nsAI
	{
		namespace nsNavigations
		{
			class CPath : private nsUtils::SNoncopyable
			{
			private:

				/**
				 * @brief �p�X�̃Z�N�V����
				*/
				struct SSection
				{
					nsMath::CVector3 startPos = nsMath::CVector3::Zero();	// �Z�N�V�����̊J�n���W
					nsMath::CVector3 endPos = nsMath::CVector3::Front();	// �Z�N�V�����̏I�����W
					nsMath::CVector3 direction = nsMath::CVector3::Front();	// �Z�N�V�����̕���
					float length = 1.0f;									// �Z�N�V�����̒���
				};

			public:
				constexpr CPath() = default;
				~CPath() = default;

				/**
				 * @brief �p�X����ړ�����
				 * @param[in] targetPos �ړ���̖ڕW���W
				 * @param[in] moveSpeed �ړ����x
				 * @param[in] deltaTime �f���^�^�C��
				 * @param[out] isEnd �p�X���I��������true���������܂��
				 * @param[in] enablePhysics ���������L���ɂ��邩�H
				 * @return �ړ���̍��W
				*/
				nsMath::CVector3 Move(
					const nsMath::CVector3& targetPos,
					float moveSpeed,
					float deltaTime,
					bool& isEnd,
					bool enablePhysics = false
				);

				/**
				 * @brief �N���A
				*/
				inline void Clear() noexcept
				{
					m_sectionNo = 0;
					m_pointArray.clear();
					m_sectionArray.clear();
				}

				/**
				 * @brief �|�C���g�̃��U�[�u
				 * @param[in] capacity �L���p�V�e�B
				*/
				_CONSTEXPR20_CONTAINER void ReservePoint(unsigned int capacity) noexcept
				{
					m_pointArray.reserve(capacity);
				}

				/**
				 * @brief �|�C���g�̒ǉ�
				 * @param[in] point �ǉ�����|�C���g
				*/
				void AddPoint(const nsMath::CVector3& point) noexcept
				{
					m_pointArray.push_back(point);
				}

				/**
				 * @brief �p�X���\�z
				*/
				void Build() noexcept;

			private:
				std::vector<nsMath::CVector3> m_pointArray = {};	// �|�C���g�̔z��
				std::vector<SSection> m_sectionArray = {};			// �Z�N�V�����̔z��
				int m_sectionNo = 0;								// �Z�N�V�����ԍ�
			};

		}
	}
}