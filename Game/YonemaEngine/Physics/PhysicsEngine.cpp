#include "PhysicsEngine.h"
#include "PhysicsCommonFunc.h"
#include "PhysicsObject.h"
#include "PhysicsTriggerObject.h"
#include "../Graphics/GraphicsEngine.h"
#include "../Graphics/DebugRenderers/PhysicsDebugLineRenderer.h"
#include "../snippets/snippetutils/SnippetUtils.h"

namespace nsYMEngine
{
	namespace nsPhysics
	{
		namespace
		{
			#define PVD_HOST "127.0.0.1"
		}

		const bool CPhysicsEngine::m_kEnableCCD = true;
		const float CPhysicsEngine::m_kGravityStrength = -9.81f;
		const float CPhysicsEngine::m_kDefaultStaticFriction = 0.5f;
		const float CPhysicsEngine::m_kDefaultDynamicFriction = 0.5f;
		const float CPhysicsEngine::m_kDefaultRestitution = 0.6f;
#ifdef _DEBUG
		const unsigned int CPhysicsEngine::m_kMaxMyDebugLine = 100;
#endif // _DEBUG



		CPhysicsEngine* CPhysicsEngine::m_instance = nullptr;
		physx::PxDefaultAllocator CPhysicsEngine::m_defaultAllocatorCallback = {};
		physx::PxDefaultErrorCallback CPhysicsEngine::m_defaultErrorCallback = {};
		CContactReportCallback CPhysicsEngine::m_contactReportCallback = {};



		static	physx::PxFilterFlags TriggersUsingFilterShader(
			physx::PxFilterObjectAttributes /*attributes0*/,
			physx::PxFilterData filterData0,
			physx::PxFilterObjectAttributes /*attributes1*/,
			physx::PxFilterData filterData1,
			physx::PxPairFlags& pairFlags,
			const void* /*constantBlock*/,
			physx::PxU32 /*constantBlockSize*/)
		{
			//	printf("contactReportFilterShader\n");

			PX_ASSERT(getImpl() == FILTER_SHADER);

			// We need to detect whether one of the shapes is a trigger.
			const bool isTriggerPair = IsTrigger(filterData0) || IsTrigger(filterData1);

			// If we have a trigger, replicate the trigger codepath from PxDefaultSimulationFilterShader
			if (isTriggerPair)
			{
				pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;

				if (CPhysicsEngine::UsesCCD())
					pairFlags |= physx::PxPairFlag::eDETECT_CCD_CONTACT;

				return physx::PxFilterFlag::eDEFAULT;
			}
			else
			{
				// Otherwise use the default flags for regular pairs
				pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;
				return physx::PxFilterFlag::eDEFAULT;
			}
		}

		CPhysicsEngine::CPhysicsEngine()
		{
			Init();

			return;
		}

		CPhysicsEngine::~CPhysicsEngine()
		{
			Terminate();
			return;
		}

		void CPhysicsEngine::Terminate()
		{
			// 破棄する前に同期する。
			m_scene->fetchResults(true);

			CExtendedDataForRigidActor::DeleteAllExtendedData();

			if (m_physicsDebugLineRenderer)
			{
				delete m_physicsDebugLineRenderer;
				m_physicsDebugLineRenderer = nullptr;
			}
			m_myDebugLineArray.clear();

			PX_RELEASE(m_scene);
			PX_RELEASE(m_dispatcher);
			PX_RELEASE(m_physics);
			PX_RELEASE(m_cooking);
			PxCloseExtensions();

			if (m_pvd)
			{
				physx::PxPvdTransport* transport = m_pvd->getTransport();
				m_pvd->release();
				m_pvd = nullptr;
				PX_RELEASE(transport);
			}
			PX_RELEASE(m_foundation);

			return;
		}


		void MyOnTriggerEnter(int num)
		{
			int a = num;
			int b = a;
			return;
		}

