#include "pch.h"
#include "EnemyArchetype.h"



#include "WISLib/GameObject/Component/StateMachineComponent.h"


#include "WISLib/GameObject/Component/TransformComponent.h"
#include "WISLib/GameObject/Component/CollisionComponent.h"
#include "WISLib/GameObject/Component/CharacterComponent.h"
#include "WISLib/GameObject/Component/ModelComponent.h"
#include "WISLib/GameObject/Component/RigidBodyComponent.h"
#include "WISLib/GameObject/Component/FloorComponent.h"

#include "GameComponent/EnemyComponent.h"

#include "UserResources.h"

std::weak_ptr<WIS::GameObject> WIS::EnemyArchetype::Create(ObjectManager* p_objectManager, UserResources* p_userResources, const std::string& BaseName, int EnemyModelNumconst ,std::string& floorName )
{


	// デバイス関連の取得
	auto device = p_userResources->GetDeviceResources()->GetD3DDevice();
	auto context = p_userResources->GetDeviceResources()->GetD3DDeviceContext();
	auto fxFactory = p_userResources->GetEffectFactory();

    	//てきの順路を床の高さに合わせる
	//enemyの設定
	auto enemy = p_objectManager->GenerateObject(BaseName,WIS::SceneTag::GAMEPLAY_SCENE);
	std::string finalName = enemy->GetName();

	auto enemyColl = enemy->AddComponent<WIS::CollisionComponent>();
	auto enemyCharacter = enemy->AddComponent<WIS::CharacterComponent>();
	auto enemyRigid = enemy->AddComponent<WIS::RigidBodyComponent>();
	enemy->AddComponent<StateMachineComponent<EnemyComponent>>();

	auto value = enemyRigid->GetValue();
	value.gravity += 10.0f;
	enemyRigid->SetValue(value);
	DirectX::SimpleMath::Vector3 pos1 = { -5.0f,5.0f,-5.0f };
	DirectX::SimpleMath::Vector3 pos2 = { 5.0f,5.0f,-5.0f };
	DirectX::SimpleMath::Vector3 pos3 = { 5.0f,5.0f,5.0f };
	DirectX::SimpleMath::Vector3 pos4 = { -5.0f,5.0f,5.0f };

	auto floorFloor = p_objectManager->GetObject(floorName)->GetComponent<WIS::FloorComponent>();
	pos1.y = floorFloor->GetHeight(pos1);
	pos2.y = floorFloor->GetHeight(pos2);
	pos3.y = floorFloor->GetHeight(pos3);
	pos4.y = floorFloor->GetHeight(pos4);

	auto enemyModel = enemy->AddComponent<WIS::ModelComponent>();


	int enemyHandle = enemyModel->AddModel(device, context, enemyModelpath, fxFactory);
	enemyModel->SetRotate(enemyHandle, { 0,DirectX::XMConvertToRadians(90),0 });
	enemyModel->SetScale(enemyHandle, { 1.0f,1.0f,1.0f });
	enemyModel->SetPosition(enemyHandle, { 0.0f,1.0f,0.0f });


	auto enemyEnemy = enemy->AddComponent<WIS::EnemyComponent>(pos1,pos2);
	enemyEnemy->AddPatrolPoint(pos3);
	enemyEnemy->AddPatrolPoint(pos4);

	enemyColl->AddBoundingSphere(2.0f, { 0.0f,0.0f,0.0f });

	enemyCharacter->SetObjectType(WIS::ObjectType::ENEMY);

	//enemyTransform
	auto enemyTransform = enemy->GetComponent<WIS::TransformComponent>();
	enemyTransform->SetPosition({ 0.0f,2.0f,5.0f });
	DirectX::SimpleMath::Quaternion rot = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(30.0f), 0.0f, 0.0f);
	enemyTransform->SetRotation(rot);

	
    return enemy;
}
