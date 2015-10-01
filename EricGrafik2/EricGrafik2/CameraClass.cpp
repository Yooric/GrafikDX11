#include "CameraClass.h"



CameraClass::CameraClass()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;
}

CameraClass::CameraClass(const CameraClass &)
{
}


CameraClass::~CameraClass()
{
}

void CameraClass::SetPosition(float p_X, float p_Y, float p_Z)
{
	m_positionX = p_X;
	m_positionY = p_Y;
	m_positionZ = p_Z;
}

void CameraClass::SetRotation(float p_X, float p_Y, float p_Z)
{
	m_rotationX = p_X;
	m_rotationY = p_Y;
	m_rotationZ = p_Z;
}

XMFLOAT3 CameraClass::GetPosition()
{
	return XMFLOAT3(m_positionX, m_positionY, m_positionZ);
}

XMFLOAT3 CameraClass::GetRotation()
{
	return XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ);
}

void CameraClass::Render()
{
	XMFLOAT3 t_Up, t_Position, t_LookAt;
	XMVECTOR t_UpVector, t_PositionVector, t_LookAtVector;
	float t_Yaw, t_Pitch, t_Roll;
	XMMATRIX t_RotationMatrix;

	//fixa uppvektorn
	t_Up.x = 0.0f;
	t_Up.y = 1.0f;
	t_Up.z = 0.0f;
	//Lägger in det sen i en XMFLoat, borde man inte kunna göra det utan att xmloada? aja kanske lättare
	t_UpVector = XMLoadFloat3(&t_Up);
	// Setup the position of the camera in the world
	t_Position.x = m_positionX;
	t_Position.y = m_positionY;
	t_Position.z = m_positionZ;

	// Load it into a XMVECTOR structure. samma här dp
	t_PositionVector = XMLoadFloat3(&t_Position);

	t_LookAt.x = 0.0f;
	t_LookAt.y = 0.0f;
	t_LookAt.z = 1.0f;

	// Load it into a XMVECTOR structure. again
	t_LookAtVector = XMLoadFloat3(&t_LookAt);

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	t_Pitch = m_rotationX * 0.0174532925f;/////
	t_Yaw = m_rotationY * 0.0174532925f;
	t_Roll = m_rotationZ * 0.0174532925f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	t_RotationMatrix = XMMatrixRotationRollPitchYaw(t_Pitch, t_Yaw, t_Roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	t_LookAtVector = XMVector3TransformCoord(t_LookAtVector, t_RotationMatrix);
	t_UpVector = XMVector3TransformCoord(t_UpVector, t_RotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	t_LookAtVector = XMVectorAdd(t_PositionVector, t_LookAtVector);

	// Finally create the view matrix from the three updated vectors.
	m_viewMatrix = XMMatrixLookAtLH(t_PositionVector, t_LookAtVector, t_UpVector);

}
//Här kommer det behövas byta ut till XMFLOAT4x4 säkert
void CameraClass::GetViewMatrix(XMMATRIX& p_ViewMatrix)
{
	p_ViewMatrix = m_viewMatrix;
}
