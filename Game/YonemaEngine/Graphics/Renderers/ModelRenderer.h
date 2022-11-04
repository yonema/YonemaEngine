#pragma once
#include "RendererTable.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsRenderers
		{
			class IModelRendererBase;
		}
	}
}


namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsRenderers
		{
			enum class EnModelFormat
			{
				enNone = -1,
				enPMD,
				enFBX,
				enVRM,
				enNumModelFormat
			};

			struct SModelInitData
			{
				const char* modelFilePath = nullptr;
				const char* animFilePath = nullptr;
				EnModelFormat modelFormat = EnModelFormat::enNone;
				CRendererTable::EnRendererType rendererType = 
					CRendererTable::EnRendererType::enNone;
				nsMath::CQuaternion vertexBias = nsMath::CQuaternion::Identity();
				bool isVertesTranspos = false;
			};

			class CModelRenderer : public nsGameObject::IGameObject
			{
			private:
				static const char* 
					m_kModelFormatExtensions[static_cast<int>(EnModelFormat::enNumModelFormat)];

			public:
				bool Start() override final;

				void Update(float deltaTime) override final;

				void OnDestroy() override final;

			public:
				constexpr CModelRenderer() = default;
				~CModelRenderer() = default;

				void Init(const SModelInitData& modelInitData);

				inline void SetPosition(const nsMath::CVector3& position) noexcept
				{
					m_position = position;
				}
				constexpr const nsMath::CVector3& GetPosition() const noexcept
				{
					return m_position;
				}

				inline void SetRotation(const nsMath::CQuaternion& rotation) noexcept
				{
					m_rotation = rotation;
				}
				constexpr const nsMath::CQuaternion& GetRotation() const noexcept
				{
					return m_rotation;
				}

				inline void SetScale(const nsMath::CVector3& scale) noexcept
				{
					m_scale = scale;
				}
				inline void SetScale(float scale) noexcept
				{
					m_scale.Scale(scale);
				}
				constexpr const nsMath::CVector3& GetScale() const noexcept
				{
					return m_scale;
				}

			private:
				void Terminate();

				void CreateRenderer(
					EnModelFormat* pModelFormat, const SModelInitData& modelInitData);

				void RegistToRendererTable(
					EnModelFormat modelFormat, const SModelInitData& modelInitData);

				EnModelFormat FindMatchExtension(const char* extension);

			private:
				IModelRendererBase* m_renderer = nullptr;
				CRendererTable::EnRendererType m_rendererType =
					CRendererTable::EnRendererType::enNumType;

				nsMath::CVector3 m_position = nsMath::CVector3::Zero();
				nsMath::CQuaternion m_rotation = nsMath::CQuaternion::Identity();
				nsMath::CVector3 m_scale = nsMath::CVector3::One();
			};

		}
	}
}