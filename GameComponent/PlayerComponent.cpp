#include "pch.h"
#include "PlayerComponent.h"
#include "WISLib/GameObject/Component/TransformComponent.h"
#include "WISLib/GameObject/Component/RigidBodyComponent.h"
#include "WISLib/GameObject/Component/WeaponComponent.h"
#include "WISLib/GameObject/GameObject.h"
#include "WISLib/Camera/CameraManager.h"
#include "WISLib/Camera/TPSCamera.h"
#include "WISLib/GameObject/ObjectManager.h"
#include "WISLib/Common/Easing.h"
#include "UserResources.h"

#include <GamePad.h>

WIS::PlayerComponent::PlayerComponent()
    : m_moveSpeed(15.0f),
	m_isAttack(false),
	m_WeaponAttackTime(0.0f),
	m_hasCapturedRotation(false)
{ 
	m_initialWeaponRotation = DirectX::SimpleMath::Quaternion::Identity;
}


WIS::PlayerComponent::~PlayerComponent()
{
}

void WIS::PlayerComponent::OnStart(UserResources* userResources)
{
}

void WIS::PlayerComponent::OnPreUpdate(const float elapsedTime, UserResources* userResources)
{
}
void WIS::PlayerComponent::OnUpdate(const float elapsedTime, UserResources* userResources)
{
    // --- 移動処理 (変更なし) ---
    auto keyboard = DirectX::Keyboard::Get().GetState();
    auto mouse = DirectX::Mouse::Get().GetState();
	auto gamePad = DirectX::GamePad::Get().GetState(0);

    m_keyboardTracker.Update(keyboard);
	m_mouseTracker.Update(mouse);
	m_gamePadTracker.Update(gamePad);


    if (keyboard.W) { m_moveDirection.z += 1.0f; }
    if (keyboard.S) { m_moveDirection.z -= 1.0f; }
    if (keyboard.A) { m_moveDirection.x += 1.0f; }
    if (keyboard.D) { m_moveDirection.x -= 1.0f; }
    //gamePad
    if (m_moveDirection.Length() == 0.0f) {
        if (gamePad.thumbSticks.leftY > 0.1f) { m_moveDirection.z += gamePad.thumbSticks.leftY; }
        if (gamePad.thumbSticks.leftY < -0.1f) { m_moveDirection.z += gamePad.thumbSticks.leftY; }
        if (gamePad.thumbSticks.leftX < -0.1f) { m_moveDirection.x -= gamePad.thumbSticks.leftX; }
        if (gamePad.thumbSticks.leftX > 0.1f) { m_moveDirection.x -= gamePad.thumbSticks.leftX; }

    }

    auto cam = p_cameraManager->GetCamera();
    auto tpsCam = dynamic_cast<WIS::TPSCamera*>(cam);
    if (!tpsCam) return;

    auto transform = this->GetOwner()->GetComponent<WIS::TransformComponent>();
    auto rigidBody = this->GetOwner()->GetComponent<WIS::RigidBodyComponent>();

    DirectX::SimpleMath::Vector3 currentPos = transform->GetPosition();
    tpsCam->SetPlayerPosition(currentPos);

    float cameraYaw = tpsCam->GetYaw();
    float correctYaw = -cameraYaw;

    DirectX::SimpleMath::Quaternion targetRotation = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::Up, correctYaw);
    transform->SetRotation(targetRotation);

    if (rigidBody)
    {
        DirectX::SimpleMath::Vector3 currentVelocity = rigidBody->GetVelocity();
        DirectX::SimpleMath::Vector3 finalVelocity = DirectX::SimpleMath::Vector3::Zero;

        if (m_moveDirection.LengthSquared() > 0)
        {
            m_moveDirection.Normalize();
            DirectX::SimpleMath::Matrix rotationMatrix = DirectX::SimpleMath::Matrix::CreateRotationY(correctYaw);
            DirectX::SimpleMath::Vector3 worldMoveVec = DirectX::SimpleMath::Vector3::TransformNormal(m_moveDirection, rotationMatrix);

            worldMoveVec.y = 0.0f;
            if (worldMoveVec.LengthSquared() > 0) {
                worldMoveVec.Normalize();
            }
            finalVelocity = worldMoveVec * m_moveSpeed;
        }
        else
        {
            finalVelocity = DirectX::SimpleMath::Vector3::Zero;
        }
        finalVelocity.y = currentVelocity.y;
        rigidBody->ChangeVelocity(finalVelocity);

        if ((keyboard.Space ||gamePad.buttons.a)&& rigidBody->GetVelocity().y == 0.0f)
        {
            rigidBody->AddImpulse(DirectX::SimpleMath::Vector3{ 0,5,0 });
        }
    }

   
	// --- 攻撃処理 ---
    if (!m_weaponName.empty())
    {
        auto objectManager = userResources->GetObjectManager();
        auto weapon = objectManager->GetObject(m_weaponName);

        if (weapon)
        {
            auto weaponTransform = weapon->GetComponent<WIS::TransformComponent>();
            auto weaponComp = weapon->GetComponent<WIS::WeaponComponent>();

            if (!m_hasCapturedRotation)
            {
                m_initialWeaponRotation = weaponTransform->GetRotation();
                m_hasCapturedRotation = true;
            }

            float angleDeg = 0;
            float currentTime = m_WeaponAttackTime;
            float slantAngle;
            DirectX::SimpleMath::Quaternion swingRotHeavy;
            DirectX::SimpleMath::Quaternion swingRot;

            if (m_isAttack)
            {
                auto attackType = weaponComp->GetCurrentAttackType();

                switch (attackType)
                {
                case WIS::AttackType::NORMAL:
                    m_rotSpeedMag = weaponComp->GetBaseWeight() / weaponComp->GetWeight() * 2;

                    if (m_IsLeftToRight)m_WeaponAttackTime += m_rotSpeedMag * elapsedTime;
                    else m_WeaponAttackTime -= m_rotSpeedMag * elapsedTime;


                    if (currentTime > 1.0f) currentTime = 1.0f;


                    //0-180度の回転をイージングで計算
                    if (m_IsLeftToRight)angleDeg = WIS::Easing::OutCubic<float>(currentTime, 1.0f, -180.0f, 0.0f);
                    else angleDeg = WIS::Easing::OutCubic<float>(currentTime, 1.0f, -180.0f, 0.0f);
                    // 振り回転の作成
                    swingRot = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(
                        DirectX::XMConvertToRadians(angleDeg),
                        0.0f,
                        DirectX::XMConvertToRadians(10 * m_IsLeftToRight)
                    );



                    weaponTransform->SetRotation(m_initialWeaponRotation * swingRot);

                    if (m_WeaponAttackTime >= 1.0f)
                    {
                        m_isAttack = false;
                        m_WeaponAttackTime = 0.0f;
                        // 終わったら初期回転に戻す
                        weaponTransform->SetRotation(m_initialWeaponRotation);
                    }
                    //0いかになったら戻す
                    if (m_WeaponAttackTime < 0.0f)
                    {
                        m_isAttack = false;
                        m_WeaponAttackTime = 0.0f;
                        // 終わったら初期回転に戻す
                        weaponTransform->SetRotation(m_initialWeaponRotation);
                    }


                    break;
                case WIS::AttackType::HEAVY:
                    // 振りの速さを計算
                    m_rotSpeedMag = weaponComp->GetBaseWeight() / weaponComp->GetWeight() * 1.5f;
                    m_WeaponAttackTime += m_rotSpeedMag * elapsedTime;

                    currentTime = m_WeaponAttackTime;
                    if (currentTime > 1.0f) currentTime = 1.0f;

                    // --- 角度の計算 ---
                    // 縦方向（Pitch）: 真上(-90) から 斜め下(45) くらいまで
                    angleDeg = WIS::Easing::OutCubic<float>(currentTime, 1.0f, 135.0f, -90.0f);

                    // 横方向（Yaw）のオフセット: これを加えると斜めになる
                    // 0度なら真っ直ぐ、45度なら右斜めから、-45度なら左斜めからになります
                    slantAngle = DirectX::XMConvertToRadians(45.0f);

                    // 縦振り（Pitch）に横（Yaw）を組み合わせて斜め回転を作成
                    swingRotHeavy = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(
                        slantAngle,                           // Yaw: 振りの方向を左右に傾ける
                        DirectX::XMConvertToRadians(angleDeg),// Pitch: 振り下ろす動作
                        slantAngle * 0.5f                     // Roll: 刀身のねじり（適宜調整）
                    );

                    // 初期回転に対して斜め振りを適用
                    weaponTransform->SetRotation(m_initialWeaponRotation * swingRotHeavy);

                    // 終了判定
                    if (m_WeaponAttackTime >= 1.0f)
                    {
                        m_isAttack = false;
                        m_WeaponAttackTime = 0.0f;
                        weaponTransform->SetRotation(m_initialWeaponRotation);
                    }
                    break;

                default:
                    break;
                }


            }
            else {
                if (m_initialWeaponRotation == DirectX::SimpleMath::Quaternion::Identity) {

                    m_initialWeaponRotation = DirectX::SimpleMath::Quaternion::Identity;
                    weaponTransform->SetRotation(m_initialWeaponRotation);
                }
                
            }


            //左Clickをする
            if (mouse.leftButton||gamePad.IsRightTriggerPressed())
            {
                //攻撃中
                if (m_isAttack) {

                    if (m_IsLeftToRight && m_WeaponAttackTime >= 0.3f) m_IsLeftToRight = false;
                    m_isAttack = true;
                }
                //攻撃中でない場合
                else {
                    m_initialWeaponRotation=
                        DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(

                            0.0f,
                            0.0f,
                            0.0f
                        );

                    m_isAttack = true;
                    m_IsLeftToRight = true;
                    m_WeaponAttackTime = 0.0f;
                }

                if (weaponComp)
                {
                    weaponComp->TryAttack();
                }

            }

            //チャージ攻撃
            if (mouse.rightButton || gamePad.IsLeftTriggerPressed())
            {
                m_isChargeAttack = true;

                //振り上げている状態
 

                m_initialWeaponRotation=
					DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(

                        0.0f,
                        0.0f,
                        DirectX::XMConvertToRadians(50.0f)
					);
				weaponTransform->SetRotation(m_initialWeaponRotation);

                if (weaponComp)
                {
                    weaponComp->ChargeAttack(elapsedTime);
                }
            }

			//チャージ攻撃解除

            if (m_isChargeAttack && (!mouse.rightButton && !gamePad.IsLeftTriggerPressed()))
            {
                m_isChargeAttack = false;

                if (!m_isAttack) {
                    m_isAttack = true;
                    m_WeaponAttackTime = 0.0f;
                }


				if (weaponComp)
				{
					weaponComp->TryAttackHeavy();

				}
            }


        }
    }
}

void WIS::PlayerComponent::OnPostUpdate(const float elapsedTime, UserResources* userResources)
{

    m_moveDirection = DirectX::SimpleMath::Vector3::Zero;
}

void WIS::PlayerComponent::OnRender(UserResources* userResources, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{

}

void WIS::PlayerComponent::OnRelease()
{

}

void WIS::PlayerComponent::OnMessegeAccepted(Message::MessageID messageID, const std::any& value)
{
    using namespace DirectX::SimpleMath;
    UNREFERENCED_PARAMETER(value);

    switch (messageID)
    {
    case Message::PLAYER_MOVE_FORWARD:
		m_moveDirection.z += 1.0f;
        break;
    case Message::PLAYER_MOVE_BACKWARD:
		m_moveDirection.z -= 1.0f;
        break;
    case Message::PLAYER_MOVE_LEFT:
		m_moveDirection.x += 1.0f;
        break;
    case Message::PLAYER_MOVE_RIGHT:
		m_moveDirection.x -= 1.0f;
        break;

    }

}
