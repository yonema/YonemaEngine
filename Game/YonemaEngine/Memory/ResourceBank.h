#pragma once

namespace nsYMEngine
{
	namespace nsMemory
	{
		template<class TResource>
		class TRsourceBank : private nsUtils::SNoncopyable
		{
		private:
			using TResourcePtr = std::unique_ptr<TResource>;
		public:
			constexpr TRsourceBank() = default;
			~TRsourceBank() = default;

			/**
			 * @brief ���\�[�X�o���N�Ƀ��\�[�X��o�^����
			 * @param filePath �t�@�C���p�X�B���ꂪ�L�[�ɂȂ�B
			 * @param resource ���\�[�X
			*/
			void Register(const char* filePath, TResource* resource)
			{
				auto it = m_resourceMap.find(filePath);
				if (it == m_resourceMap.end())
				{
					// ���o�^�̂��߁A�V�K�o�^����B
					m_resourceMap.emplace(filePath, resource);
				}
			}

			/**
			 * @brief ���\�[�X�o���N���烊�\�[�X�𓾂�B
			 * @param filePath �t�@�C���p�X�B���ꂪ�L�[�ɂȂ�B
			 * @return ���\�[�X�B���o�^�̏ꍇnullptr�B
			*/
			TResource* Get(const char* filePath)
			{
				auto it = m_resourceMap.find(filePath);
				if (it != m_resourceMap.end()) 
				{
					// �o�^�ς݂̂��߁A�o���N���烊�\�[�X������Ă���B
					return it->second.get();
				}

				// ���o�^
				return nullptr;
			}

		private:
			std::unordered_map<std::string, TResourcePtr> m_resourceMap = {};
		};
	}
}