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

// �G�C���A�X�錾

using namespace nsYMEngine;

// �Q�[���I�u�W�F�N�g
using IGameObject = nsGameObject::IGameObject;

// �C���v�b�g
using EnPadButton = nsInput::CGamepad::EnPadButton;
using EnKeyButton = nsInput::CKeyboard::EnKeyButton;
using EnActionMapping = nsInput::EnActionMapping;
using EnAxisMapping = nsInput::EnAxisMapping;

// ���f��
using CModelRenderer = nsGraphics::nsRenderers::CModelRenderer;
using SModelInitData = nsGraphics::nsRenderers::SModelInitData;
using EnModelInitDataFlags = nsGraphics::nsRenderers::EnModelInitDataFlags;

// �X�v���C�g
using CSpriteRenderer = nsGraphics::nsRenderers::CSpriteRenderer;
using SSpriteInitData = nsGraphics::nsSprites::SSpriteInitData;
using EnAlphaBlendMode = nsGraphics::nsSprites::EnAlphaBlendMode;
using CAnimatedSpriteRenderer = nsGraphics::nsRenderers::CAnimatedSpriteRenderer;

// �t�H���g
using CFontRenderer = nsGraphics::nsFonts::CFontRenderer;
using SFontParameter = nsGraphics::nsFonts::CFontRenderer::SFontParameter;
using EnFontType = nsGraphics::nsFonts::EnFontType;

// ����
using CPhysicsStaticObject = nsPhysics::CPhysicsStaticObject;
using CPhysicsDynamicObject = nsPhysics::CPhysicsDynamicObject;
using CPhysicsTriggerObject = nsPhysics::CPhysicsTriggerObject;
using SContactEvent = nsPhysics::SContactEvent;
using CExtendedDataForRigidActor = nsPhysics::CExtendedDataForRigidActor;
using EnPhysicsAttributes = nsPhysics::EnPhysicsAttributes;
using CCharacterController = nsPhysics::CCharacterController;

// �G�t�F�N�g
using CEffectPlayer = nsEffect::CEffectPlayer;

// �T�E���h
using CSoundPlayer = nsSound::CSoundPlayer;
using EnSubmixType = nsSound::EnSubmixType;

// �i�r�Q�[�V����
using CNavMesh = nsAI::nsNavigations::CNavMesh;
using CPath = nsAI::nsNavigations::CPath;
using CPathFinding = nsAI::nsNavigations::CPathFinding;

// ���x��3D
using CLevel3D = nsLevel3D::CLevel3D;
using CLevel2D = nsLevel2D::CLevel2D;
using SLevelChipData = nsLevel3D::SLevelChipData;
using SLevel3DInitData = nsLevel3D::SLevel3DinitData;

// ���w
using CVector2 = nsMath::CVector2;
using CVector3 = nsMath::CVector3;
using CVector4 = nsMath::CVector4;
using CQuaternion = nsMath::CQuaternion;
using CMatrix = nsMath::CMatrix;


// �w���p�[�֐�

/**
 * @brief �Q�[�����I�����܂��B
*/
static inline void ExitGame()
{
	CYonemaEngine::GetInstance()->ExitGame();
}

/**
 * @brief ���C���J�������擾���܂��B
 * @return ���C���J�����̃|�C���^
*/
static constexpr auto MainCamera()
{
	return nsGraphics::CGraphicsEngine::GetInstance()->GetMainCamera();
}


//////// ���� ////////

/**
 * @brief �Q�[���p�b�h���͂̃C���^�[�t�F�[�X���擾���܂��B
 * @return �Q�[���p�b�h���͂̃C���^�[�t�F�[�X
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


//////// ���� ////////

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


// ����
static nsUtils::CRandom* Random() noexcept
{
	return CYonemaEngine::GetInstance()->GetRandom();
}