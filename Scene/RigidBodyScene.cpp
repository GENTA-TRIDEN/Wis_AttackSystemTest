#include "pch.h"
#include "RigidBodyScene.h"


//component
#include "WISLib/GameObject/Component/TransformComponent.h"
#include "WISLib/GameObject/Component/CollisionComponent.h"
#include "WISLib/GameObject/Component/CharacterComponent.h"
#include "WISLib/GameObject/Component/WeaponComponent.h"
#include "WISLib/GameObject/Component/ModelComponent.h"
#include "WISLib/GameObject/Component/RigidBodyComponent.h"
#include "WISLib/GameObject/Component/FloorComponent.h"

#include "GameComponent/PlayerComponent.h"
#include "GameComponent/ItemComponent.h"
#include "GameComponent/EnemyComponent.h"

#include "Scene/WeaponCreateScene.h"
#include "Scene/TitleScene.h"

#include "Scene/ResultScene.h"




RigidBodyScene::RigidBodyScene()
{
}

RigidBodyScene::~RigidBodyScene()
{
}

void RigidBodyScene::Initialize()
{
	// 乱数のシードを初期化
	srand(static_cast<unsigned int>(time(NULL)));
	//初期化
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
	auto device = GetUserResources()->GetDeviceResources()->GetD3DDevice();
	auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();
	auto fxFactory = GetUserResources()->GetEffectFactory();

	// --- マネージャーの初期化 ---
	m_objectManager = GetUserResources()->GetObjectManager();
	//m_blueprintManager = GetUserResources()->GetBlueprintManager();
	p_cameraManager = WIS::CameraManager::GetInstance();
	int handle = p_cameraManager->AddCamera({ 0.0f, 2.0f, 0.0f }, WIS::CameraMode::MODE_TPSCAM, m_cameraHandle);
	p_cameraManager->RequestCameraByHandle(handle);
	SetCurrentSceneTag(WIS::SceneTag::GAMEPLAY_SCENE);
	m_objectManager->SetCurrentSceneTag(WIS::SceneTag::GAMEPLAY_SCENE);

	//GameObjectの生成
	m_objectManager->GenerateObject(m_PlayerText, WIS::SceneTag::GAMEPLAY_SCENE);
	m_objectManager->GenerateObject(m_EnemyText, WIS::SceneTag::GAMEPLAY_SCENE);
	m_objectManager->GenerateObject(m_floorText, WIS::SceneTag::GAMEPLAY_SCENE);

	//playerの設定
	auto player = m_objectManager->GetObject(m_PlayerText);
	auto playerColl = player->AddComponent<WIS::CollisionComponent>();
	auto PlayerComp = player->AddComponent<WIS::PlayerComponent>();
	auto PlayerCharacter = player->AddComponent<WIS::CharacterComponent>();
	auto PlayerRigidBody= player->AddComponent<WIS::RigidBodyComponent>();
	auto PlayerModel = player->AddComponent<WIS::ModelComponent>();


	int playerHandle = PlayerModel->AddModel(device, context, L"Resources/Models/Player.sdkmesh", fxFactory);
	PlayerModel->SetRotate(playerHandle, { 0,DirectX::XMConvertToRadians(-90),0 });
	PlayerModel->SetScale(playerHandle, { 1.0f,1.0f,1.0f });
	PlayerModel->SetPosition(playerHandle, { 0.0f,1.0f,0.0f });



	auto PlayerTransform = player->GetComponent<WIS::TransformComponent>();
	PlayerTransform->SetPosition({ 0,3,0 });
	playerColl->AddBoundingSphere(1.0f, { 0.0f,0.0f,0.0f });
	PlayerCharacter->SetObjectType(WIS::ObjectType::PLAYER);

	m_PlayerAttackLength = 1.0f;

	//Attack()を追加

	//Playerの当たり判定にコールバックを設定
	playerColl->SetOnCollisionCallback([player](std::shared_ptr<WIS::GameObject> other) {
		//auto Character=other->GetComponent<WIS::CharacterComponent>();
		//if (Character) {
		//	if (Character->GetObjectType() == WIS::ObjectType::ENEMY) {
		//		Character->TakeAttack(10);
		//	}
		//}
		});




	for (int i = 0; i < 5; i++) {

		ItemGenerate({ static_cast<float>(rand() % 10 - 5),2.0f,static_cast<float>(rand() % 10 - 5) }, WIS::ItemKinds::GEAR);

	}

	auto weapon = m_objectManager->GetObject(m_weaponName);
	if (!weapon)weapon = m_objectManager->GenerateObject(m_weaponName, WIS::SceneTag::GAMEPLAY_SCENE);

	m_objectManager->AddObjectSceneTag(m_weaponName, WIS::SceneTag::STACK_SCENE);

	auto weaponColl = weapon->AddComponent<WIS::CollisionComponent>();
	auto weaponModel = weapon->AddComponent<WIS::ModelComponent>();
	auto WeaponComponent = weapon->AddComponent<WIS::WeaponComponent>();
	auto WeaponTransform = weapon->GetComponent<WIS::TransformComponent>();
	weaponColl->AddBoundingOrientedBox({ 2.7f,0.2f,0.2f }, { 3.0f,0.0f,0.0f });
	weaponColl->SetDebugColor(DirectX::Colors::Red);



	int Handle = weaponModel->AddModel(device, context, L"Resources/Models/Weapon/stick.sdkmesh", fxFactory);
	weaponModel->SetRotate(Handle, { 0,0,DirectX::XMConvertToRadians(-90) });
	weaponModel->SetScale(Handle, { 2.3f,2.3f,2.3f });
	WeaponTransform->SetParent(PlayerTransform);

	//地面
	auto floor = m_objectManager->GetObject(m_floorText);
	auto floorModel = floor->AddComponent<WIS::ModelComponent>();
	auto floorFloor = floor->AddComponent<WIS::FloorComponent>();
	auto FloorColl = floor->AddComponent<WIS::CollisionComponent>();
	FloorColl->AddBoundingOrientedBox({ 30.0f,5.0f,30.0f }, { -15.0f,-2.0f,-15.0f });
	
	int floorHandle = floorModel->AddModel(device, context, L"Resources/Models/Floor.sdkmesh", fxFactory);
	floorModel->SetRotate(floorHandle, { DirectX::XMConvertToRadians(90),0,0 });
	floorModel->SetScale(floorHandle, { 15.0f,15.0f,15.0f });
	floorFloor->SetMeshData(floorModel->GetVertices(), floorModel->GetIndices());



	//てきの順路を床の高さに合わせる
	//enemyの設定
	auto enemy = m_objectManager->GetObject(m_EnemyText);
	auto enemyColl = enemy->AddComponent<WIS::CollisionComponent>();
	auto enemyCharacter = enemy->AddComponent<WIS::CharacterComponent>();
	auto enemyRigid = enemy->AddComponent<WIS::RigidBodyComponent>();
	
	DirectX::SimpleMath::Vector3 pos1 = { -5.0f,5.0f,-5.0f };
	DirectX::SimpleMath::Vector3 pos2 = { 5.0f,5.0f,-5.0f };
	DirectX::SimpleMath::Vector3 pos3 = { 5.0f,5.0f,5.0f };
	DirectX::SimpleMath::Vector3 pos4 = { -5.0f,5.0f,5.0f };
	pos1.y = floorFloor->GetHeight(pos1);
	pos2.y = floorFloor->GetHeight(pos2);
	pos3.y = floorFloor->GetHeight(pos3);
	pos4.y = floorFloor->GetHeight(pos4);

	auto enemyModel = enemy->AddComponent<WIS::ModelComponent>();


	int enemyHandle = enemyModel->AddModel(device, context, L"Resources/Models/Enemy.sdkmesh", fxFactory);
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

}

