#pragma once

namespace nsAWA
{
	class CGame : public nsGameObject::IGameObject
	{
	private:
		static const char* const m_kGameObjectName;

	public:
		bool Start() override final;

		void Update(float deltaTime) override final;

		void OnDestroy() override final;

	public:
		constexpr CGame() = default;
		~CGame() = default;

	private:
		int m_debugNum = 0;
	};

}