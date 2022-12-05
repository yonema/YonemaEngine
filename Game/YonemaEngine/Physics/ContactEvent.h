#pragma once
namespace nsYMEngine
{
	namespace nsPhysics
	{
		class CExtendedDataForRigidActor;
	}
}
namespace nsYMEngine
{
	namespace nsPhysics
	{
		struct SContactEvent : private nsUtils::SNoncopyable
		{
			using ContactEventFunc = std::function<void(CExtendedDataForRigidActor*)>;

			constexpr SContactEvent() = default;
			~SContactEvent() = default;

			ContactEventFunc m_onTriggerEnterFunc = [](CExtendedDataForRigidActor*){};
			ContactEventFunc m_onTriggerExitFunc = [](CExtendedDataForRigidActor*){};
			ContactEventFunc m_onTriggerStayFunc = [](CExtendedDataForRigidActor*){};
		};
	}
}
