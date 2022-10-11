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
			TRsourceBank() = default;
			~TRsourceBank() = default;

			/**
			 * @brief リソースバンクにリソースを登録する
			 * @param filePath ファイルパス。これがキーになる。
			 * @param resource リソース
			*/
			void Regist(const char* filePath, TResource* resource)
			{
				auto it = m_resourceMap.find(filePath);
				if (it == m_resourceMap.end())
				{
					m_resourceMap.emplace(filePath, resource);
				}
			}

			/**
			 * @brief リソースバンクからリソースを得る。
			 * @param filePath ファイルパス。これがキーになる。
			 * @return リソース。未登録の場合nullptr。
			*/
			TResource* Get(const char* filePath)
			{
				auto it = m_resourceMap.find(filePath);
				if (it != m_resourceMap.end()) 
				{
					return it->second.get();
				}
				return nullptr;
			}

		private:
			std::unordered_map<const char*, TResourcePtr> m_resourceMap;
		};
	}
}