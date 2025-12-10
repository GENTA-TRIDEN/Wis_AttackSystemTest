#include "pch.h"
#include "PlayerComponent.h"
#include "WISLib/GameObject/Component/TransformComponent.h"
#include "WISLib/GameObject/GameObject.h"


WIS::PlayerComponent::PlayerComponent()
{
}

WIS::PlayerComponent::~PlayerComponent()
{
}

void WIS::PlayerComponent::OnStart(UserResources* userResources)
{
	// ‰Šú‰»ˆ—


}

void WIS::PlayerComponent::OnPreUpdate(const float elapsedTime, UserResources* userResources)
{
}

void WIS::PlayerComponent::OnUpdate(const float elapsedTime, UserResources* userResources)
{
	auto keyboard = DirectX::Keyboard::Get().GetState();

	//WASD‚ÅˆÚ“®
	if (keyboard.W) { m_moveDirection.z += 1; }
	if (keyboard.S) { m_moveDirection.z -= 1; }
	if (keyboard.A) { m_moveDirection.x += 1; }
	if (keyboard.D) { m_moveDirection.x -= 1; }

	if (m_moveDirection.Length() > 0) {
		m_moveDirection.Normalize();

		auto transform = GetOwner()->GetComponent<WIS::TransformComponent>();
		DirectX::SimpleMath::Vector3 forward = transform->GetForward();
		DirectX::SimpleMath::Vector3 right = transform->GetRight();
		DirectX::SimpleMath::Vector3 movement = (forward * m_moveDirection.z + right * m_moveDirection.x) * m_moveSpeed * elapsedTime;
		DirectX::SimpleMath::Vector3 newPosition = transform->GetPosition() + movement;
		transform->SetPosition(newPosition);
		m_moveDirection = DirectX::SimpleMath::Vector3::Zero;


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
