#include "pch.h"
#include "PlayerComponent.h"
#include "WISLib/GameObject/Component/TransformComponent.h"
#include "WISLib/GameObject/GameObject.h"
#include "WISLib/Camera/CameraManager.h"
#include "WISLib/Camera/TPSCamera.h"


WIS::PlayerComponent::PlayerComponent()
{
}

WIS::PlayerComponent::~PlayerComponent()
{
}

void WIS::PlayerComponent::OnStart(UserResources* userResources)
{
	// 初期化処理
	m_moveSpeed = 2.2f;

	(void)userResources; // C4100 警告対応
}

void WIS::PlayerComponent::OnPreUpdate(const float elapsedTime, UserResources* userResources)
{
	(void)elapsedTime; // C4100 警告対応
	(void)userResources; // C4100 警告対応
}

void WIS::PlayerComponent::OnUpdate(const float elapsedTime, UserResources* userResources)
{
	(void)userResources; // C4100 警告対応

	auto keyboard = DirectX::Keyboard::Get().GetState();

	m_moveDirection = DirectX::SimpleMath::Vector3::Zero;

	//WASDで移動
	if (keyboard.W) { m_moveDirection.z += 1.0f; }
	if (keyboard.S) { m_moveDirection.z-= 1.0f; }
	if (keyboard.A) { m_moveDirection.x += 1.0f; }
	if (keyboard.D) { m_moveDirection.x -= 1.0f; }


	auto cam = p_cameraManager->GetCamera();

	if (auto tpsCam = dynamic_cast<WIS::TPSCamera*>(cam)) {

		// プレイヤーの最新位置を取得
		auto transform = this->GetOwner()->GetComponent<WIS::TransformComponent>();

		// GetWorldPosition を推奨
		DirectX::SimpleMath::Vector3 currentPos = transform->GetPosition();

		// カメラにプレイヤーの最新位置を設定する (アプローチ 1)
		tpsCam->SetPlayerPosition(currentPos);

		float cameraYaw = tpsCam->GetYaw();

		// 修正: 移動方向の反転を解決するため、ヨー角の符号を反転させる
		float correctYaw = -cameraYaw;

		// プレイヤーの回転をカメラのヨー角に合わせる
		DirectX::SimpleMath::Quaternion targetRotation = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::Up, correctYaw);
		transform->SetRotation(targetRotation);

		if (m_moveDirection.LengthSquared() > 0) {
			m_moveDirection.Normalize();

			// ローカル移動ベクトルをワールド空間へ変換
			DirectX::SimpleMath::Matrix rotationMatrix = DirectX::SimpleMath::Matrix::CreateRotationY(correctYaw);
			DirectX::SimpleMath::Vector3 worldMoveVec = DirectX::SimpleMath::Vector3::TransformNormal(m_moveDirection, rotationMatrix);

			// Y方向を無視 (地面と平行な移動を保証)
			worldMoveVec.y = 0.0f;
			if (worldMoveVec.LengthSquared() > 0) {
				worldMoveVec.Normalize();
			}

			// 実際に移動
			transform->Translate(worldMoveVec * m_moveSpeed * elapsedTime);
		}
	}
}

void WIS::PlayerComponent::OnPostUpdate(const float elapsedTime, UserResources* userResources)
{
	(void)elapsedTime; // C4100 警告対応
	(void)userResources; // C4100 警告対応
}

void WIS::PlayerComponent::OnRender(UserResources* userResources, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{
	(void)userResources; // C4100 警告対応
	(void)view; // C4100 警告対応
	(void)proj; // C4100 警告対応
}

void WIS::PlayerComponent::OnRelease()
{
}