void RigidBodyScene::Update(float elapsedTime)
{
	auto key_state = DirectX::Keyboard::Get().GetState();
	auto mouse_state = DirectX::Mouse::Get().GetState();


	m_objectManager->Update(
		elapsedTime,
		GetUserResources()
	);
	m_DebugCountUp++;
	if (m_DebugCountMax <= m_DebugCountUp)
	{
		m_DebugCountUp = 0;
	}



	auto Weapon = m_objectManager->GetObject(m_weaponName);
	if (Weapon) {

		auto WeaponTransform = Weapon->GetComponent<WIS::TransformComponent>();
		auto WeaponWeaponComp = Weapon->GetComponent<WIS::WeaponComponent>();

		//Weaponの回転1秒で180ど
		if (m_isAttack) {
			m_WeaponAttackTime += elapsedTime;


			DirectX::SimpleMath::Quaternion rot = WeaponTransform->GetRotation();
			DirectX::SimpleMath::Quaternion addRot = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(-180.0f) * elapsedTime, 0.0f, 0.0f);

			rot = rot * addRot;
			WeaponTransform->SetRotation(rot);


			if (m_WeaponAttackTime >= 1.0f) {
				m_isAttack = false;
				m_WeaponAttackTime = 0.0f;
				//回転をリセット
				DirectX::SimpleMath::Quaternion resetRot = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, 0.0f, 0.0f);
				WeaponTransform->SetRotation(resetRot);


			}


		}

		if (mouse_state.leftButton && !m_isAttack) {
			m_isAttack = true;
			m_WeaponAttackTime = 0.0f;
			WeaponWeaponComp->TryAttack();
		}


	}


	if (key_state.Tab) {
		this->StackScene<WeaponCreateScene>();
	}



	auto player = m_objectManager->GetObject(m_PlayerText);
	auto playerTranc = player->GetComponent<WIS::TransformComponent>();
	auto enemy = m_objectManager->GetObject(m_EnemyText);

	if (enemy) {
		auto enemyEnemy = enemy->GetComponent<WIS::EnemyComponent>();
		enemyEnemy->SetPlayerPos(playerTranc->GetPosition());
	}
	else {
		ChangeScene<ResultScene>();
	}







}

