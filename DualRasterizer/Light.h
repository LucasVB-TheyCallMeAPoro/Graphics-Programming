#pragma once
#include "EMath.h"
#include "ERGBColor.h"

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
		Directional
	};

	//Light(const PointLightData& pos, const RGBColor& color, float lightIntensity);
	Light(const DirectionalLightData& dir, const RGBColor& color, float lightIntensity);
	Light::Type GetType() const;
	void ToggleLight();
	Elite::FVector3 GetDirection() const;
	Elite::RGBColor GetColor() const;
	float GetIntensity() const;
	
private:

	Type m_Type;
	RGBColor m_Color;
	float m_Intensity;

	union
	{
		//PointLightData m_PointLightData;
		DirectionalLightData m_DirectionalLightData;
	};

	bool m_Enabled;
};

