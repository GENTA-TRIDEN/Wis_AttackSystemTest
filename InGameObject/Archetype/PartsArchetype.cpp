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





std::weak_ptr<WIS::GameObject> WIS::PartsArchetype::Create(ObjectManager* p_objectManager, UserResources* p_userResources, const std::string& BaseName, int PartsModelNumconst, const std::vector<std::string>& m_itemNames, WIS::ItemKinds itemKinds,
	DirectX::SimpleMath::Vector3 SpawnPos)
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

		
	//Partskindの設定
	auto itemComp = parts->AddComponent<WIS::ItemComponent>(itemKinds);
	//当たり判定の設定
	partsColl->AddBoundingSphere(1.0f, { 0.0f,0.0f,0.0f });

	//モデル
	int Handle = partsModel->AddModel(device, context, GetItemModelPath(itemKinds), fxFactory);
	partsTransform->SetPosition(SpawnPos);
		
	partsModel->SetRotate(Handle, { 0,0,DirectX::XMConvertToRadians(-90) });
	partsModel->SetScale(Handle, { 0.8f,0.8f,0.8f });

	return parts;
}


//NONE = 0,
//BLADE,//刃
//SHIELD,//盾
//BOMB,//爆弾
//ARROW,
//GEAR//歯車
const wchar_t* WIS::PartsArchetype::GetItemModelPath(WIS::ItemKinds itemKinds)
{

	switch (itemKinds)
	{
	case WIS::ItemKinds::BLADE:
		return L"Resources/Models/Weapon/Blade.sdkmesh";

	case WIS::ItemKinds::BOMB:
		return L"Resources/Models/Weapon/Bomb.sdkmesh";

	case WIS::ItemKinds::ARROW:
		return L"Resources/Models/Weapon/Arrow.sdkmesh";

	case WIS::ItemKinds::GEAR:
		return L"Resources/Models/Weapon/Gear.sdkmesh";

	default:
		return L"Resources/Models/Weapon/Gear.sdkmesh";
	}
	return L"";

}