void RigidBodyScene::Render()
{
	auto device = GetUserResources()->GetDeviceResources()->GetD3DDevice();
	auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();
	auto font = GetUserResources()->GetDebugFont();
	auto primitiveBatch = GetUserResources()->GetPrimitiveBatch();
	auto world = DirectX::SimpleMath::Matrix::Identity;
	m_basicEffect->SetWorld(world);
	m_basicEffect->SetView(*p_cameraManager->GetView());
	m_basicEffect->SetProjection(m_proj);

	m_basicEffect->Apply(context);
	context->IASetInputLayout(m_inputLayout.Get());
	//void XM_CALLCONV DX::DrawGrid(PrimitiveBatch<VertexPositionColor>*batch,
	//	FXMVECTOR xAxis,
	//	FXMVECTOR yAxis,
	//	FXMVECTOR origin,
	//	size_t xdivs,
	//	size_t ydivs,
	//	GXMVECTOR color)
	primitiveBatch->Begin();



	//debug表示
	DX::DrawGrid(
		primitiveBatch,
		DirectX::XMVectorSet(10.0f, 0.0f, 0.0f, 0.0f),
		DirectX::XMVectorSet(0.0f, 0.0f, 10.0f, 0.0f),
		DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		20,
		20,
		DirectX::Colors::White
	);



	primitiveBatch->End();

	m_objectManager->Render(
		GetUserResources(),
		*p_cameraManager->GetView(),
		m_proj
	);

	int enemyHP = 0;
	auto enemy = m_objectManager->GetObject(m_EnemyText);
	if (enemy)
	{
		auto enemyCharacter = enemy->GetComponent<WIS::CharacterComponent>();
		enemyHP = enemyCharacter->GetHP();
		font->AddString((L"enemyHP:" + std::to_wstring(enemyHP)).c_str(), { 10.0f, 90.0f });
	}

	int PlayerHP = 0;
	auto player = m_objectManager->GetObject(m_PlayerText);
	auto playerTrans = player->GetComponent<WIS::TransformComponent>();
	auto playerCharacter = player->GetComponent<WIS::CharacterComponent>();
	PlayerHP = playerCharacter->GetHP();
	DirectX::SimpleMath::Vector3 pos = playerTrans->GetPosition();
	
	font->AddString((L"PlayerPosX:" + std::to_wstring(pos.x)).c_str(), { 150.0f, 350.0f });
	font->AddString((L"PlayerPosY:" + std::to_wstring(pos.y)).c_str(), { 150.0f, 380.0f });
	font->AddString((L"PlayerPosZ:" + std::to_wstring(pos.z)).c_str(), { 150.0f, 410.0f });

	//void DebugFont::AddString(const wchar_t* string, DirectX::SimpleMath::Vector2 pos, FXMVECTOR color, float scale)
	font->AddString((L"PlayerHP:" + std::to_wstring(PlayerHP)).c_str(), { 10.0f, 50.0f });
	font->AddString((L"PlayerAttackLength:" + std::to_wstring(m_PlayerAttackLength)).c_str(), { 10.0f, 120.0f });
	font->AddString((L"PlayerAttackLength:" + std::to_wstring(m_PlayerAttackLength)).c_str(), { 10.0f, 120.0f });

	font->AddString((L"DebugCount:" + std::to_wstring(m_DebugCountUp)).c_str(), { 10.0f, 200.0f });
	font->AddString((L"m_WeaponAttackTime:" + std::to_wstring(m_WeaponAttackTime)).c_str(), { 10.0f, 250.0f });


	font->AddString(L"GamePlayScene(RigidBodyScene):", { 0.0f, 0.0f });

	auto PlayerComp = player->GetComponent<WIS::PlayerComponent>();
	PlayerComp->GetItemCount(WIS::ItemKinds::GEAR);
	//ItemKindsの中身を表示
	font->AddString((L"GEAR Count:" + std::to_wstring(PlayerComp->GetItemCount(WIS::ItemKinds::GEAR))).c_str(), { 10.0f, 300.0f });


}