		bool CPhysicsEngine::Init()
		{
			if (InitPhysics() != true)
			{
				return false;
			}

			if (InitScene() != true)
			{
				return false;
			}

			m_defaultMaterial = m_physics->createMaterial(
				m_kDefaultStaticFriction, m_kDefaultDynamicFriction, m_kDefaultRestitution);

#ifdef _DEBUG
			InitDebugging();
#endif // _DEBUG

			return true;
		}

		bool CPhysicsEngine::InitPhysics()
		{
			// Foundation生成
			m_foundation = PxCreateFoundation(
				PX_PHYSICS_VERSION, m_defaultAllocatorCallback, m_defaultErrorCallback);
			if (m_foundation == nullptr)
			{
				nsGameWindow::MessageBoxError(L"PhysXのFoundationオブジェクトの生成に失敗しました。");
				return false;
			}

			// PVD生成
			m_pvd = physx::PxCreatePvd(*m_foundation);
			auto* transport = physx::PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
			m_pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

			// Physics生成
			bool recordMemoryAllactions = true;
			m_physics = PxCreatePhysics(
				PX_PHYSICS_VERSION,
				*m_foundation,
				physx::PxTolerancesScale(),
				recordMemoryAllactions,
				m_pvd
			);
			if (m_physics == nullptr)
			{
				nsGameWindow::MessageBoxError(L"PhysXのPhysicsオブジェクトの生成に失敗しました。");
				return false;
			}

			// Cooking生成
			m_cooking = PxCreateCooking(
				PX_PHYSICS_VERSION,
				*m_foundation,
				physx::PxCookingParams(/*scale*/physx::PxTolerancesScale())
			);
			if (m_cooking == nullptr)
			{
				nsGameWindow::MessageBoxError(L"PhysXのCookinfオブジェクトの生成に失敗しました。");
				return false;
			}

			// Extensions生成
			if (PxInitExtensions(*m_physics, m_pvd) != true)
			{
				nsGameWindow::MessageBoxError(L"PhysXの拡張機能の初期化に失敗しました。");
				return false;
			}


			return true;
		}

		bool CPhysicsEngine::InitScene()
		{
			physx::PxSceneDesc sceneDesc(m_physics->getTolerancesScale());

			sceneDesc.gravity = physx::PxVec3(0.0f, m_kGravityStrength, 0.0f);
			sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
			//sceneDesc.filterShader = TriggersUsingFilterShader;
			sceneDesc.simulationEventCallback = &m_contactReportCallback;

			if (m_kEnableCCD)
			{
				sceneDesc.flags |= physx::PxSceneFlag::eENABLE_CCD;
			}

			const physx::PxU32 numCores = physx::SnippetUtils::getNbPhysicalCores();
			m_dispatcher = physx::PxDefaultCpuDispatcherCreate(numCores == 0 ? 0 : numCores - 1);
			sceneDesc.cpuDispatcher = m_dispatcher;

			m_scene = m_physics->createScene(sceneDesc);

			if (m_scene == nullptr)
			{
				nsGameWindow::MessageBoxError(L"PhysXのSceneの生成に失敗しました。");
				return false;
			}

			physx::PxPvdSceneClient* pvdClient = m_scene->getScenePvdClient();
			if (pvdClient)
			{
				pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
				pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
				pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
			}

			return true;
		}


		void CPhysicsEngine::Update(float deltaTime)
		{
			m_scene->simulate(deltaTime);
			m_scene->fetchResults(true);

			for (auto& physicsTriggerObject : m_physicsTriggerObjectList)
			{
				if (physicsTriggerObject->IsActive() != true)
				{
					continue;
				}
				physicsTriggerObject->UpdateContactEvent();
			}

#ifdef _DEBUG
			UpdateDebugData();
#endif // _DEBUG

			// ExtendedDataは、simulateとfetchResultsが呼ばれた後破棄する。
			// コールバック時にExtendedDataがnullになるのを防ぐため。
			CExtendedDataForRigidActor::DeleteDeadExtendedData();
			return;
		}



