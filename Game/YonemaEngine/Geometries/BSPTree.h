#pragma once
namespace nsYMEngine
{
	namespace nsGeometries
	{
		class CBSPTree : private nsUtils::SNoncopyable
		{
		private:
			struct SPlane : private nsUtils::SNoncopyable
			{
				nsMath::CVector3 normal = nsMath::CVector3::Up();
				float distance = 0.0f;
			};

		public:
			enum class EnEntityType
			{
				enNode,
				enLeaf,
				enLeafList
			};

			struct SEntity : private nsUtils::SNoncopyable
			{
				constexpr SEntity() = default;
				constexpr SEntity(EnEntityType type)
					:type(type) {};
				~SEntity() = default;

				EnEntityType type = EnEntityType::enNode;
			};

			using SEntityPtr = std::shared_ptr<SEntity>;

			struct SNode : public SEntity
			{
				constexpr SNode() = default;
				~SNode() = default;

				SPlane plane = {};				// �������ʁB
				SEntityPtr rightEntity = {};	// �E�̗v�f
				SEntityPtr leftEntity = {};		// ���̗v�f�B
				nsMath::CVector3 centerPos = nsMath::CVector3::Zero();	// ���S���W�B
				std::vector<SEntityPtr> leafArray = {};	// �m�[�h������Ă��郊�[�t�B
			};

			struct SLeaf : public SEntity
			{
				constexpr SLeaf() = default;
				constexpr SLeaf(const nsMath::CVector3& pos, void* exData, EnEntityType type)
					:position(pos), extraData(exData), SEntity(type){};
				~SLeaf() = default;

				nsMath::CVector3 position = nsMath::CVector3::Zero();
				void* extraData = nullptr;
			};

			struct SLeafList : public SEntity
			{
				constexpr SLeafList() = default;
				~SLeafList() = default;

				std::vector<SEntityPtr> leafList = {};
			};

		public:
			constexpr CBSPTree() = default;
			~CBSPTree() = default;

			_CONSTEXPR20_CONTAINER void ReserveLeaf(unsigned int capacity) noexcept
			{
				m_leafArray.reserve(capacity);
			}

			inline void AddLeaf(const nsMath::CVector3& position, void* extraData) noexcept
			{
				// ���[�t����
				auto newEntity = std::make_shared<SLeaf>(position, extraData, EnEntityType::enLeaf);
				m_leafArray.emplace_back(newEntity);
			}

			/**
			 * @brief BSP�c���[���\�z����
			*/
			void Build() noexcept;

			/**
			 * @brief BSP�c���[��T������
			 * @param[in] pos ���W
			 * @param[in,out] onEndWalk �T�����I�������Ƃ��ɌĂ΂��R�[���o�b�N�֐�
			*/
			void WalkTree(
				const nsMath::CVector3& pos, std::function<void(SLeaf* leaf)> onEndWalk) const noexcept;


		private:

			/**
			 * @brief �V����BSP�c���[�̗v�f���쐬����
			 * @param[in] leafArray ���[�t�̔z��
			 * @return ���[�t�̃G���e�B�e�B
			*/
			SEntityPtr CreateBSPTreeEntity(const std::vector<SEntityPtr>& leafArray) const noexcept;

			/**
			 * @brief BSP�c���[��LeafList�v�f���쐬����
			 * @param[in] leafArray ���[�t�̔z��
			 * @return ���[�t�̃G���e�B�e�B
			*/
			SEntityPtr CreateBSPTreeEntity_LeafList(
				const std::vector<SEntityPtr>& leafArray) const noexcept;

			/**
			 * @brief ���[�t�̃��X�g���璆�S���W���v�Z����
			 * @param leafArray ���[�t�̔z��
			 * @return ���S���W
			*/
			nsMath::CVector3 CalcCenterPositionFromLeafList(
				const std::vector<SEntityPtr>& leafArray) const noexcept;

			/**
			 * @brief ���[�t�m�[�h�̔z�񂩂狤���U�s����v�Z����
			 * @param[out] covarianceMatrix �����U�s��̌v�Z��
			 * @param[in] leafArray ���[�t�m�[�h�̔z��
			 * @param[in] centerPos ���[�t�m�[�h�̒��S���W
			*/
			void CalcCovarianceMatrixFromLeafNodeList(
				float covarianceMatrix[3][3],
				const std::vector<SEntityPtr>& leafArray,
				const nsMath::CVector3& centerPos
			) const noexcept;

			/**
			 * @brief �����U�s�񂩂番�����ʂ��v�Z����
			 * @param[out] plane �������ʂ̏������ݐ�
			 * @param[in,out] covarianceMatrix �����U�s��̏������ݐ�
			 * @param[in] leafArray ���[�t�m�[�h�̔z��
			 * @param[in] centerPos ���[�t�m�[�h�̒��S���W
			*/
			void CalcSplitPlaneFromCovarianceMatrix(
				SPlane& plane,
				float covarianceMatrix[3][3],
				const std::vector<SEntityPtr>& leafArray,
				const nsMath::CVector3& centerPos
			) const noexcept;

			/**
			 * @brief ���ʂŃ��[�t�m�[�h�𕪊����Ă���
			 * @param leftLeafArray ���̃��[�t�z��̏������ݐ�
			 * @param rightLeafArray �E�̃��[�t�z��̏������ݐ�
			 * @param plane ��������
			 * @param leafArray �������郊�[�t�z��
			*/
			void SplitLeafArray(
				std::vector<SEntityPtr>& leftLeafArray,
				std::vector<SEntityPtr>& rightLeafArray,
				const SPlane& plane,
				const std::vector<SEntityPtr>& leafArray
			) const noexcept;

			void WalkTree(
				SEntityPtr entityPtr,
				const nsMath::CVector3& pos,
				std::function<void(SLeaf* leaf)> onEndWalk
			) const noexcept;


		private:
			SEntityPtr m_rootNode = nullptr;			// ���[�g�m�[�h�B
			std::vector<SEntityPtr> m_leafArray = {};	// ���[�t�̔z��B
		};

	}
}