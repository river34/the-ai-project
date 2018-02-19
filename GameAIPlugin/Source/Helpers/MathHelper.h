#pragma once

#include "CoreMinimal.h"
#include "GameMathLib.h"

class MathHelper
{
public:
	inline static GameMath::Vector3 getVector3(FVector _vec)
	{
		return GameMath::Vector3(_vec.X, _vec.Y, _vec.Z);
	}
};