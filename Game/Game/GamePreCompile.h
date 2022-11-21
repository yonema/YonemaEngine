#pragma once
#include "../YonemaEngine.h"
#include "../YonemaEngine/Graphics/GraphicsEngine.h"
#include "../YonemaEngine/Input/InputManager.h"
#include "../YonemaEngine/Graphics/Renderers/ModelRenderer.h"
#include "../YonemaEngine/Graphics/Animations/Animator.h"
#include "../YonemaEngine/Graphics/Renderers/SpriteRenderer.h"
#include "../YonemaEngine/Graphics/Sprites/Sprite.h"
#include "../YonemaEngine/Graphics/Fonts/FontRenderer.h"

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
using SAnimationInitData = nsGraphics::nsAnimations::SAnimationInitData;

// スプライト
using CSpriteRenderer = nsGraphics::nsRenderers::CSpriteRenderer;
using SSpriteInitData = nsGraphics::nsSprites::SSpriteInitData;

// フォント
using CFontRenderer = nsGraphics::nsFonts::CFontRenderer;
using SFontParameter = nsGraphics::nsFonts::CFontRenderer::SFontParameter;
using EnFontType = nsGraphics::nsFonts::EnFontType;

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