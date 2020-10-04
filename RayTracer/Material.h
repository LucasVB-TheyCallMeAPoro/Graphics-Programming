#pragma once
#include "EMath.h"
struct HitRecord;
class Material
{
public:
	Material() = default;
	virtual Elite::RGBColor Shade(const HitRecord& hit, const Elite::FVector3& incLight, const Elite::FVector3& outLight) const = 0;
};

