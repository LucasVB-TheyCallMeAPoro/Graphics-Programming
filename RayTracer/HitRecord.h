#pragma once
#include "EMath.h"
#include "ERGBColor.h"
class Material;
struct HitRecord
{
	Elite::FPoint3 hitPoint;
	float intersection{}; //t-value in the slides
	Elite::FVector3 normal;
	const Material* pMaterial;
	Elite::FVector3 direction;
};