#include "pch.h"
#include "CameraControl.h"
#include "SDL.h"
#include "PerspectiveCamera.h"
#include <cmath>
const float CameraControl::m_MoveSpeed{ 0.01f };
const float CameraControl::m_RotSensitivity{ 0.01f };
const float CameraControl::m_MovSensitivity{ 1.f };
CameraControl::CameraControl()
	:m_PrevTick{static_cast<float>(SDL_GetTicks())}
{
	SDL_GetRelativeMouseState(&m_PrevXpos, &m_PrevYpos);
}

void CameraControl::Update(PerspectiveCamera& camera)
{
	int x, y;
	const Uint8* keystate{ SDL_GetKeyboardState(nullptr) };
	const Uint32 mousestate{ SDL_GetRelativeMouseState(&x,&y) };
	
	float curTick{ static_cast<float>(SDL_GetTicks()) };
	float deltatime = curTick - m_PrevTick;
	 m_PrevTick = curTick;
	Elite::FVector4 movement{};
	if (keystate[SDL_SCANCODE_W])
	{
		movement.z -= m_MoveSpeed * deltatime;
	}
	if (keystate[SDL_SCANCODE_S])
	{
		movement.z += m_MoveSpeed * deltatime;
	}
	if (keystate[SDL_SCANCODE_A])
	{
		movement.x -= m_MoveSpeed * deltatime;
	}
	if (keystate[SDL_SCANCODE_D])
	{
		movement.x += m_MoveSpeed * deltatime;
	}
	if ((bool)(SDL_BUTTON_LMASK & mousestate) && (bool)(SDL_BUTTON_RMASK & mousestate))
	{
		movement.y -= y * m_MovSensitivity;
		movement.x += x * m_MovSensitivity;
	}
	else if (SDL_BUTTON_RMASK & mousestate)
	{
		camera.SetYaw(std::fmod(m_RotSensitivity * x + camera.GetYaw(), 2 * E_PI));
		float nPitch{ -m_RotSensitivity * y + camera.GetPitch() };
		if (nPitch > 1.5f)
		{
			nPitch = 1.5f;
		}
		else if (nPitch < -1.5f)
		{
			nPitch = -1.5f;
		}
		camera.SetPitch(nPitch);
	}
	else if (SDL_BUTTON_LMASK & mousestate)
	{
		camera.SetYaw(std::fmod(m_RotSensitivity * x + camera.GetYaw(), 2 * E_PI));
		movement.z += y * m_MovSensitivity;
	}
	
	movement = camera.CalculateMatrix()* movement;
	camera.SetPosition(camera.GetPosition() + static_cast<Elite::FVector3>(movement));

	
}
