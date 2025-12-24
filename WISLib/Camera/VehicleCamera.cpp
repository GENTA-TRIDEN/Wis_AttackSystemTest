#include "pch.h"
#include "VehicleCamera.h"
#include "WISLib/GameObject/Component/TransformComponent.h" // TransformComponentのヘッダをインクルード
#include <algorithm>

// コンストラクタ
WIS::VehicleCamera::VehicleCamera(DirectX::SimpleMath::Vector3 Pos)
	: CameraBase(Pos, WIS::CameraMode::MODE_VEHICLE), // カメラタイプを正しく設定
	m_yaw(0.0f),
	m_pitch(0.0f)
{
	// m_pTargetTransform は自動的に空の状態で初期化されます
}

// デストラクタ
WIS::VehicleCamera::~VehicleCamera()
{
}

// ターゲット設定関数
void WIS::VehicleCamera::SetTarget(std::shared_ptr<WIS::TransformComponent> pTransform)
{
	// 受け取ったshared_ptrをweak_ptrに代入して監視対象にする
	m_pTargetTransform = pTransform;
}

// 初期化
void WIS::VehicleCamera::UniqueInitialize()
{
	// カメラが有効になったらマウスカーソルを消して相対モードにする
	DirectX::Mouse::Get().SetMode(DirectX::Mouse::MODE_RELATIVE);
}

// 毎フレームの更新処理
void WIS::VehicleCamera::UniqueUpdate(DirectX::SimpleMath::Vector3& position, DirectX::SimpleMath::Vector3& target, DirectX::SimpleMath::Vector3& up, const DirectX::Mouse::State& state)
{
	//  weak_ptrの安全な使い方 
	// lock()を試みて、ターゲットがまだ存在するかどうかを確認する
	std::shared_ptr<WIS::TransformComponent> lockedTarget = m_pTargetTransform.lock();

	//  もしlock()に成功したら（ターゲットがまだ存在したら）、カメラの処理を続ける
	if (lockedTarget)
	{
		// ターゲットの現在の位置を取得
		target = lockedTarget->GetPosition();


		using namespace DirectX::SimpleMath;

		// マウスの移動量からカメラの回転角度を計算
		float sensitivity = 0.002f; // マウス感度
		m_yaw += state.x * sensitivity;
		m_pitch += state.y * sensitivity;

		// ヨー角（水平回転）を0から2πの範囲に正規化
		m_yaw = std::fmod(m_yaw, DirectX::XM_2PI);

		// ピッチ角（垂直回転）が真上や真下に行き過ぎないように制限する
		float limit = DirectX::XM_PI / 2.0f - 0.01f; // 約89.9度
		m_pitch = std::max(-limit, std::min(limit, m_pitch));

		// ターゲットからの相対的なカメラの位置（オフセット）
		Vector3 cameraOffset = Vector3(0.0f, 5.0f, -3.0f);

		// 回転を適用して、最終的なカメラの位置を計算する
		position = target + Vector3::Transform(cameraOffset, Matrix::CreateFromYawPitchRoll(-m_yaw, m_pitch, 0.0f));
	}
	// 3. もしlock()に失敗したら（ターゲットが破棄されていたら）、何もしない
	else
	{
	}
}

// 描画処理（今回は特に何もしない）
void WIS::VehicleCamera::UniqueRander()
{
}

// 終了処理
void WIS::VehicleCamera::Uniquefinalize()
{
	// カメラが無効になったらマウスカーソルを通常モードに戻す
	DirectX::Mouse::Get().SetMode(DirectX::Mouse::MODE_ABSOLUTE);

	// weak_ptrをリセットして監視を解除
	m_pTargetTransform.reset();
}