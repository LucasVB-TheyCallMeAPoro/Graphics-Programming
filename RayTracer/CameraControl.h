#pragma once
class PerspectiveCamera;
class CameraControl 
{
public:

	CameraControl();
	void Update(PerspectiveCamera& camera);
private:

	float m_PrevTick;
	int m_PrevXpos;
	int m_PrevYpos;
	
	static const float m_RotSensitivity;
	static const float m_MovSensitivity;
	static const float m_MoveSpeed; //Units per tick
};

