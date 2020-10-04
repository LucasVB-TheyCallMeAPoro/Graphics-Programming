#pragma once
#include "EMath.h"
#include "CameraControl.h"

#undef near
#undef far

struct Ray;
class PerspectiveCamera
{
public:
	PerspectiveCamera();
	PerspectiveCamera(const Elite::FPoint3& Pos, float FOV, float AspectR, float near, float far);
	Elite::FMatrix4  GetViewMatrix() const;
	Elite::FMatrix4 GetProjectionMatrix() const;
	void Update();

	const Elite::FPoint3& GetPosition() const;
	void SetPosition(const Elite::FPoint3& pos);
	Elite::FVector3 GetDirection() const;
	float GetYaw() const;
	void SetYaw(float yaw);
	float GetPitch() const;
	void SetPitch(float pitch);
	float GetFOV() const;
	float GetFar() const;
	float GetNear() const;
private:
	float m_AspectRatio;
	float m_FOV;
	float m_Yaw;
	float m_Pitch;
	float m_Near;
	float m_Far;

	Elite::FPoint3 m_Position;
	Elite::FVector3 m_WorldUp;
	CameraControl m_CameraControl;
};

