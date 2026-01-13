#include "pch.h"
#include "ItemComponent.h"
#include "WISLib/GameObject/GameObject.h"
#include "WISLib/GameObject/Component/CollisionComponent.h"
#include "WISLib/GameObject/Component/TransformComponent.h"
#include "WISLib/GameObject/Component/ModelComponent.h"
#include "GameComponent/PlayerComponent.h"
#include "UserResources.h"

void WIS::ItemComponent::OnStart(UserResources* userResources)
{
	//Collisionを持っていなかったら追加する
	(void)userResources; // C4100 警告対応

	auto device = userResources->GetDeviceResources()->GetD3DDevice();
	auto context = userResources->GetDeviceResources()->GetD3DDeviceContext();
	auto fxFactory = userResources->GetEffectFactory();

	auto CollisionComp=GetOwner()->GetComponent<WIS::CollisionComponent>();
	auto ModelComp = GetOwner()->GetComponent<WIS::ModelComponent>();
	auto CollisionTrans=GetOwner()->GetComponent<WIS::TransformComponent>();

	if (!CollisionComp)
	{
		CollisionComp = GetOwner()->AddComponent<WIS::CollisionComponent>();
		CollisionComp->AddBoundingSphere(1.0f, { 0.0f,0.0f,0.0f });
	}




	if (!ModelComp) {
		ModelComp = GetOwner()->AddComponent<WIS::ModelComponent>();
		int Handle = ModelComp->AddModel(device, context, L"Resources/Models/Weapon/Gear.sdkmesh", fxFactory);
		ModelComp->SetRotate(Handle, { 0,0,DirectX::XMConvertToRadians(-90) });
		ModelComp->SetScale(Handle, { 0.8f,0.8f,0.8f });
	}

	UniqueOnStart(userResources);

}

void WIS::ItemComponent::OnUpdate(const float elapsedTime, UserResources* userResources)
{
	UniqueUpdate(elapsedTime, userResources);
	//その場で回転する
	auto TransformComp = GetOwner()->GetComponent<WIS::TransformComponent>();

	m_timer += elapsedTime;

	//2秒かけて一回転する
	DirectX::SimpleMath::Quaternion rot = TransformComp->GetRotation();
	DirectX::SimpleMath::Quaternion addRot = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(180.0f) * elapsedTime, 0.0f, 0.0f);
	rot = rot * addRot;
	TransformComp->SetRotation(rot);




}

void WIS::ItemComponent::OnRender(UserResources* userResources, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{
	UniqueOnRender(userResources, view, proj);
}

void WIS::ItemComponent::OnCollision(std::shared_ptr<WIS::GameObject> otherObject)
{
	UniqueOnCollision(otherObject);
	auto player = otherObject->GetComponent<WIS::PlayerComponent>();

	if (m_timer >= m_camGettingInterval) {


	if (player) {
		player->AddItem(m_itemType);
		// アイテム取得時の処理（例: アイテムオブジェクトの削除）
		this->GetOwner()->SetActive(false);
	}

	}


	

}
