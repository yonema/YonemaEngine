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

				SPlane plane = {};				// 分割平面。
				SEntityPtr rightEntity = {};	// 右の要素
				SEntityPtr leftEntity = {};		// 左の要素。
				nsMath::CVector3 centerPos = nsMath::CVector3::Zero();	// 中心座標。
				std::vector<SEntityPtr> leafArray = {};	// ノードが内包しているリーフ。
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
				// リーフ生成
				auto newEntity = std::make_shared<SLeaf>(position, extraData, EnEntityType::enLeaf);
				m_leafArray.emplace_back(newEntity);
			}

			/**
			 * @brief BSPツリーを構築する
			*/
			void Build() noexcept;

			/**
			 * @brief BSPツリーを探索する
			 * @param[in] pos 座標
			 * @param[in,out] onEndWalk 探索が終了したときに呼ばれるコールバック関数
			*/
			void WalkTree(
				const nsMath::CVector3& pos, std::function<void(SLeaf* leaf)> onEndWalk) const noexcept;


		private:

			/**
			 * @brief 新しいBSPツリーの要素を作成する
			 * @param[in] leafArray リーフの配列
			 * @return リーフのエンティティ
			*/
			SEntityPtr CreateBSPTreeEntity(const std::vector<SEntityPtr>& leafArray) const noexcept;

			/**
			 * @brief BSPツリーのLeafList要素を作成する
			 * @param[in] leafArray リーフの配列
			 * @return リーフのエンティティ
			*/
			SEntityPtr CreateBSPTreeEntity_LeafList(
				const std::vector<SEntityPtr>& leafArray) const noexcept;

			/**
			 * @brief リーフのリストから中心座標を計算する
			 * @param leafArray リーフの配列
			 * @return 中心座標
			*/
			nsMath::CVector3 CalcCenterPositionFromLeafList(
				const std::vector<SEntityPtr>& leafArray) const noexcept;

			/**
			 * @brief リーフノードの配列から共分散行列を計算する
			 * @param[out] covarianceMatrix 共分散行列の計算先
			 * @param[in] leafArray リーフノードの配列
			 * @param[in] centerPos リーフノードの中心座標
			*/
			void CalcCovarianceMatrixFromLeafNodeList(
				float covarianceMatrix[3][3],
				const std::vector<SEntityPtr>& leafArray,
				const nsMath::CVector3& centerPos
			) const noexcept;

			/**
			 * @brief 共分散行列から分割平面を計算する
			 * @param[out] plane 分割平面の書き込み先
			 * @param[in,out] covarianceMatrix 共分散行列の書き込み先
			 * @param[in] leafArray リーフノードの配列
			 * @param[in] centerPos リーフノードの中心座標
			*/
			void CalcSplitPlaneFromCovarianceMatrix(
				SPlane& plane,
				float covarianceMatrix[3][3],
				const std::vector<SEntityPtr>& leafArray,
				const nsMath::CVector3& centerPos
			) const noexcept;

			/**
			 * @brief 平面でリーフノードを分割していく
			 * @param leftLeafArray 左のリーフ配列の書き込み先
			 * @param rightLeafArray 右のリーフ配列の書き込み先
			 * @param plane 分割平面
			 * @param leafArray 分割するリーフ配列
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
			SEntityPtr m_rootNode = nullptr;			// ルートノード。
			std::vector<SEntityPtr> m_leafArray = {};	// リーフの配列。
		};

	}
}