#include "pch.h"
#include "FloorArchetype.h"

#include "WISLib/GameObject/Component/TransformComponent.h"
#include "WISLib/GameObject/Component/CollisionComponent.h"
#include "WISLib/GameObject/Component/CharacterComponent.h"
#include "WISLib/GameObject/Component/ModelComponent.h"
#include "WISLib/GameObject/Component/RigidBodyComponent.h"
#include "WISLib/GameObject/Component/FloorComponent.h"


#include "UserResources.h"


std::weak_ptr<WIS::GameObject> WIS::FloorArchetype::Create(ObjectManager* p_objectManager, UserResources* p_userResources, const std::string& BaseName, int FloorModelNumconst, const std::string& playerName)
{

	auto device = p_userResources->GetDeviceResources()->GetD3DDevice();
	auto context = p_userResources->GetDeviceResources()->GetD3DDeviceContext();
	auto fxFactory = p_userResources->GetEffectFactory();


    auto floor = p_objectManager->GenerateObject(BaseName, WIS::SceneTag::GAMEPLAY_SCENE);
	auto floorModel = floor->AddComponent<WIS::ModelComponent>();
	auto floorFloor = floor->AddComponent<WIS::FloorComponent>();
	auto FloorColl = floor->AddComponent<WIS::CollisionComponent>();

	FloorColl->AddBoundingOrientedBox({ 50.0f,50.0f,50.0f }, { -15.0f,-2.0f,-15.0f });

	int floorHandle = floorModel->AddModel(device, context, floorModelPath, fxFactory);
	floorModel->SetRotate(floorHandle, { DirectX::XMConvertToRadians(0),0,0 });
	floorModel->SetScale(floorHandle, ModelScale);
	floorFloor->SetMeshData(floorModel->GetVertices(), floorModel->GetIndices());

    return floor;
}
