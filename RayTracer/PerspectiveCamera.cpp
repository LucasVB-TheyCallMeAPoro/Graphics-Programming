#include "pch.h"
#include "PerspectiveCamera.h"
#include "Ray.h"
#include <cmath>
using namespace Elite;
PerspectiveCamera::PerspectiveCamera()
	:m_Position{ 0,0,0 }
	, m_FOV{ 0 }
	, m_AspectRatio{ 0 }
	, m_WorldUp{ 0,1,0 }
	, m_Pitch{ 0.f }
	, m_Yaw{ 0.f }
{
}
PerspectiveCamera::PerspectiveCamera(const Elite::FPoint3& Pos, float FOV, float AspectR)
	:m_Position{Pos}
	,m_FOV{FOV}
	,m_AspectRatio{AspectR}
	,m_WorldUp{0,1,0}
	,m_Pitch{0.f}
	,m_Yaw{0.f}
{
}

void PerspectiveCamera::CalcRay(Ray& ray, int c, int r, int width, int height) const
{
	ray.direction =  GetNormalized(FVector3{ (float)(2 * ((c + 0.5) / width) - 1) * m_AspectRatio * m_FOV,(float)(1 - 2 * ((r + 0.5) / height)) * m_FOV, -1 });
	ray.origin = m_Position;
}

void PerspectiveCamera::Update()
{
	m_CameraControl.Update(*this);
}

Elite::FMatrix4 PerspectiveCamera::CalculateMatrix() const
{
	FVector3 direction{ GetDirection() };
	FVector3 right = GetNormalized(Cross(m_WorldUp, direction));
	FVector3 up = Cross(direction, right);
	return Elite::FMatrix4{ FVector4{right},FVector4{up},FVector4{direction},FVector4{m_Position} };
}

const Elite::FPoint3& PerspectiveCamera::GetPosition() const
{
	return	m_Position;
}

void PerspectiveCamera::SetPosition(const Elite::FPoint3& pos)
{
	m_Position = pos;
}

Elite::FVector3 PerspectiveCamera::GetDirection() const
{
	FVector3 direction{};
	direction.x = sin(-m_Yaw) * cos(m_Pitch);
	direction.y = -sin(m_Pitch);
	direction.z = cos(-m_Yaw) * cos(m_Pitch);
	return direction;
}

float PerspectiveCamera::GetYaw() const
{
	return m_Yaw;
}

void PerspectiveCamera::SetYaw(float yaw)
{
	m_Yaw = yaw;
}

float PerspectiveCamera::GetPitch() const
{
	return m_Pitch;
}

void PerspectiveCamera::SetPitch(float pitch)
{
	m_Pitch = pitch;
}

