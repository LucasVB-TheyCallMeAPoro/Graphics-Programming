#pragma once
#include "EMath.h"
#include "ERGBColor.h"
#include "HitRecord.h"

using namespace Elite;

struct PointLightData
{
	FPoint3 position;

	explicit PointLightData(const FPoint3& pos) : position{ pos } {}
};

struct DirectionalLightData
{
	FVector3 direction;
	explicit DirectionalLightData(const FVector3& dir) : direction{ dir } {}
};
class Light
{
public:
	enum class Type
	{
		Point,
		Directional
	};
	enum class Toggle
	{
		Irrandiance = 0,
		BRDF = 1,
		All = 2,
	};

	Light(const PointLightData& pos, const RGBColor& color, float lightIntensity);
	Light(const DirectionalLightData& dir, const RGBColor& color, float lightIntensity);
	RGBColor CalculatedBiradiance(const HitRecord& hit) const;
	Light::Type GetType() const;
	void ToggleLight();
	void ToggleCalculation();
	Elite::FVector3 GetDirection(const HitRecord& hit) const;
	
private:

	Type m_Type;
	Toggle m_Toggle;
	RGBColor m_Color;
	float m_Intensity;

	union
	{
		PointLightData m_PointLightData;
		DirectionalLightData m_DirectionalLightData;
	};

	bool m_Enabled;
};

