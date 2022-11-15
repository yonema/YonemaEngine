#pragma once
namespace nsYMEngine
{
	namespace nsGameObject
	{
		/**
		 * @brief ゲームオブジェクトの基底クラス。
		 * 基本的にゲーム内のオブジェクトは、このクラスをpublicで継承してください。
		*/
		class IGameObject : private nsUtils::SNoncopyable
		{
		private:

			enum class EnGameObjectFlagTable
			{
				enStarted,
				enDead,
				enActive,
				enNumFlags
			};

			/**
			 * @brief 開始処理が有効になる時のフラグビットセットの組み合わせ
			*/
			static const std::bitset<static_cast<int>(EnGameObjectFlagTable::enNumFlags)>
				m_kEnableStartFlags;
			/**
			 * @brief 更新処理が有効になる時のフラグビットセットの組み合わせ
			*/
			static const std::bitset<static_cast<int>(EnGameObjectFlagTable::enNumFlags)>
				m_kEnableUpdateFlags;

		protected:
			/**
			 * @brief スタート処理。この処理は自身を生成した次のフレームの開始時に一度だけ呼ばれます。
			 * @return Update()処理を行うか？
			 * @retval true Update()処理を行う
			 * @retval false Update()処理を行わない
			*/
			virtual bool Start() { return true; }

			/**
			 * @brief このゲームオブジェクトが破棄されるときに呼ばれる処理。
			*/
			virtual void OnDestroy() {}

			/**
			 * @brief 更新処理。毎フレームこの処理が呼ばれます。
			 * @param[in] deltaTime 前回のフレームからの経過時間(秒)
			*/
			virtual void Update(float deltaTime) {}

		public:
			constexpr IGameObject() = default;
			virtual ~IGameObject() = default;

			/**
			 * @brief 開始処理が呼ばれたか調べる
			 * @return 開始処理が呼ばれたか？
			 * @retval true 開始処理が呼ばれた後
			 * @retval false 開始処理がまだ呼ばれていない
			*/
			constexpr bool IsStarted() const noexcept
			{
				return GetFlag(EnGameObjectFlagTable::enStarted);
			}

			/**
			 * @brief アクティブ化する。更新処理や開始処理が行われるようになります。
			*/
			inline void Activate() noexcept
			{
				SetFlag(EnGameObjectFlagTable::enActive, true);
			}

			/**
			 * @brief 非アクティブ化する。更新処理や開始処理が行われなくなります。
			*/
			inline void Deactivate() noexcept
			{
				SetFlag(EnGameObjectFlagTable::enActive, false);
			}

			/**
			 * @brief アクティブ状態か調べる
			 * @return アクティブ状態か？
			 * @retval true アクティブ状態
			 * @retval false 非アクティブ状態
			*/
			constexpr bool IsActive() const noexcept
			{
				return GetFlag(EnGameObjectFlagTable::enActive);
			}

			/**
			 * @brief 死亡フラグを立てる。
			 * 自身のオブジェクトを破棄するときは、この関数ではなく DeleteGO<>() を使用してください。
			*/
			inline void Dead() noexcept
			{
				SetFlag(EnGameObjectFlagTable::enDead, true);
			}

			/**
			 * @brief 死亡フラグが立っているか調べる
			 * @return 死亡フラグが立っているか？
			 * @retval true 死亡フラグが立っている
			 * @retval false 死亡フラグが立っていない
			*/
			constexpr bool IsDead() const noexcept
			{
				return GetFlag(EnGameObjectFlagTable::enDead);
			}

			/**
			 * @brief 名前を設定します。名前はゲームオブジェクトの検索などで使用します。
			 * @param[in] name 設定する名前
			*/
			constexpr void SetName(const char* name)
			{
				if (name != nullptr)
				{
					m_name = name;
				}
			}

			/**
			 * @brief 自身の名前と、引数で渡された名前が同じ名前か調べる。
			 * @param[in] objectName 自身の名前と比べる名前
			 * @return 同じ名前か？
			 * @retval true 同じ名前
			 * @retval false 違う名前
			*/
			bool IsMatchName(const char* objectName) const noexcept;

			/**
			 * @brief 開始処理のラップ関数。
			 * この関数はIGameObjectManagerからのみ実行されます。
			 * 他の場所からは実行しないでください。
			*/
			inline void StartWrapper() 
			{
				if (m_flagTable == m_kEnableStartFlags)
				{
					SetFlag(EnGameObjectFlagTable::enStarted, Start());
				}
			};

			/**
			 * @brief 更新処理のラップ関数。
			 * この関数はIGameObjectManagerからのみ実行されます。
			 * 他の場所からは実行しないでください。
			 * @param[in] deltaTime 前回のフレームからの経過時間(秒)
			*/
			inline void UpdateWrapper(float deltaTime) 
			{
				if (m_flagTable == m_kEnableUpdateFlags)
				{
					Update(deltaTime);
				}
			};

			/**
			 * @brief 破棄されるときに呼ばれる関数のラップ関数。
			 * この関数はIGameObjectManagerからのみ実行されます。
			 * 他の場所からは実行しないでください。
			*/
			inline void OnDestroyWrapper()
			{
				OnDestroy();
			}

		private:
			inline void SetFlag(EnGameObjectFlagTable flag, bool value) noexcept
			{
				m_flagTable[static_cast<int>(flag)] = value;
			}
			constexpr bool GetFlag(EnGameObjectFlagTable flag) const noexcept
			{
				return m_flagTable[static_cast<int>(flag)];
			}

		private:
			std::string m_name = "";
			std::bitset<static_cast<int>(EnGameObjectFlagTable::enNumFlags)> m_flagTable = 
				m_kEnableStartFlags;
		};

	}
}