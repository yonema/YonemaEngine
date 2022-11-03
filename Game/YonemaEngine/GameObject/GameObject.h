#pragma once
namespace nsYMEngine
{
	namespace nsGameObject
	{
		/**
		 * @brief �Q�[���I�u�W�F�N�g�̊��N���X�B
		 * ��{�I�ɃQ�[�����̃I�u�W�F�N�g�́A���̃N���X��public�Ōp�����Ă��������B
		*/
		class IGameObject : private nsUtils::SNoncopyable
		{
		private:

			enum class EnGameObjectFlagsTable
			{
				enStarted,
				enDead,
				enActive,
				enNumFlags
			};

			/**
			 * @brief �J�n�������L���ɂȂ鎞�̃t���O�r�b�g�Z�b�g�̑g�ݍ��킹
			*/
			static const std::bitset<static_cast<int>(EnGameObjectFlagsTable::enNumFlags)>
				m_kEnableStartFlags;
			/**
			 * @brief �X�V�������L���ɂȂ鎞�̃t���O�r�b�g�Z�b�g�̑g�ݍ��킹
			*/
			static const std::bitset<static_cast<int>(EnGameObjectFlagsTable::enNumFlags)>
				m_kEnableUpdateFlags;

		public:
			/**
			 * @brief �X�^�[�g�����B���̏����͎��g�𐶐��������̃t���[���̊J�n���Ɉ�x�����Ă΂�܂��B
			 * @return Update()�������s�����H
			 * @retval true Update()�������s��
			 * @retval false Update()�������s��Ȃ�
			*/
			virtual bool Start() { return true; }

			/**
			 * @brief ���̃Q�[���I�u�W�F�N�g���j�������Ƃ��ɌĂ΂�鏈���B
			*/
			virtual void OnDestroy() {}

			/**
			 * @brief �X�V�����B���t���[�����̏������Ă΂�܂��B
			 * @param[in] deltaTime �O��̃t���[������̌o�ߎ���(�b)
			*/
			virtual void Update(float deltaTime) {}

		public:
			constexpr IGameObject() = default;
			// �f�X�g���N�^����Ȃ�����Avirtual�ɂ��Ȃ��B
			// �f�X�g���N�^�ɏ�������������Ƃ���virtual��t�������Ă����B
			~IGameObject() = default;

			/**
			 * @brief �J�n�������Ă΂ꂽ�����ׂ�
			 * @return �J�n�������Ă΂ꂽ���H
			 * @retval true �J�n�������Ă΂ꂽ��
			 * @retval false �J�n�������܂��Ă΂�Ă��Ȃ�
			*/
			constexpr bool IsStarted() const noexcept
			{
				return GetFlag(EnGameObjectFlagsTable::enStarted);
			}

			/**
			 * @brief �A�N�e�B�u������B�X�V������J�n�������s����悤�ɂȂ�܂��B
			*/
			inline void Activate() noexcept
			{
				SetFlag(EnGameObjectFlagsTable::enActive, true);
			}

			/**
			 * @brief ��A�N�e�B�u������B�X�V������J�n�������s���Ȃ��Ȃ�܂��B
			*/
			inline void Deactivare() noexcept
			{
				SetFlag(EnGameObjectFlagsTable::enActive, false);
			}

			/**
			 * @brief �A�N�e�B�u��Ԃ����ׂ�
			 * @return �A�N�e�B�u��Ԃ��H
			 * @retval true �A�N�e�B�u���
			 * @retval false ��A�N�e�B�u���
			*/
			constexpr bool IsActive() const noexcept
			{
				return GetFlag(EnGameObjectFlagsTable::enActive);
			}

			/**
			 * @brief ���S�t���O�𗧂Ă�B
			 * ���g�̃I�u�W�F�N�g��j������Ƃ��́A���̊֐��ł͂Ȃ� DeleteGO<>() ���g�p���Ă��������B
			*/
			inline void Dead() noexcept
			{
				SetFlag(EnGameObjectFlagsTable::enDead, true);
			}

			/**
			 * @brief ���S�t���O�������Ă��邩���ׂ�
			 * @return ���S�t���O�������Ă��邩�H
			 * @retval true ���S�t���O�������Ă���
			 * @retval false ���S�t���O�������Ă��Ȃ�
			*/
			constexpr bool IsDead() const noexcept
			{
				return GetFlag(EnGameObjectFlagsTable::enDead);
			}

			/**
			 * @brief ���O��ݒ肵�܂��B���O�̓Q�[���I�u�W�F�N�g�̌����ȂǂŎg�p���܂��B
			 * @param[in] name �ݒ肷�閼�O
			*/
			constexpr void SetName(const char* name)
			{
				if (name != nullptr)
				{
					m_name = name;
				}
			}

			/**
			 * @brief ���g�̖��O�ƁA�����œn���ꂽ���O���������O�����ׂ�B
			 * @param[in] objectName ���g�̖��O�Ɣ�ׂ閼�O
			 * @return �������O���H
			 * @retval true �������O
			 * @retval false �Ⴄ���O
			*/
			bool IsMatchName(const char* objectName) const noexcept;

			/**
			 * @brief �J�n�����̃��b�v�֐��B
			 * ���̊֐���IGameObjectManager����̂ݎ��s����܂��B
			 * ���̏ꏊ����͎��s���Ȃ��ł��������B
			*/
			inline void StartWrapper() 
			{
				if (m_flags == m_kEnableStartFlags)
				{
					SetFlag(EnGameObjectFlagsTable::enStarted, Start());
				}
			};

			/**
			 * @brief �X�V�����̃��b�v�֐��B
			 * ���̊֐���IGameObjectManager����̂ݎ��s����܂��B
			 * ���̏ꏊ����͎��s���Ȃ��ł��������B
			 * @param[in] deltaTime �O��̃t���[������̌o�ߎ���(�b)
			*/
			inline void UpdateWrapper(float deltaTime) 
			{
				if (m_flags == m_kEnableUpdateFlags)
				{
					Update(deltaTime);
				}
			};

		private:
			inline void SetFlag(EnGameObjectFlagsTable flag, bool value) noexcept
			{
				m_flags[static_cast<int>(flag)] = value;
			}
			constexpr bool GetFlag(EnGameObjectFlagsTable flag) const noexcept
			{
				return m_flags[static_cast<int>(flag)];
			}

		private:
			std::string m_name = "";
			std::bitset<static_cast<int>(EnGameObjectFlagsTable::enNumFlags)> m_flags = 
				m_kEnableStartFlags;
		};

	}
}