#pragma once
#include "EMath.h"
#include "CameraControl.h"
struct Ray;
class PerspectiveCamera
{
public:
	PerspectiveCamera();
	PerspectiveCamera(const Elite::FPoint3& Pos, float FOV, float AspectR);
	void CalcRay(Ray& ray, int c, int r, int width, int height) const;
	Elite::FMatrix4  CalculateMatrix() const;
	void Update();

	const Elite::FPoint3& GetPosition() const;
	void SetPosition(const Elite::FPoint3& pos);
	Elite::FVector3 GetDirection() const;
	float GetYaw() const;
	void SetYaw(float yaw);
	float GetPitch() const;
	void SetPitch(float pitch);
private:
	float m_AspectRatio;
	float m_FOV;
	float m_Yaw;
	float m_Pitch;
	Elite::FPoint3 m_Position;
	//Elite::FVector3 m_Direction;
	Elite::FVector3 m_WorldUp;
	CameraControl m_CameraControl;
};

