

// シャドウデプスの最大値
static float g_kMaxShadowDepth = 500.0f;

// EVSM用の低の数
static const int g_kExp = 40.0f;

// シャドウアクネ回避のための深度バイアス
static const float g_kDepthBias = 0.001f;

// 距離によるバイアスの指数
static const float g_kDistBiasPower = 4.0f;

// 1/√2
static const float g_k1DivRoot2 = 0.7071067811865475244008f;

// EVSM
#define EVSM