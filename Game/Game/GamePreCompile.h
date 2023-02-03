#pragma once
#include "../YonemaEngine.h"
#include "../YonemaEngine/Graphics/GraphicsEngine.h"
#include "../YonemaEngine/Input/InputManager.h"
#include "../YonemaEngine/Graphics/Renderers/ModelRenderer.h"
#include "../YonemaEngine/Graphics/Animations/Animator.h"
#include "../YonemaEngine/Graphics/Renderers/SpriteRenderer.h"
#include "../YonemaEngine/Graphics/Renderers/AnimatedSpriteRenderer.h"
#include "../YonemaEngine/Graphics/Sprites/Sprite.h"
#include "../YonemaEngine/Graphics/Fonts/FontRenderer.h"
#include "../YonemaEngine/Physics/PhysicsEngine.h"
#include "../YonemaEngine/Physics/PhysicsStaticObject.h"
#include "../YonemaEngine/Physics/PhysicsDynamicObject.h"
#include "../YonemaEngine/Physics/PhysicsTriggerObject.h"
#include "../YonemaEngine/Physics/ContactEvent.h"
#include "../YonemaEngine/Physics/ExtendedDataForRigidActor.h"
#include "../YonemaEngine/Physics/CharacterController.h"
#include "../YonemaEngine/Effect/EffectPlayer.h"
#include "../YonemaEngine/Sound/SoundPlayer.h"
#include "../YonemaEngine/Utils/Random.h"
#include "../YonemaEngine/AI/Navigations/NavMesh.h"
#include "../YonemaEngine/AI/Navigations/Path.h"
#include "../YonemaEngine/AI/Navigations/PathFinding.h"
#include "../YonemaEngine/Level3D/Level3D.h"
#include "../YonemaEngine/Level2D/Level2D.h"

// エイリアス宣言

using namespace nsYMEngine;

// ゲームオブジェクト
using IGameObject = nsGameObject::IGameObject;

// インプット
using EnPadButton = nsInput::CGamepad::EnPadButton;
using EnKeyButton = nsInput::CKeyboard::EnKeyButton;
using EnActionMapping = nsInput::EnActionMapping;
using EnAxisMapping = nsInput::EnAxisMapping;

// モデル
using CModelRenderer = nsGraphics::nsRenderers::CModelRenderer;
using SModelInitData = nsGraphics::nsRenderers::SModelInitData;
using EnModelInitDataFlags = nsGraphics::nsRenderers::EnModelInitDataFlags;

// スプライト
using CSpriteRenderer = nsGraphics::nsRenderers::CSpriteRenderer;
using SSpriteInitData = nsGraphics::nsSprites::SSpriteInitData;
using EnAlphaBlendMode = nsGraphics::nsSprites::EnAlphaBlendMode;
using CAnimatedSpriteRenderer = nsGraphics::nsRenderers::CAnimatedSpriteRenderer;

// フォント
using CFontRenderer = nsGraphics::nsFonts::CFontRenderer;
using SFontParameter = nsGraphics::nsFonts::CFontRenderer::SFontParameter;
using EnFontType = nsGraphics::nsFonts::EnFontType;

// 物理
using CPhysicsStaticObject = nsPhysics::CPhysicsStaticObject;
using CPhysicsDynamicObject = nsPhysics::CPhysicsDynamicObject;
using CPhysicsTriggerObject = nsPhysics::CPhysicsTriggerObject;
using SContactEvent = nsPhysics::SContactEvent;
using CExtendedDataForRigidActor = nsPhysics::CExtendedDataForRigidActor;
using EnPhysicsAttributes = nsPhysics::EnPhysicsAttributes;
using CCharacterController = nsPhysics::CCharacterController;

// エフェクト
using CEffectPlayer = nsEffect::CEffectPlayer;

// サウンド
using CSoundPlayer = nsSound::CSoundPlayer;
using EnSubmixType = nsSound::EnSubmixType;

// ナビゲーション
using CNavMesh = nsAI::nsNavigations::CNavMesh;
using CPath = nsAI::nsNavigations::CPath;
using CPathFinding = nsAI::nsNavigations::CPathFinding;

// レベル3D
using CLevel3D = nsLevel3D::CLevel3D;
using CLevel2D = nsLevel2D::CLevel2D;
using SLevelChipData = nsLevel3D::SLevelChipData;
using SLevel3DInitData = nsLevel3D::SLevel3DinitData;

// 数学
using CVector2 = nsMath::CVector2;
using CVector3 = nsMath::CVector3;
using CVector4 = nsMath::CVector4;
using CQuaternion = nsMath::CQuaternion;
using CMatrix = nsMath::CMatrix;


// ヘルパー関数

/**
 * @brief ゲームを終了します。
*/
static inline void ExitGame()
{
	CYonemaEngine::GetInstance()->ExitGame();
}

/**
 * @brief メインカメラを取得します。
 * @return メインカメラのポインタ
*/
static constexpr auto MainCamera()
{
	return nsGraphics::CGraphicsEngine::GetInstance()->GetMainCamera();
}


//////// 入力 ////////

/**
 * @brief ゲームパッド入力のインターフェースを取得します。
 * @return ゲームパッド入力のインターフェース
*/
static constexpr const nsInput::CGamepad* const Gamepad(int userIdx = 0)
{
	return CYonemaEngine::GetInstance()->GetInput()->GetGamepad(userIdx);
}

static constexpr const nsInput::CKeyboard* const Keyboard()
{
	return CYonemaEngine::GetInstance()->GetInput()->GetKeyboard();
}

static constexpr const nsInput::CInputManager* const Input()
{
	return CYonemaEngine::GetInstance()->GetInput();
}


//////// 物理 ////////

static void EnableDebugDrawPhysicsLine() noexcept
{
	nsPhysics::CPhysicsEngine::GetInstance()->EnableDebugDrawLine();
}
static void DisableDebugDrawPhysicsLine() noexcept
{
	nsPhysics::CPhysicsEngine::GetInstance()->DisableDebugDrawLine();
}
static bool IsEnableDebugDrawPhysicsLine() noexcept
{
	return nsPhysics::CPhysicsEngine::GetInstance()->IsEnableDebugDrawLine();
}

static void SetCullingBoxForDebugDrawLine(float halfExtent, const nsMath::CVector3& center) noexcept
{
	nsPhysics::CPhysicsEngine::GetInstance()->SetCullingBoxForDebugDrawLine(halfExtent, center);
}

static void EnableAutoFitCullingBoxToMainCamera() noexcept
{
	nsPhysics::CPhysicsEngine::GetInstance()->EnableAutoFitCullingBoxToMainCamera();
}
static void DisableAutoFitCullingBoxToMainCamera() noexcept
{
	nsPhysics::CPhysicsEngine::GetInstance()->DisableAutoFitCullingBoxToMainCamera();
}
static bool IsEnableAutoFitCullingBoxToMainCamera() noexcept
{
	return nsPhysics::CPhysicsEngine::GetInstance()->IsEnableAutoFitCullingBoxToMainCamera();
}


// 乱数
static nsUtils::CRandom* Random() noexcept
{
	return CYonemaEngine::GetInstance()->GetRandom();
}