void RigidBodyScene::Finalize()
{
	m_objectManager->Finalize();

	p_cameraManager->RequestFirstCamera();

}

void RigidBodyScene::CreateDeviceDependentResources()
{
	using namespace DirectX;
	auto device = GetUserResources()->GetDeviceResources()->GetD3DDevice();

	m_basicEffect = std::make_unique<DirectX::BasicEffect>(device);


	void const* shaderByteCode;
	size_t byteCodeLength;

	m_basicEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);


	device->CreateInputLayout(
		VertexPositionColor::InputElements,
		VertexPositionColor::InputElementCount,
		shaderByteCode,
		byteCodeLength,
		m_inputLayout.ReleaseAndGetAddressOf()
	);
}

void RigidBodyScene::CreateWindowSizeDependentResources()
{
	auto deviceResources = GetUserResources()->GetDeviceResources();
	auto size = deviceResources->GetOutputSize();
	float aspectRatio = float(size.right) / float(size.bottom);
	m_proj = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(
		DirectX::XMConvertToRadians(45.0f),
		aspectRatio,
		0.1f,
		100.0f
	);




}

void RigidBodyScene::OnDeviceLost()
{
}

void RigidBodyScene::Attack()
{
}

std::string RigidBodyScene::ItemGenerate(DirectX::SimpleMath::Vector3 Pos, WIS::ItemKinds kinds)
{
	//Itemの生成処理
	std::string name = "Item_" + std::to_string(m_itemNames.size());
	m_itemNames.push_back(name);
	auto item = m_objectManager->GenerateObject(name, WIS::SceneTag::GAMEPLAY_SCENE);
	//Itemの設定

	auto itemTransform = item->GetComponent<WIS::TransformComponent>();
	auto itemColl = item->AddComponent<WIS::CollisionComponent>();
	auto itemModel = item->AddComponent<WIS::ModelComponent>();
	item->AddComponent<WIS::RigidBodyComponent>();
	auto itemComp = item->AddComponent<WIS::ItemComponent>(WIS::ItemKinds::GEAR);
	itemTransform->SetPosition(Pos);
	itemComp->SetItemKinds(kinds);
	itemColl->AddBoundingSphere(1.0f, { 0.0f,0.0f,0.0f });

	int Handle = itemModel->AddModel(GetUserResources()->GetDeviceResources()->GetD3DDevice(),
		GetUserResources()->GetDeviceResources()->GetD3DDeviceContext(),
		L"Resources/Models/Weapon/Gear.sdkmesh",
		GetUserResources()->GetEffectFactory());
	itemModel->SetRotate(Handle, { 0,0,DirectX::XMConvertToRadians(-90) });
	itemModel->SetScale(Handle, { 0.8f,0.8f,0.8f });



	return name;
}

