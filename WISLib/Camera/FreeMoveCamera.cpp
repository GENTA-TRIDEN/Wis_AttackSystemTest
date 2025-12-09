#include "pch.h"
#include "FreeMoveCamera.h"

FreeMoveCamera::FreeMoveCamera(DirectX::SimpleMath::Vector3 position)
	: CameraBase(position, WIS::CameraMode::MODE_FREEMOVE),
	m_yaw(0.0f),   // ヨー角を0で初期化
	m_pitch(0.0f), // ピッチ角を0で初期化
	m_moveSpeed(0.5f) // 移動速度を初期化
{
}

FreeMoveCamera::~FreeMoveCamera()
{
}

void FreeMoveCamera::UniqueInitialize()
{
}

void FreeMoveCamera::UniqueUpdate(DirectX::SimpleMath::Vector3& position, DirectX::SimpleMath::Vector3& target, DirectX::SimpleMath::Vector3& up, const DirectX::Mouse::State& state)
{
	DirectX::Mouse::Get().SetMode(DirectX::Mouse::MODE_RELATIVE);

	
	auto key_state = DirectX::Keyboard::Get().GetState();
	auto mouse_state = DirectX::Mouse::Get().GetState();

	float sensitivity = 0.002f; 
	m_yaw += state.x * sensitivity;
	m_pitch += state.y * sensitivity;

	float limit = DirectX::XM_PI / 2.0f - 0.01f; // 約89.9度
	m_pitch = std::max(-limit, std::min(limit, m_pitch));

	DirectX::SimpleMath::Matrix rot = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(-m_yaw, m_pitch, 0.0f);

	DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitZ, rot);

	up = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, rot);

	if (key_state.W)
	{
		// Wキーが押されている場合、前方に移動
		position += forward * m_moveSpeed;
	}


	if (key_state.S)
	{
		// Sキーが押されている場合、後方に移動
		position -= forward * m_moveSpeed;
	}
	if (key_state.D)
	{
		// Aキーが押されている場合、左に移動
		position -= DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, rot) * m_moveSpeed;
	}
	if (key_state.A)
	{
		// Dキーが押されている場合、右に移動
		position += DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, rot) * m_moveSpeed;
	}









	target = position + forward;

}

void FreeMoveCamera::UniqueRander()
{
}

void FreeMoveCamera::Uniquefinalize()
{
}
