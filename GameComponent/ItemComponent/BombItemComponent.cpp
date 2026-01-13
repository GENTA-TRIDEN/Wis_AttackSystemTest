
#include "pch.h"
#include "BombItemComponent.h"
#include "WISLib/GameObject/GameObject.h"
#include "UserResources.h"

#include "WISLib/GameObject/Component/TransformComponent.h"

#include "WISLib/GameObject/Component/InstantAttackComponent.h"
#include "WISLib/GameObject/Component/CollisionComponent.h"

#include "WISLib/UserInterface/BombEffect.h"


std::shared_ptr<WIS::ItemComponent> WIS::BombItemComponent::CreateItemComponent(WIS::ItemKinds itemKinds)
{



	return this->WIS::ComponentBase::GetOwner()->GetComponent<WIS::ItemComponent>();
}

void WIS::BombItemComponent::UniqueOnStart(UserResources* userResources)
{
	m_Bombeffect = std::make_unique<WIS::BombEffect>();

	m_Bombeffect->Create(userResources->GetDeviceResources());
}

void WIS::BombItemComponent::UniqueUpdate(const float elapsedTime, UserResources* userResources)
{
	m_Bombeffect->Update(elapsedTime);

	auto keyboard = DirectX::Keyboard::Get().GetState();

}

void WIS::BombItemComponent::UniqueOnRender(UserResources* userResources, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{

	//void CreateBillboard(
	//	DirectX::SimpleMath::Vector3 target,
	//	DirectX::SimpleMath::Vector3 eye,
	//	DirectX::SimpleMath::Vector3 up);

	auto cameramanager = userResources->GetCameraManager();

	auto camera = cameramanager->GetCamera();
	if (m_Bombeffect) {

		if (!camera)
			return;
		m_Bombeffect->CreateBillboard(
			camera->GetTarget(),
			camera->GetPosition(),
			DirectX::SimpleMath::Vector3::UnitY
		);

		//void WIS::BombEffect::Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
		m_Bombeffect->Render(view, proj);


	}
}

void WIS::BombItemComponent::UniqueOnCollision(std::shared_ptr<WIS::GameObject> otherObject)
{


}

void WIS::BombItemComponent::SpawnExplosion(UserResources* resources)
{
	
	// 1. ObjectManagerを通して生成（これでTransformが自動追加される）
		// currentTag は現在実行中のシーンタグ（GAMEなど）
	auto explosion = resources->GetObjectManager()->GenerateObject("Explosion", WIS::SceneTag::GAMEPLAY_SCENE);

	// 2. 位置を設定
	auto trans = explosion->GetComponent<TransformComponent>();
	auto mytrans = this->GetOwner()->GetComponent<TransformComponent>();
	trans->SetPosition(mytrans->GetPosition());

	// 3. 判定の設定
	auto col = explosion->AddComponent<CollisionComponent>();
	col->AddBoundingSphere(3.0f); // 半径3
	col->SetCollisionTag(CollisionTag::DYNAMIC);
	// 自爆防止
	col->SetCollisionUniqueNumber(GetOwner()->GetComponent<CollisionComponent>()->GetCollisionUniqueNumber());

	// 4. 攻撃ロジックと寿命の設定
	// 寿命0.2秒、ダメージ50
	explosion->AddComponent<InstantAttackComponent>(0.2f, 50);

}

