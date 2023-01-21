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
			 * @brief リソースバンクにリソースを登録する
			 * @param filePath ファイルパス。これがキーになる。
			 * @param resource リソース
			*/
			void Register(const char* filePath, TResource* resource)
			{
				auto it = m_resourceMap.find(filePath);
				if (it == m_resourceMap.end())
				{
					// 未登録のため、新規登録する。
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
					// 登録済みのため、バンクからリソースを取ってくる。
					return it->second.get();
				}

				// 未登録
				return nullptr;
			}

		private:
			std::unordered_map<std::string, TResourcePtr> m_resourceMap = {};
		};
	}
}