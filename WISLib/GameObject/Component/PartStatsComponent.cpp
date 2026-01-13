#include "pch.h"
#include "PartStatsComponent.h"
#include "WISLib/GameObject/GameObject.h"
#include "CharacterComponent.h" 
#include "RigidBodyComponent.h" 
#include "WeaponComponent.h" 
#include "TransformComponent.h" 
#include "WeaponPartsComponent.h" 

#include "UserResources.h"
#include <iostream> // デバッグ/ログ出力用

//一時的な攻撃判定

using namespace WIS;

void WIS::PartStatsComponent::OnStart(UserResources* userResources)
{
	m_Bombeffect = std::make_unique<WIS::BombEffect>();
	m_Bombeffect->Create(userResources->GetDeviceResources());
}

void WIS::PartStatsComponent::OnUpdate(const float elapsedTime, UserResources* userResources)
{

	if (m_Bombeffect) {

		m_Bombeffect->Update(elapsedTime);

	}
}

void WIS::PartStatsComponent::OnRender(UserResources* userResources, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{
	auto camera = userResources->GetCameraManager()->GetCamera();

	if (m_Bombeffect) {

		m_Bombeffect->CreateBillboard(
			camera->GetTarget(),
			camera->GetPosition(),
			DirectX::SimpleMath::Vector3::UnitY
		);
		m_Bombeffect->Render(view, proj);
	}
}

// 衝突時にパーツ固有の特殊効果を適用する
void PartStatsComponent::ApplyOnHitEffect(std::shared_ptr<WIS::GameObject> hitTarget, float baseDamage, DirectX::SimpleMath::Vector3 effectPos)
{
	//ターゲット
	auto enemyrigid = hitTarget->GetComponent<WIS::RigidBodyComponent>();
	//Weapon
	auto owner = this->GetOwner();
	//WeaponComp
	auto weaponPartsComp = owner->GetComponent<WIS::WeaponPartsComponent>();

	if(enemyrigid&& weaponPartsComp){

	DirectX::SimpleMath::Vector3 impulse = { 0,0,0 };
	enemyrigid->AddImpulse(impulse);
	}

	switch (m_partType)
	{
	case WIS::ItemKinds::NONE:
		break;
	case WIS::ItemKinds::BLADE:
		break;
	case WIS::ItemKinds::BOMB:

		if (m_Bombeffect) {

			for (size_t i = 0; i < 5; i++)
			{

				m_Bombeffect->hit(effectPos);
			}

		}



		break;
	case WIS::ItemKinds::ARROW:
		break;
	case WIS::ItemKinds::GEAR:
		break;
	default:
		break;
	}

}