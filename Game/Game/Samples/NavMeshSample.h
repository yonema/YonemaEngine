#pragma once
namespace nsAWA
{
	namespace nsSamples
	{
		class CNavMeshSample : public IGameObject
		{
		public:
			bool Start() override final;

			void OnDestroy() override final;

			void Update(float deltaTime) override final;

		public:
			constexpr CNavMeshSample() = default;
			~CNavMeshSample() = default;

		private:
			CNavMesh m_navMesh = {};
			CPath m_path = {};
			CPathFinding m_pathFinding = {};
			CLevel3D m_level3D = {};
			CModelRenderer* m_aiMoveMR = nullptr;
			CModelRenderer* m_targetMR = nullptr;
		};

	}
}