		bool CPhysicsEngine::RayCast(
			const nsMath::CVector3& origin,
			const nsMath::CVector3& normalDir,
			float distance,
			physx::PxRaycastBuffer* hitCallback,
			const nsMath::CVector4& color
		)
		{
			nsMath::CVector3 toPos = origin + (normalDir * distance);
			m_myDebugLineArray.emplace_back(
				SMyDebugLine(
					origin,
					{ color.r, color.g, color.b },
					toPos,
					{ color.r, color.g, color.b }
				)
			);
			if (m_myDebugLineArray.size() > m_kMaxMyDebugLine)
			{
				nsGameWindow::MessageBoxError(L"レイの数が多すぎます。");
				std::abort();
			}

			return m_scene->raycast(
				physx::PxVec3(origin.x, origin.y, origin.z),
				physx::PxVec3(normalDir.x, normalDir.y, normalDir.z),
				distance,
				*hitCallback
			);
		}

		bool CPhysicsEngine::OverlapMultiple(
			const IPhysicsObject& physicsObject, physx::PxOverlapBuffer* hitCallbackBuffer)
		{
			const auto* actor = physicsObject.GetActor();

			physx::PxShape* shapes[1];
			actor->getShapes(shapes, 1);
			const auto& overlapGeometry = shapes[0]->getGeometry().any();

			const auto& finalPose = physx::PxShapeExt::getGlobalPose(*shapes[0], *actor);


			static const physx::PxQueryFilterData filter(
				physx::PxQueryFlag::eNO_BLOCK |
				physx::PxQueryFlag::eDYNAMIC |
				physx::PxQueryFlag::eSTATIC
			);

			// @attention 自分自身(引数で渡されたphysicsObject)とも衝突します。
			return m_scene->overlap(overlapGeometry, finalPose, *hitCallbackBuffer, filter);

		}



		bool CPhysicsEngine::Sweep(
			const IPhysicsObject& physicsObject,
			const nsMath::CVector3& normalDir,
			float distance,
			physx::PxSweepCallback* hitCallback,
			physx::PxQueryFilterCallback* filterCallback,
			float inflation
		)
		{
			const auto* actor = physicsObject.GetActor();

			physx::PxShape* shapes[1];
			actor->getShapes(shapes, 1);
			const auto& sweepGeometry = shapes[0]->getGeometry().any();
			const auto& finalPose = physx::PxShapeExt::getGlobalPose(*shapes[0], *actor);


			static const physx::PxHitFlags hitFlags =
				physx::PxHitFlag::ePOSITION |
				physx::PxHitFlag::eNORMAL |
				//physx::PxHitFlag::ePRECISE_SWEEP |
				physx::PxHitFlag::eMESH_ANY;
			static const physx::PxQueryFilterData filter(
				physx::PxQueryFlag::eANY_HIT |
				physx::PxQueryFlag::eDYNAMIC |
				physx::PxQueryFlag::eSTATIC |
				physx::PxQueryFlag::ePREFILTER
			);


			return m_scene->sweep(
				sweepGeometry,
				finalPose,
				{ normalDir.x, normalDir.y, normalDir.z },
				distance,
				*hitCallback,
				hitFlags,
				filter,
				filterCallback,
				nullptr,
				inflation
			);
		}

		bool CPhysicsEngine::SweepMultiple(
			const IPhysicsObject& physicsObject,
			const nsMath::CVector3& normalDir,
			float distance,
			physx::PxSweepBuffer* hitCallbackBuffer,
			physx::PxQueryFilterCallback* filterCallback,
			float inflation
		)
		{
			const auto* actor = physicsObject.GetActor();

			physx::PxShape* shapes[1];
			actor->getShapes(shapes, 1);
			const auto& sweepGeometry = shapes[0]->getGeometry().any();
			const auto& finalPose = physx::PxShapeExt::getGlobalPose(*shapes[0], *actor);


			static const physx::PxHitFlags hitFlags =
				physx::PxHitFlag::ePOSITION |
				physx::PxHitFlag::eNORMAL/* |
				physx::PxHitFlag::ePRECISE_SWEEP*/;
			static const physx::PxQueryFilterData filter(
				physx::PxQueryFlag::eDYNAMIC |
				physx::PxQueryFlag::eSTATIC |
				physx::PxQueryFlag::ePREFILTER
			);


			return m_scene->sweep(
				sweepGeometry,
				finalPose,
				{ normalDir.x, normalDir.y, normalDir.z },
				distance,
				*hitCallbackBuffer,
				hitFlags,
				filter,
				filterCallback,
				nullptr,
				inflation
			);
		}

