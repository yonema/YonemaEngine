#pragma once

#define NOMINMAX
#include <Windows.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <d3dx12.h>

#include <DirectXMath.h>
#include <DirectXTex.h>
#include <GraphicsMemory.h>

#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <array>
#include <string>
#include <functional>
#include <memory>
#include <algorithm>
#include <bitset>
#include <limits>
#include <strsafe.h>
#include <typeinfo>
#include <random>
#include <thread>
#include <mutex>

// DirectX12
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

// 画像
#pragma comment(lib, "DirectXTex.lib")

// フォントなど
#pragma comment(lib, "DirectXTK12.lib")
#pragma comment(lib, "dxguid.lib")

// 入力
#pragma comment(lib, "xinput.lib")


// 物理(PhysX)
#pragma comment (lib, "PhysXCommon_64.lib")
#pragma comment (lib, "PhysX_64.lib")
#pragma comment (lib, "PhysXFoundation_64.lib")
#pragma comment (lib, "PhysXCooking_64.lib")
#pragma comment (lib, "PhysXExtensions_static_64.lib")
#pragma comment (lib, "PhysXPvdSDK_static_64.lib")
#pragma comment (lib, "SnippetUtils_static_64.lib")

// エフェクト
#pragma comment (lib, "Effekseer.lib")
#pragma comment (lib, "EffekseerRendererDX12.lib")
#pragma comment (lib, "LLGI.lib")


#include "../Utils/Noncopyable.h"
#include "../Utils/StringManipulation.h"
#include "../Math/Math.h"
#include "../Math/Vector2.h"
#include "../Math/Vector3.h"
#include "../Math/Vector4.h"
#include "../Math/Quaternion.h"
#include "../Math/Matrix.h"
#include "../GameWindow/MessageBox.h"
#include "../Graphics/Dx12Wrappers/Dx12Wrappers_Include.h"
#include "../CommonLibrary/CommonLibrary.h"

#include "../../Game/GamePreCompile.h"