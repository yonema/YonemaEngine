#pragma once

#include "../YonemaEngine/Graphics/Renderers/ModelRenderer.h"
#include "../YonemaEngine/Graphics/Renderers/SpriteRenderer.h"
#include "../YonemaEngine/Graphics/2D/Sprite.h"

using namespace nsYMEngine;

using IGameObject = nsGameObject::IGameObject;
using CModelRenderer = nsGraphics::nsRenderers::CModelRenderer;
using SModelInitData = nsGraphics::nsRenderers::SModelInitData;
using CSpriteRenderer = nsGraphics::nsRenderers::CSpriteRenderer;
using SSpriteInitData = nsGraphics::ns2D::SSpriteInitData;

using CVector2 = nsMath::CVector2;
using CVector3 = nsMath::CVector3;
using CVector4 = nsMath::CVector4;
using CQuaternion = nsMath::CQuaternion;
using CMatrix = nsMath::CMatrix;