		void CPhysicsEngine::AddPhysicsTriggerObject(CPhysicsTriggerObject* object)
		{
			m_physicsTriggerObjectList.emplace_back(object);
			return;
		}

		void CPhysicsEngine::RemovePhysicsTriggerObject(CPhysicsTriggerObject* object)
		{
			m_physicsTriggerObjectList.remove(object);
			return;
		}


#pragma region DebugSystem
		void CPhysicsEngine::InitDebugging()
		{
#ifdef _DEBUG
			m_scene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0f);

			m_scene->setVisualizationParameter(physx::PxVisualizationParameter::eACTOR_AXES, 2.0f);
			m_scene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);

			//SetCullingBoxForDebugDrawLine(1000.0f, nsMath::CVector3::Zero());

			m_physicsDebugLineRenderer = new nsGraphics::nsDebugRenderers::CPhysicsDebugLineRenderer();

			m_myDebugLineArray.reserve(m_kMaxMyDebugLine);
#endif // _DEBUG
			return;
		}

		void CPhysicsEngine::UpdateDebugData()
		{
#ifdef _DEBUG
			if (m_enableDebugDrawLine != true)
			{
				return;
			}

			AutoFitCullingBoxToMainCamere();

			const physx::PxRenderBuffer& rb = m_scene->getRenderBuffer();
			for (physx::PxU32 i = 0; i < rb.getNbLines(); i++)
			{
				const physx::PxDebugLine& line = rb.getLines()[i];

				nsMath::CVector3 fromPos, toPos, fromColor, toColor;
				memcpy_s(&fromPos, sizeof(nsMath::CVector3), &line.pos0, sizeof(physx::PxVec3));
				memcpy_s(&toPos, sizeof(nsMath::CVector3), &line.pos1, sizeof(physx::PxVec3));
				fromColor = ConverteFromPxDebugColorToMyVec3(line.color0);
				toColor = ConverteFromPxDebugColorToMyVec3(line.color1);

				// render the line
				m_physicsDebugLineRenderer->AddVertex(fromPos, toPos, fromColor, toColor);
			}

			for (auto& line : m_myDebugLineArray)
			{
				m_physicsDebugLineRenderer->AddVertex(
					line.pos0, line.pos1, line.color0, line.color1);
			}
			m_myDebugLineArray.clear();
#endif // _DEBUG
			return;
		}

		void CPhysicsEngine::SetCullingBoxForDebugDrawLine(
			float halfExtent, const nsMath::CVector3& center) noexcept
		{
#ifdef _DEBUG
			m_cullingBoxHalfExtent = halfExtent;
			m_scene->setVisualizationCullingBox(
				physx::PxBounds3::centerExtents(
					{ center.x, center.y, center.z },
					{ m_cullingBoxHalfExtent , m_cullingBoxHalfExtent, m_cullingBoxHalfExtent }
				)
			);
#endif // _DEBUG
			return;
		}

		void CPhysicsEngine::AutoFitCullingBoxToMainCamere()
		{
			if (m_enableAutoFitCullingBoxToMainCamera != true)
			{
				return;
			}

			auto* camera = nsGraphics::CGraphicsEngine::GetInstance()->GetMainCamera();

			const auto& origin = camera->GetPosition();
			const auto& dirVec = camera->GetForeardDirection();

			nsMath::CVector3 toCenterVec(dirVec * m_cullingBoxHalfExtent);
			nsMath::CVector3 centerPos = origin + toCenterVec;

			SetCullingBoxForDebugDrawLine(m_cullingBoxHalfExtent, centerPos);

			return;
		}

#pragma endregion DebugSystem



	}
}