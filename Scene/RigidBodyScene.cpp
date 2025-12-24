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


#include "InGameObject/Archetype/PlayerArchetype.h"
#include "InGameObject/Archetype/EnemyArchetype.h"
#include "InGameObject/Archetype/FloorArchetype.h"
#include "InGameObject/Archetype/WeapoArchetype.h"

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



	WIS::PlayerArchetype playerArchetype = WIS::PlayerArchetype();
	playerArchetype.Create(m_objectManager, GetUserResources(), m_PlayerText, m_PlayerModelHandle);

		
		
	m_PlayerAttackLength = 1.0f;

	//Attack()を追加


	auto player = m_objectManager->GetObject(m_PlayerText);
	auto PlayerTransform = player->GetComponent<WIS::TransformComponent>();

	for (int i = 0; i < 5; i++) {

		ItemGenerate({ static_cast<float>(rand() % 10 - 5),2.0f,static_cast<float>(rand() % 10 - 5) }, WIS::ItemKinds::GEAR);

	}


	WIS::WeapoArchetype weaponArchetype = WIS::WeapoArchetype();
	m_weaponName = weaponArchetype.Create(m_objectManager, GetUserResources(), m_weaponName, m_WeaponModelHandle, m_PlayerText).lock()->GetName();





	WIS::FloorArchetype floorArchetype = WIS::FloorArchetype();
	auto floor = floorArchetype.Create(m_objectManager, GetUserResources(), m_floorText, m_FloorModelHandle, m_PlayerText);
	std::string floorName = floor.lock()->GetName();


	WIS::EnemyArchetype enemyArchetype = WIS::EnemyArchetype();
	m_EnemyText=enemyArchetype.Create(m_objectManager, GetUserResources(), m_EnemyText, m_EnemyModelHandle, floorName).lock()->GetName();
	




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
		font->AddString((L"てき:" + std::to_wstring(enemyHP)).c_str(), { 10.0f, 90.0f });
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

