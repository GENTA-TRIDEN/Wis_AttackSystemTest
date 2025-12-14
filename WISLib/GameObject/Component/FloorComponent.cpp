#include "pch.h"
#include "FloorComponent.h"
#include "CollisionComponent.h"
#include "ModelComponent.h"


#include "WISLib/GameObject/GameObject.h"

WIS::FloorComponent::FloorComponent()
{
}

WIS::FloorComponent::~FloorComponent()
{
}

void WIS::FloorComponent::OnStart(UserResources* userResources)
{
	auto model=GetOwner()->GetComponent<WIS::ModelComponent>();
	
}

void WIS::FloorComponent::OnUpdate(const float elapsedTime, UserResources* userResources)
{

}

void WIS::FloorComponent::OnCollision(std::shared_ptr<WIS::GameObject> otherObject)
{

}

