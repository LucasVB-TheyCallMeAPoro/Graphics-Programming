#pragma once
#include "EMath.h"
struct Ray
{
	float tMin = 0.0001f;
	float tMax = FLT_MAX;
	Elite::FVector3 direction;
	Elite::FPoint3 origin;
	Elite::FPoint3 screenPoint;

	bool inverse;
};