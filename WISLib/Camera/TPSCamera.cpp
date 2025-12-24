#include "pch.h"
#include "TPSCamera.h"
#include <algorithm>


WIS::TPSCamera::TPSCamera(DirectX::SimpleMath::Vector3 position) :
	CameraBase(position, WIS::CameraMode::MODE_FPSCAM),
	m_yaw(0.0f),
	m_pitch(0.0f)
{
}

WIS::TPSCamera::~TPSCamera()
{
}

void WIS::TPSCamera::UniqueInitialize()
{
	DirectX::Mouse::Get().SetMode(DirectX::Mouse::MODE_RELATIVE);
}

void WIS::TPSCamera::UniqueUpdate(DirectX::SimpleMath::Vector3& position, DirectX::SimpleMath::Vector3& target, DirectX::SimpleMath::Vector3& up, const DirectX::Mouse::State& state)
{

	float sensitivity = 0.002f; 
	m_yaw += state.x * sensitivity;
	m_pitch += state.y * sensitivity;

	float limit = DirectX::XM_PI / 2.0f - 0.01f;//90度ぐらい
	m_pitch = std::max(-limit, std::min(limit, m_pitch));


	DirectX::SimpleMath::Matrix rot = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(-m_yaw, m_pitch, 0.0f);

	DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitZ, rot);
	up = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, rot);


	//カメラの位置と向きを更新
	target = m_PlayerPos;
	target.y += 3;


	position = target - forward * 7.0f + up * 3.0f;

}

void WIS::TPSCamera::UniqueRander()
{
}

void WIS::TPSCamera::Uniquefinalize()
{
	DirectX::Mouse::Get().SetMode(DirectX::Mouse::MODE_ABSOLUTE);
}