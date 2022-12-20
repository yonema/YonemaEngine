#include "AnimationEventSample.h"

namespace nsAWA
{
	namespace nsSamples
	{
		const char* CAnimationEventSample::m_kModelFilePath =
			"Assets/Animations/Samples/Rumba_Dancing.fbx";
		const char* CAnimationEventSample::
			m_kAnimFilePaths[static_cast<int>(EnAnimType::enNum)] =
		{
			"Assets/Animations/Samples/animationWithEvent.fbx"
		};

		bool CAnimationEventSample::Start()
		{
			// InitModel
			{
				SAnimationInitData animInitData(
					static_cast<unsigned int>(EnAnimType::enNum), m_kAnimFilePaths);

				SModelInitData modelInitData;
				modelInitData.modelFilePath = m_kModelFilePath;
				modelInitData.animInitData = &animInitData;

				m_modelRenderer = NewGO<CModelRenderer>("SkeltalAnimationSampleModel");
				m_modelRenderer->Init(modelInitData);
				m_modelRenderer->SetPosition({ 0.0f, 4.0f, 0.0f });
				m_modelRenderer->SetScale(0.1f);
				m_modelRenderer->SetIsAnimationLoop(true);
				m_modelRenderer->SetAnimationSpeed(1.5f);
				m_modelRenderer->PlayAnimation(static_cast<unsigned int>(EnAnimType::enAnimWithEvent));
			}


			// SetAnimationEvent

			m_modelRenderer->ReserveAnimationEventFuncArray(
				static_cast<unsigned int>(EnAnimType::enAnimWithEvent), m_kNumAnimEvent);

			m_modelRenderer->AddAnimationEventFunc(
				static_cast<unsigned int>(EnAnimType::enAnimWithEvent),
				[&]() {AnimEventFunc1(); }
				);
			m_modelRenderer->AddAnimationEventFunc(
				static_cast<unsigned int>(EnAnimType::enAnimWithEvent),
				[&]() {AnimEventFunc2(); }
			);
			m_modelRenderer->AddAnimationEventFunc(
				static_cast<unsigned int>(EnAnimType::enAnimWithEvent),
				[&]() {AnimEventFunc3(); }
			);


			// InitFont
			{
				for (unsigned int eventIdx = 0; eventIdx < m_kNumAnimEvent; eventIdx++)
				{
					m_eventCounterFR[eventIdx] = NewGO<CFontRenderer>();
				}
				SFontParameter fontParam;
				fontParam.anchor = EnAnchors::enTopLeft;
				fontParam.pivot = { 0.0f,0.0f };
				fontParam.scale = 0.5f;

				fontParam.text = L"イベント1が呼ばれた回数　0回";
				fontParam.position = { 50.0f, 50.0f };
				m_eventCounterFR[0]->Init(fontParam);
				fontParam.text = L"イベント2が呼ばれた回数　0回";
				fontParam.position = { 50.0f, 100.0f };
				m_eventCounterFR[1]->Init(fontParam);
				fontParam.text = L"イベント3が呼ばれた回数　0回";
				fontParam.position = { 50.0f, 150.0f };
				m_eventCounterFR[2]->Init(fontParam);
			}

			return true;
		}

		void CAnimationEventSample::OnDestroy()
		{
			for (unsigned int eventIdx = 0; eventIdx < m_kNumAnimEvent; eventIdx++)
			{
				DeleteGO(m_eventCounterFR[eventIdx]);
			}
			DeleteGO(m_modelRenderer);

			return;
		}

		void CAnimationEventSample::Update(float deltaTime)
		{
			return;
		}


		void CAnimationEventSample::AnimEventFunc1() noexcept
		{
			constexpr unsigned int eventIdx = 0;
			m_eventCounter[eventIdx]++;
			constexpr size_t dispTextSize = sizeof(wchar_t) * static_cast<size_t>(64);
			wchar_t wText[64] = {};
			StringCbPrintf(
				wText,
				dispTextSize, L"イベント%dが呼ばれた回数　%d回",
				eventIdx + 1,
				m_eventCounter[eventIdx]
			);
			m_eventCounterFR[eventIdx]->SetText(wText);

			return;
		}
		void CAnimationEventSample::AnimEventFunc2() noexcept
		{
			constexpr unsigned int eventIdx = 1;
			m_eventCounter[eventIdx]++;
			constexpr size_t dispTextSize = sizeof(wchar_t) * static_cast<size_t>(64);
			wchar_t wText[64] = {};
			StringCbPrintf(
				wText,
				dispTextSize, L"イベント%dが呼ばれた回数　%d回",
				eventIdx + 1,
				m_eventCounter[eventIdx]
			);
			m_eventCounterFR[eventIdx]->SetText(wText);

			return;
		}
		void CAnimationEventSample::AnimEventFunc3() noexcept
		{
			constexpr unsigned int eventIdx = 2;
			m_eventCounter[eventIdx]++;
			constexpr size_t dispTextSize = sizeof(wchar_t) * static_cast<size_t>(64);
			wchar_t wText[64] = {};
			StringCbPrintf(
				wText,
				dispTextSize, L"イベント%dが呼ばれた回数　%d回",
				eventIdx + 1,
				m_eventCounter[eventIdx]
			);
			m_eventCounterFR[eventIdx]->SetText(wText);

			return;
		}
	}
}