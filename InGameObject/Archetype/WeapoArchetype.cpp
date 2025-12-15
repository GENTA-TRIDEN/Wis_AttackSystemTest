#include "pch.h"
#include "FloorArchetype.h"

#include "WISLib/GameObject/Component/TransformComponent.h"
#include "WISLib/GameObject/Component/CollisionComponent.h"
#include "WISLib/GameObject/Component/CharacterComponent.h"
#include "WISLib/GameObject/Component/WeaponComponent.h"
#include "WISLib/GameObject/Component/ModelComponent.h"
#include "WISLib/GameObject/Component/RigidBodyComponent.h"
#include "WISLib/GameObject/Component/FloorComponent.h"


#include "UserResources.h"
#include "WeapoArchetype.h"

std::weak_ptr<WIS::GameObject> WIS::WeapoArchetype::Create(ObjectManager* p_objectManager, UserResources* p_userResources, const std::string& BaseName, int WeaponModelNum, const std::string& PlayerName)
{

	// 名前からオブジェクトを生成
// GenerateObjectが名前を変更する場合を考慮して戻り値を受け取る形はOKです

	auto weapon = p_objectManager->GenerateObject(BaseName, WIS::SceneTag::GAMEPLAY_SCENE);
	std::string finalName = weapon->GetName();

	p_objectManager->AddObjectSceneTag(finalName, WIS::SceneTag::STACK_SCENE);
	// デバイス関連の取得
	auto device = p_userResources->GetDeviceResources()->GetD3DDevice();
	auto context = p_userResources->GetDeviceResources()->GetD3DDeviceContext();
	auto fxFactory = p_userResources->GetEffectFactory();




	auto PlayerTransform= p_objectManager->GetObject(PlayerName)->GetComponent<WIS::TransformComponent>();

	auto weaponColl = weapon->AddComponent<WIS::CollisionComponent>();
	auto weaponModel = weapon->AddComponent<WIS::ModelComponent>();
	auto WeaponComponent = weapon->AddComponent<WIS::WeaponComponent>();
	auto WeaponTransform = weapon->GetComponent<WIS::TransformComponent>();
	weaponColl->AddBoundingOrientedBox({ 2.7f,0.2f,0.2f }, { 3.0f,0.0f,0.0f });
	weaponColl->SetDebugColor(DirectX::Colors::Red);



	int Handle = weaponModel->AddModel(device, context, L"Resources/Models/Weapon/stick.sdkmesh", fxFactory);
	weaponModel->SetRotate(Handle, { 0,0,DirectX::XMConvertToRadians(-90) });
	weaponModel->SetScale(Handle, { 2.3f,2.3f,2.3f });
	WeaponTransform->SetPosition({ 0,1,0 });
	WeaponTransform->SetParent(PlayerTransform);

	return weapon;
}
