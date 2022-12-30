#pragma once
namespace nsYMEngine
{
	namespace nsAI
	{
		namespace nsNavigations
		{
			class CPath;
			class CNavCell;
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
			class CPathFinding : private nsUtils::SNoncopyable
			{
			private:
				struct SCellWork
				{
					constexpr SCellWork() = default;
					~SCellWork() = default;

					void Init(const CNavCell* cell);

					const CNavCell* cell = nullptr;
					SCellWork* parentCell = nullptr;	// �e�̃Z���B
					float costFromStartCell = 0.0f;
					nsMath::CVector3 pathPoint= nsMath::CVector3::Zero();
					float cost = FLT_MAX;			// �ړ��R�X�g
					bool isOpend = false;			// �J���ꂽ�H
					bool isClosed = false;			// ����ꂽ�H
					bool isSmooth = false;			// �X���[�X�����H
				};

			public:
				constexpr CPathFinding() = default;
				~CPathFinding() = default;

				/**
				 * @brief �p�X�̌������������s
				 * @param[out] path ���������p�X�̊i�[��
				 * @param[in] naviMesh �i�r�Q�[�V�������b�V��
				 * @param[in] startPos �����J�n���W
				 * @param[in] endPos �����I�����W
				 * @param[in] enablePhysics ���������L���ɂ��邩�H
				 * @param[in] agentRadius AI�G�[�W�F���g�̔��a
				 * @param[in] agentHeight AI�G�[�W�F���g�̍���
				*/
				void Execute(
					CPath& path,
					const CNavMesh& naviMesh,
					const nsMath::CVector3& startPos,
					const nsMath::CVector3& endPos,
					bool enablePhysics = false,
					float agentRadius = 50.0f,
					float agentHeight = 200.0f
				);

			private:

				/**
				 * @brief ���̃Z���Ɉړ�����R�X�g���v�Z
				 * @param[out] totalCost �S�̂̃R�X�g�̊i�[��
				 * @param[out] costFromStartCell �J�n�Z������̃R�X�g�̊i�[��
				 * @param[in] nextCell ���̃Z��
				 * @param[in] prevCell 1�O�̃Z��
				 * @param[in] endCell �ŏI�Z��
				*/
				void CalcCost(
					float& totalCost,
					float& costFromStartCell,
					const SCellWork* nextCell,
					const SCellWork* prevCell,
					const CNavCell* endCell
				) const noexcept;

				/**
				 * @brief �X���[�W���O
				 * @param[in,out] cellList �Z�����X�g
				 * @param[in] enablePhysics ���������L���ɂ��邩�H
				 * @param[in] agentRadius AI�G�[�W�F���g�̔��a
				 * @param[in] agentHeight AI�G�[�W�F���g�̍���
				*/
				void Smoothing(
					std::list<SCellWork*>& cellList,
					bool enablePhysics,
					float agentRadius,
					float agentHeight
				) const noexcept;

				bool IsIntersectRayToCell(
					nsMath::CVector3 rayStartPos,
					nsMath::CVector3 rayEndPos,
					SCellWork* currentCellWork
				) const noexcept;

			private:
				std::vector<SCellWork> m_cellWorkArray = {};

			};

		}
	}
}