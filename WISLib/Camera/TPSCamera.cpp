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
	DirectX::Mouse::Get().SetMode(DirectX::Mouse::MODE_RELATIVE);
	// 1. マウスの移動量から回転角度を計算
	// -------------------------------------------------------------------
	float sensitivity = 0.002f; // マウス感度。この値を調整すると視点移動の速さが変わる
	m_yaw += state.x * sensitivity;
	m_pitch += state.y * sensitivity;

	// 2. 垂直方向（上下）の回転に制限をかける
	// -------------------------------------------------------------------
	float limit = DirectX::XM_PI / 2.0f - 0.01f; // 約89.9度
	m_pitch = std::max(-limit, std::min(limit, m_pitch));

	// 3. ヨー角とピッチ角から最終的な回転行列を作成
	// -------------------------------------------------------------------
	// プレイヤー移動との一貫性を保つため -m_yaw を使用
	DirectX::SimpleMath::Matrix rot = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(-m_yaw, m_pitch, 0.0f);

	// 4. 回転行列から「前方」と「上方」のベクトルを計算
	// -------------------------------------------------------------------
	DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitZ, rot);
	up = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, rot);


	//カメラの位置と向きを更新
	//追従ロジックの修正: メンバー変数から引数の target に位置をコピー
	target = m_PlayerPos;

	// カメラ位置 = ターゲット位置 - 前方ベクトル * 距離 + 上方ベクトル * 高さ
	position = target - forward * 7.0f + up * 3.0f;

}

void WIS::TPSCamera::UniqueRander()
{
}

void WIS::TPSCamera::Uniquefinalize()
{
	DirectX::Mouse::Get().SetMode(DirectX::Mouse::MODE_ABSOLUTE);
}