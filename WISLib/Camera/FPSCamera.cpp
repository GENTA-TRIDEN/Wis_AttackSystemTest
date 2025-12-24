#include "pch.h"
#include "FPSCamera.h"
#include <algorithm>


WIS::FPSCamera::FPSCamera(DirectX::SimpleMath::Vector3 position):
	CameraBase(position,WIS::CameraMode::MODE_FPSCAM),
	m_yaw(0.0f),   // ヨー角を0で初期化
	m_pitch(0.0f)  // ピッチ角を0で初期化

{
}

WIS::FPSCamera::~FPSCamera()
{
}

void WIS::FPSCamera::UniqueInitialize()
{
	DirectX::Mouse::Get().SetMode(DirectX::Mouse::MODE_RELATIVE);
}

void WIS::FPSCamera::UniqueUpdate(DirectX::SimpleMath::Vector3& position, DirectX::SimpleMath::Vector3& target, DirectX::SimpleMath::Vector3& up, const DirectX::Mouse::State& state)
{
	DirectX::Mouse::Get().SetMode(DirectX::Mouse::MODE_RELATIVE);
	float sensitivity = 0.002f; // マウス感度。この値を調整すると視点移動の速さが変わる
	m_yaw += state.x * sensitivity;
	m_pitch += state.y * sensitivity;


	float limit = DirectX::XM_PI / 2.0f - 0.01f; // 約89.9度
	m_pitch = std::max(-limit, std::min(limit, m_pitch));


	DirectX::SimpleMath::Matrix rot = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(-m_yaw, m_pitch, 0.0f);


	// Z軸の正方向(0,0,1)を基準に、カメラが向いている「前方」を求める
	DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitZ, rot);


	// これを引数の up に設定
	up = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, rot);
	

	//カメラの位置と向きを更新
	target = position + forward;


}

void WIS::FPSCamera::UniqueRander()
{
}

void WIS::FPSCamera::Uniquefinalize()
{
	DirectX::Mouse::Get().SetMode(DirectX::Mouse::MODE_ABSOLUTE);
}
