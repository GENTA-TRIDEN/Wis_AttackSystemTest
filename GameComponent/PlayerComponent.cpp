#include "pch.h"
#include "PlayerComponent.h"
#include "WISLib/GameObject/Component/TransformComponent.h"
#include "WISLib/GameObject/Component/RigidBodyComponent.h"
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

}

void WIS::PlayerComponent::OnPreUpdate(const float elapsedTime, UserResources* userResources)
{
}
void WIS::PlayerComponent::OnUpdate(const float elapsedTime, UserResources* userResources)
{
    // --- 1. 入力処理 ---
    auto keyboard = DirectX::Keyboard::Get().GetState();
    m_moveDirection = DirectX::SimpleMath::Vector3::Zero;

    if (keyboard.W) { m_moveDirection.z += 1.0f; }
    if (keyboard.S) { m_moveDirection.z -= 1.0f; }
    if (keyboard.A) { m_moveDirection.x += 1.0f; }
    if (keyboard.D) { m_moveDirection.x -= 1.0f; }

    // --- 2. カメラ・座標関連の準備 ---
    auto cam = p_cameraManager->GetCamera();
    auto tpsCam = dynamic_cast<WIS::TPSCamera*>(cam);
    if (!tpsCam) return;

    // コンポーネント取得
    auto transform = this->GetOwner()->GetComponent<WIS::TransformComponent>();
    auto rigidBody = this->GetOwner()->GetComponent<WIS::RigidBodyComponent>(); // RigidBodyを取得

    // カメラ位置更新
    DirectX::SimpleMath::Vector3 currentPos = transform->GetPosition();
    tpsCam->SetPlayerPosition(currentPos);

    // カメラの向き取得
    float cameraYaw = tpsCam->GetYaw();
    float correctYaw = -cameraYaw;

    // キャラクターの向きをカメラに合わせる（ここは仕様に合わせて調整してください）
    DirectX::SimpleMath::Quaternion targetRotation = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::Up, correctYaw);
    transform->SetRotation(targetRotation);

    // --- 3. 物理移動計算ロジック ---

    // RigidBodyがないと動かせないのでチェック
    if (rigidBody)
    {
        //いまの速度を取得重力などを調べるため
        DirectX::SimpleMath::Vector3 currentVelocity = rigidBody->GetVelocity();

        //次のフレームの速度
        DirectX::SimpleMath::Vector3 finalVelocity = DirectX::SimpleMath::Vector3::Zero;

        //入力がある場合
        if (m_moveDirection.LengthSquared() > 0)
        {
            // 移動方向 × スピード

            //正規化
            m_moveDirection.Normalize();


            // カメラの向きに合わせてベクトルを回転
            DirectX::SimpleMath::Matrix rotationMatrix = DirectX::SimpleMath::Matrix::CreateRotationY(correctYaw);
            DirectX::SimpleMath::Vector3 worldMoveVec = DirectX::SimpleMath::Vector3::TransformNormal(m_moveDirection, rotationMatrix);

            // Y軸の入力成分はなくし水平移動のみにする
            worldMoveVec.y = 0.0f;
            if (worldMoveVec.LengthSquared() > 0) {
                worldMoveVec.Normalize();
            }

            // スピードを掛ける
            finalVelocity = worldMoveVec * m_moveSpeed;
        }
        else
        {
            // 入力がない場合：水平速度はゼロ
            finalVelocity = DirectX::SimpleMath::Vector3::Zero;
        }


        // C. 【重要】重力成分の維持
        // ここで Y を 0 や固定値にすると、空中に浮いたりゆっくり落ちてしまいます。
        // 「今の落下速度」をそのまま採用することで、重力だけは物理演算に任せます。
        finalVelocity.y = currentVelocity.y;



        // D. 最終的な速度を適用
        rigidBody->ChangeVelocity(finalVelocity);

        if (keyboard.Space && rigidBody->GetVelocity().y == 0.0f)
        {
            rigidBody->AddImpulse(DirectX::SimpleMath::Vector3{ 5,5,0 });
        }

    }
}

void WIS::PlayerComponent::OnPostUpdate(const float elapsedTime, UserResources* userResources)
{
}

void WIS::PlayerComponent::OnRender(UserResources* userResources, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{
}

void WIS::PlayerComponent::OnRelease()
{
}