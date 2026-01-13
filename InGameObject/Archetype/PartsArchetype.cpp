#include "pch.h"
#include "PartsArchetype.h"
#include "UserResources.h"

#include "WISLib/GameObject/Component/TransformComponent.h"
#include "WISLib/GameObject/Component/CollisionComponent.h"
#include "WISLib/GameObject/Component/CharacterComponent.h"
#include "WISLib/GameObject/Component/ModelComponent.h"
#include "WISLib/GameObject/Component/RigidBodyComponent.h"
#include "WISLib/GameObject/Component/FloorComponent.h"
#include "GameComponent/ItemComponent.h"

//ItemComponent
#include "GameComponent/ItemComponent/GearItemComponent.h"
#include "GameComponent/ItemComponent/BombItemComponent.h"





std::weak_ptr<WIS::GameObject> WIS::PartsArchetype::Create(
	ObjectManager* p_objectManager,
	UserResources* p_userResources,
	const std::string& BaseName, 
	int PartsModelNumconst, 
	const std::vector<std::string>& m_itemNames,
	WIS::ItemKinds itemKinds,
	DirectX::SimpleMath::Vector3 SpawnPos,
	DirectX::SimpleMath::Vector3 SpawnImpact)
{
	auto parts = p_objectManager->GenerateObject(BaseName, WIS::SceneTag::GAMEPLAY_SCENE);
	std::string finalName = parts->GetName();
	// デバイス関連の取得
	auto device = p_userResources->GetDeviceResources()->GetD3DDevice();
	auto context = p_userResources->GetDeviceResources()->GetD3DDeviceContext();
	auto fxFactory = p_userResources->GetEffectFactory();
	auto partsColl = parts->AddComponent<WIS::CollisionComponent>();
	auto partsModel = parts->AddComponent<WIS::ModelComponent>();
	auto partsTransform = parts->GetComponent<WIS::TransformComponent>();
	
	//リジッドボディ
	auto partsRigid = parts->AddComponent<WIS::RigidBodyComponent>();

	partsRigid->SetHeightOffset(0.7f);
	
	partsRigid->AddImpulse(SpawnImpact);


	partsColl->SetCollisionTag(WIS::CollisionTag::DYNAMIC);

	//Partskindの設定
	auto itemComp = AddItemComponent(parts, itemKinds);

	//当たり判定の設定
	partsColl->AddBoundingSphere(1.0f, { 0.0f,0.0f,0.0f });



	//モデル
	int Handle = partsModel->AddModel(device, context, itemComp->GetItemModelPath(), fxFactory);

	partsTransform->SetPosition(SpawnPos);
		
	partsModel->SetRotate(Handle, Rotate);
	partsModel->SetScale(Handle, { 0.8f,0.8f,0.8f });


	//灰色
	//partsModel->SetColor(Handle, { 2.0f / 255.0f,2.0f / 255.0f,2.0f / 255.0f,1.0f });
	return parts;
}

std::shared_ptr<WIS::ItemComponent> WIS::PartsArchetype::AddItemComponent(std::shared_ptr<WIS::GameObject> obj, WIS::ItemKinds itemKinds)
{
	switch (itemKinds)
	{
	case WIS::ItemKinds::BLADE:
		return obj->AddComponent<WIS::GearItemComponent>();
	case WIS::ItemKinds::BOMB:
		Rotate = { 0, DirectX::XMConvertToRadians(-90), 0 };
		return obj->AddComponent<WIS::BombItemComponent>();
	case WIS::ItemKinds::ARROW:

		return obj->AddComponent<WIS::GearItemComponent>();
	case WIS::ItemKinds::GEAR:
		Rotate = { 0, 0, DirectX::XMConvertToRadians(-90) };
		return obj->AddComponent<WIS::GearItemComponent>();
	default:
		return obj->AddComponent<WIS::GearItemComponent>();
	}
	return nullptr;
}
