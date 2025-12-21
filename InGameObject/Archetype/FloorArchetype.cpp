#include "pch.h"
#include "FloorArchetype.h"

#include "WISLib/GameObject/Component/TransformComponent.h"
#include "WISLib/GameObject/Component/CollisionComponent.h"
#include "WISLib/GameObject/Component/CharacterComponent.h"
#include "WISLib/GameObject/Component/ModelComponent.h"
#include "WISLib/GameObject/Component/RigidBodyComponent.h"
#include "WISLib/GameObject/Component/FloorComponent.h"
#include "WISLib/GameObject/Component/PolygonComponent.h"


#include "UserResources.h"


std::weak_ptr<WIS::GameObject> WIS::FloorArchetype::Create(ObjectManager* p_objectManager, UserResources* p_userResources, const std::string& BaseName, int FloorModelNumconst, const std::string& playerName)
{

	auto device = p_userResources->GetDeviceResources()->GetD3DDevice();
	auto context = p_userResources->GetDeviceResources()->GetD3DDeviceContext();
	auto fxFactory = p_userResources->GetEffectFactory();

    auto floor = p_objectManager->GenerateObject(BaseName, WIS::SceneTag::GAMEPLAY_SCENE);
	auto floorModel = floor->AddComponent<WIS::ModelComponent>();
	auto floorFloor = floor->AddComponent<WIS::FloorComponent>();
	auto floorPolygon = floor->AddComponent<WIS::PolygonComponent>();
	auto FloorColl = floor->AddComponent<WIS::CollisionComponent>();
	FloorColl->AddBoundingOrientedBox({ 1500.0f,50.0f,1500.0f }, { 0.0f,-2.0f,0.0f });
	
	fxFactory->SetDirectory(L"Resources/Models");
	int floorHandle = floorModel->AddModel(device, context, floorModelPath, fxFactory);
	//floorModel->SetTextureFromFile(device, floorHandle, L"Resources/Models/Stage.dds");


	//floorModel->SetDepthStencilState(p_userResources->GetCommonStates()->DepthRead());


	floorModel->SetRotate(floorHandle, { DirectX::XMConvertToRadians(0),0,0 });
	floorModel->SetScale(floorHandle, ModelScale);
	floorModel->SetPosition(floorHandle, { 0.0f,0.0f,0.0f });
	floorModel->SetColor(floorHandle, { 32.0f/255.0f,153.0f/255.0f,40.0f/255.0f,1.0f });


	floorFloor->SetMeshData(floorModel->GetVertices(), floorModel->GetIndices());


	//std::vector<DirectX::VertexPositionColor> VertexList;
	//std::vector<uint16_t> IndexList;
	//VertexList = {
	//	DirectX::VertexPositionColor{{2.0f,0.0f,2.0f,1.0f},{1.0f,1.0f,1.0f,1.0f}},
	//	DirectX::VertexPositionColor{{2.0f,0.0f,-2.0f,1.0f},{1.0f,1.0f,1.0f,1.0f}},
	//	DirectX::VertexPositionColor{{-2.0f,0.0f,2.0f,1.0f},{1.0f,1.0f,1.0f,1.0f}},
	//	DirectX::VertexPositionColor{{-2.0f,0.0f,-2.0f,1.0f},{1.0f,1.0f,1.0f,1.0f}},

	//};
	//	0, 3, 1,	//IndexList = {0, 1, 2,

	//};


	//floorFloor->SetMeshData(VertexList, IndexList);
	


    return floor;
}
