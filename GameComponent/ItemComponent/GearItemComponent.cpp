
#include "pch.h"
#include "GearItemComponent.h"
#include "WISLib/GameObject/GameObject.h"


std::shared_ptr<WIS::ItemComponent> WIS::GearItemComponent::CreateItemComponent(WIS::ItemKinds itemKinds)
{








	return this->WIS::ComponentBase::GetOwner()->GetComponent<WIS::ItemComponent>();
}

void WIS::GearItemComponent::UniqueOnStart(UserResources* userResources)
{
}

void WIS::GearItemComponent::UniqueUpdate(const float elapsedTime, UserResources* userResources)
{
	int a = 5;
}

void WIS::GearItemComponent::UniqueOnRender(UserResources* userResources, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{
}

void WIS::GearItemComponent::UniqueOnCollision(std::shared_ptr<WIS::GameObject> otherObject)
{
}

