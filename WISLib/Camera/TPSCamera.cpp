#include "pch.h"
#include "TPSCamera.h"
#include <algorithm>


WIS::TPSCamera::TPSCamera(DirectX::SimpleMath::Vector3 position) :
	CameraBase(position, WIS::CameraMode::MODE_FPSCAM),
	m_yaw(0.0f),   // ヨー角を0で初期化
	m_pitch(0.0f)  // ピッチ角を0で初期化

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
	DirectX::Mouse::Get().SetMode(DirectX::Mouse::MODE_RELATIVE);
	// 1. マウスの移動量から回転角度を計算
	// -------------------------------------------------------------------
	float sensitivity = 0.002f; // マウス感度。この値を調整すると視点移動の速さが変わる
	m_yaw += state.x * sensitivity;
	m_pitch += state.y * sensitivity;

	// 2. 垂直方向（上下）の回転に制限をかける
	// -------------------------------------------------------------------
	// 視点が真上や真下を通り越してひっくり返らないようにする
	float limit = DirectX::XM_PI / 2.0f - 0.01f; // 約89.9度
	m_pitch = std::max(-limit, std::min(limit, m_pitch));

	// 3. ヨー角とピッチ角から最終的な回転行列を作成
	// -------------------------------------------------------------------
	DirectX::SimpleMath::Matrix rot = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(-m_yaw, m_pitch, 0.0f);

	// 4. 回転行列から「前方」と「上方」のベクトルを計算
	// -------------------------------------------------------------------
	// Z軸の正方向(0,0,1)を基準に、カメラが向いている「前方」を求める
	DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitZ, rot);

	// Y軸の正方向(0,1,0)を基準に、カメラの「上方」を求める
	// これを引数の up に設定
	up = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, rot);


	//カメラの位置と向きを更新
	target = position;
	position = target - forward * 3.0f + up * 2.0f;

}

void WIS::TPSCamera::UniqueRander()
{
}

void WIS::TPSCamera::Uniquefinalize()
{
	DirectX::Mouse::Get().SetMode(DirectX::Mouse::MODE_ABSOLUTE);
}
