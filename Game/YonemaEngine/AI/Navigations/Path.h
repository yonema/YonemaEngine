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
				 * @brief パスのセクション
				*/
				struct SSection
				{
					nsMath::CVector3 startPos = nsMath::CVector3::Zero();	// セクションの開始座標
					nsMath::CVector3 endPos = nsMath::CVector3::Front();	// セクションの終了座標
					nsMath::CVector3 direction = nsMath::CVector3::Front();	// セクションの方向
					float length = 1.0f;									// セクションの長さ
				};

			public:
				constexpr CPath() = default;
				~CPath() = default;

				/**
				 * @brief パス上を移動する
				 * @param[in] targetPos 移動先の目標座標
				 * @param[in] moveSpeed 移動速度
				 * @param[in] deltaTime デルタタイム
				 * @param[out] isEnd パスが終了したらtrueが書き込まれる
				 * @param[in] enablePhysics 物理判定を有効にするか？
				 * @return 移動後の座標
				*/
				nsMath::CVector3 Move(
					const nsMath::CVector3& targetPos,
					float moveSpeed,
					float deltaTime,
					bool& isEnd,
					bool enablePhysics = false
				);

				/**
				 * @brief クリア
				*/
				inline void Clear() noexcept
				{
					m_sectionNo = 0;
					m_pointArray.clear();
					m_sectionArray.clear();
				}

				/**
				 * @brief ポイントのリザーブ
				 * @param[in] capacity キャパシティ
				*/
				_CONSTEXPR20_CONTAINER void ReservePoint(unsigned int capacity) noexcept
				{
					m_pointArray.reserve(capacity);
				}

				/**
				 * @brief ポイントの追加
				 * @param[in] point 追加するポイント
				*/
				void AddPoint(const nsMath::CVector3& point) noexcept
				{
					m_pointArray.push_back(point);
				}

				/**
				 * @brief パスを構築
				*/
				void Build() noexcept;

			private:
				std::vector<nsMath::CVector3> m_pointArray = {};	// ポイントの配列
				std::vector<SSection> m_sectionArray = {};			// セクションの配列
				int m_sectionNo = 0;								// セクション番号
			};

		}
	}
}