#include "pch.h"
#include "CharacterComponent.h"
#include "CollisionComponent.h"

#include "WISLib/GameObject/GameObject.h"

WIS::CharacterComponent::CharacterComponent()
{
}

WIS::CharacterComponent::~CharacterComponent()
{
}

void WIS::CharacterComponent::OnStart(UserResources* userResources)
{
	HP = 100;
	MaxHP = 100;
	isInvincible = false;
	invincibleTimer = 0.0f;


}

void WIS::CharacterComponent::OnUpdate(const float elapsedTime, UserResources* userResources)
{
	invincibleTimer += elapsedTime;

	if (isInvincible && invincibleTimer >= invincibleTime) {
		isInvincible = false;
		invincibleTimer = 0.0f;
	}
	auto coll=GetOwner()->GetComponent<WIS::CollisionComponent>();
	if (isInvincible) {
		coll->SetDebugColor(DirectX::Colors::Yellow);
	}
	else
	{
		coll->SetDebugColor(DirectX::Colors::Blue);
	}
}

void WIS::CharacterComponent::OnCollision(std::shared_ptr<WIS::GameObject> otherObject)
{

}

void WIS::CharacterComponent::DecreaseHP(int damage)
{
	HP -= damage;
	if (HP < 0)
	{
		HP = 0;
		this->GetOwner()->SetActive(false);
	}
}

void WIS::CharacterComponent::IncreaseHP(int heal)
{

	HP += heal;

}

void WIS::CharacterComponent::TakeAttack(int damage)
{
	if (!isInvincible) {
		DecreaseHP(damage);
		isInvincible = true;
		invincibleTimer = 0.0f;
	}
}




