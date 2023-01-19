#pragma once

namespace nsYMEngine
{
	namespace nsLevel2D
	{
		struct SLevel2DSpriteData : private nsUtils::SNoncopyable
		{
		public:
			std::string Path = "";
			std::string Name = "";
			nsMath::CVector2 SpriteSize;
			nsMath::CVector2 Position;
			float RotateDeg = 0.0f;
			nsMath::CVector2 Scale;
			nsMath::CVector4 MulColor;
			nsMath::CVector2 Pivot;
			int Priority = 0;
		};
	}
}