#pragma once
#include "ContactEvent.h"
#include "../../Game/PhysicsAttributes.h"

namespace nsYMEngine
{
	namespace nsPhysics
	{
		class IPhysicsObject;
	}
}
namespace nsYMEngine
{
	namespace nsPhysics
	{
		class CExtendedDataForRigidActor : nsUtils::SNoncopyable
		{
		public:
			inline CExtendedDataForRigidActor()
			{
				m_extendedDataCollector.emplace_back(this);
			}
			~CExtendedDataForRigidActor() = default;

			template <class OwnerClass>
			constexpr void Init(
				OwnerClass* owner,
				IPhysicsObject* physicsObject,
				EnPhysicsAttributes attribute = EnPhysicsAttributes::enNone) noexcept
			{
				SetOwner<OwnerClass>(owner);
				SetPhysicsObject(physicsObject);
				SetPhysicsAttribute(attribute);
			}

			constexpr void Init(
				IPhysicsObject* physicsObject,
				EnPhysicsAttributes attribute = EnPhysicsAttributes::enNone) noexcept
			{
				SetPhysicsObject(physicsObject);
				SetPhysicsAttribute(attribute);
			}

			constexpr SContactEvent* GetContactEvent() noexcept
			{
				return &m_contactEvent;
			}

			template <class OwnerClass>
			constexpr void SetOwner(OwnerClass* owner) noexcept
			{
				m_owner = static_cast<OwnerClass*>(owner);
				m_typeInfo = &typeid(OwnerClass);
			}

			template <class OwnerClass>
			constexpr OwnerClass* GetOwner() noexcept
			{
				return IsSameClass<OwnerClass>() ? static_cast<OwnerClass*>(m_owner) : nullptr;
			}

			constexpr void SetPhysicsObject(IPhysicsObject* physicsObject) noexcept
			{
				m_physicsObject = physicsObject;
			}

			constexpr IPhysicsObject* GetPhysicsObject() noexcept
			{
				return m_physicsObject;
			}

			constexpr void SetPhysicsAttribute(EnPhysicsAttributes attribute) noexcept
			{
				m_physicsAttribute = attribute;
			}
			
			constexpr EnPhysicsAttributes GetPhysicsAttribute() const noexcept
			{
				return m_physicsAttribute;
			}

			constexpr const type_info* GetTypeInfo() const noexcept
			{
				return m_typeInfo;
			}

			constexpr bool HasOwner() const noexcept
			{
				return m_owner ? true : false;
			}

			constexpr bool HasPhysicsObject() const noexcept
			{
				return m_physicsObject ? true : false;
			}

			template <class CheckClass>
			constexpr bool IsSameClass()
			{
				return HasOwner() ? *m_typeInfo == typeid(CheckClass) : false;
			}

			constexpr void Dead() noexcept
			{
				m_isDead = true;
			}

			constexpr bool IsDead() const noexcept
			{
				return m_isDead;
			}

			static void DeleteDeadExtendedData();

			static void DeleteAllExtendedData();

		private:
			SContactEvent m_contactEvent;
			void* m_owner = nullptr;
			IPhysicsObject* m_physicsObject = nullptr;
			EnPhysicsAttributes m_physicsAttribute = EnPhysicsAttributes::enNone;
			const type_info* m_typeInfo = nullptr;

			static std::list<CExtendedDataForRigidActor*> m_extendedDataCollector;
			bool m_isDead = false;
		};
	}
}
