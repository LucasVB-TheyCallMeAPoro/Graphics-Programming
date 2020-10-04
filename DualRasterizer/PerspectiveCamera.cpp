#include "pch.h"
#include "PerspectiveCamera.h"
#include <cmath>
using namespace Elite;
PerspectiveCamera::PerspectiveCamera()
	:m_Position{ 0,0,0 }
	, m_FOV{ 0 }
	, m_AspectRatio{ 0 }
	, m_WorldUp{ 0,1,0 }
	, m_Pitch{ 0.f }
	, m_Yaw{ 0.f }
	,m_Near{0.f}
	,m_Far{0.f}
{
}
PerspectiveCamera::PerspectiveCamera(const Elite::FPoint3& Pos, float FOV, float AspectR, float near, float far)
	:m_Position{Pos}
	,m_FOV{FOV * static_cast<float>(E_PI/180)}
	,m_AspectRatio{AspectR}
	,m_WorldUp{0,1,0}
	,m_Pitch{0.f}
	,m_Yaw{0.f}
	,m_Near{near}
	,m_Far{far}
{
}
void PerspectiveCamera::Update()
{
	m_CameraControl.Update(*this);
}

Elite::FMatrix4 PerspectiveCamera::GetViewMatrix() const
{
	FVector3 direction{ GetDirection() };
	FVector3 right = GetNormalized(Cross(m_WorldUp, direction));
	FVector3 up = Cross(direction, right);
	return Elite::Inverse(Elite::FMatrix4{ FVector4{right},FVector4{up},FVector4{direction},FVector4{m_Position.x, m_Position.y, -m_Position.z,1} });
}

Elite::FMatrix4 PerspectiveCamera::GetProjectionMatrix() const
{
	return Elite::FMatrix4{ Elite::FVector4{1 / (m_AspectRatio* m_FOV),0,0,0},Elite::FVector4{0,1 / m_FOV,0,0},Elite::FVector4{0,0,m_Far / (m_Far - m_Near),1},Elite::FVector4{0,0,(-(m_Far * m_Near) / (m_Far - m_Near)),0} };;
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
	direction.x = -sin(-m_Yaw) * cos(m_Pitch);
	direction.y = sin(m_Pitch);
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

float PerspectiveCamera::GetFOV() const
{
	return m_FOV;
}

float PerspectiveCamera::GetFar() const
{
	return m_Far;
}

float PerspectiveCamera::GetNear() const
{
	return m_Near;
}

