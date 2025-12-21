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
    // --- ˆÚ“®ˆ— (•ÏX‚È‚µ) ---
    auto keyboard = DirectX::Keyboard::Get().GetState();
    auto mouse = DirectX::Mouse::Get().GetState();

    m_moveDirection = DirectX::SimpleMath::Vector3::Zero;

    if (keyboard.W) { m_moveDirection.z += 1.0f; }
    if (keyboard.S) { m_moveDirection.z -= 1.0f; }
    if (keyboard.A) { m_moveDirection.x += 1.0f; }
    if (keyboard.D) { m_moveDirection.x -= 1.0f; }

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

        if (keyboard.Space && rigidBody->GetVelocity().y == 0.0f)
        {
            rigidBody->AddImpulse(DirectX::SimpleMath::Vector3{ 0,5,0 });
        }
    }

    // --- •ŠíUŒ‚ˆ— (C³”Å) ---
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

            if (m_isAttack)
            {
                m_WeaponAttackTime += elapsedTime;

                float currentTime = m_WeaponAttackTime;
                if (currentTime > 1.0f) currentTime = 1.0f;

                // BackŒn‚ğg‚¤ê‡: ‘æ3ˆø”‚É”½“®—Ê(—á‚¦‚Î1.7f)‚ğw’è‚µA‘æ4ˆø”‚É–Ú•WŠp“x(-180.0f)‚ğ“ü‚ê‚é
                float angleDeg = WIS::Easing::OutCubic<float>(currentTime, 1.0f, -180.0f, 0.0f);

                // U‚è‰ñ“]‚Ìì¬
                DirectX::SimpleMath::Quaternion swingRot = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(
                    DirectX::XMConvertToRadians(angleDeg),
                    
                    0.0f,
                    DirectX::XMConvertToRadians(10)
                );

                // ‚±‚ê‚É‚æ‚èA‚à‚Æ‚à‚Æ‚Ì‚¿•û‚ğˆÛ‚µ‚½‚Ü‚ÜU‚é‚±‚Æ‚ª‚Å‚«‚Ü‚·
                weaponTransform->SetRotation(m_initialWeaponRotation * swingRot);

                if (m_WeaponAttackTime >= 1.0f)
                {
                    m_isAttack = false;
                    m_WeaponAttackTime = 0.0f;
                    // I‚í‚Á‚½‚ç‰Šú‰ñ“]‚É–ß‚·
                    weaponTransform->SetRotation(m_initialWeaponRotation);
                }
            }

            if (mouse.leftButton && !m_isAttack)
            {
                m_isAttack = true;
                m_WeaponAttackTime = 0.0f;
                if (weaponComp)
                {
                    weaponComp->TryAttack();
                }
            }
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