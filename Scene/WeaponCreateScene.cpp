#include "pch.h"
#include "WeaponCreateScene.h"


//component
#include "WISLib/GameObject/Component/TransformComponent.h"
#include "WISLib/GameObject/Component/CollisionComponent.h"
#include "WISLib/GameObject/Component/CharacterComponent.h"
#include "WISLib/GameObject/Component/ModelComponent.h"
#include "WISLib/GameObject/Component/WeaponComponent.h"
#include "WISLib/GameObject/Component/WeaponPartsComponent.h"
#include "WISLib/GameObject/Component/PartStatsComponent.h"


#include "GameComponent/PlayerComponent.h"


#include "Scene/AttackScene.h"
#include "Scene/WeaponComponentTestScene.h"
#include "Scene/PlayerTestScene.h"
#include "Scene/ItemGetScene.h"

//

#include "WISLib/UserInterface/Button.h"
#include "WISLib/UserInterface/UserInterface.h"


using namespace DirectX;

WeaponCreateScene::WeaponCreateScene()
{
}

WeaponCreateScene::~WeaponCreateScene()
{
}

void WeaponCreateScene::Initialize()
{
	// 乱数のシードを初期化
	srand(static_cast<unsigned int>(time(NULL)));
	//初期化
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
	auto device = GetUserResources()->GetDeviceResources()->GetD3DDevice();
	auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();

	// --- マネージャーの初期化 ---
	m_objectManager = GetUserResources()->GetObjectManager();
	//m_blueprintManager = GetUserResources()->GetBlueprintManager();
	p_cameraManager = WIS::CameraManager::GetInstance();
	m_ReturnCameraHandle = p_cameraManager->GetCurrentCameraHandle();
	p_cameraManager->AddCamera({ 00.0f, 5.0f, 0.0f }, WIS::CameraMode::MODE_FIXED, m_cameraHandle);
	p_cameraManager->RequestCameraByHandle(m_cameraHandle);
	p_cameraManager->GetCamera(m_cameraHandle)->SetPosition({ 0.0f, 0.0f, 0.0f });
	SetCurrentSceneTag(WIS::SceneTag::STACK_SCENE);
	m_objectManager->SetCurrentSceneTag(WIS::SceneTag::STACK_SCENE);

	DirectX::Mouse::Get().SetMode(DirectX::Mouse::Mode::MODE_RELATIVE);
	DirectX::Mouse::Get().SetVisible(true);
	//GameObjectの生成
	auto fxFactory = GetUserResources()->GetEffectFactory();



	m_button->Initialize(GetUserResources()->GetDeviceResources(), 1280, 720);
	m_BombButton->Initialize(GetUserResources()->GetDeviceResources(), 1280, 720);
	//m_UpButton->Initialize(GetUserResources()->GetDeviceResources(), 1280, 720);
	//m_DownButton->Initialize(GetUserResources()->GetDeviceResources(), 1280, 720);
	m_RightButton->Initialize(GetUserResources()->GetDeviceResources(), 1280, 720);
	m_LeftButton->Initialize(GetUserResources()->GetDeviceResources(), 1280, 720);
	m_SelectButton->Initialize(GetUserResources()->GetDeviceResources(), 1280, 720);




	//オブジェクトの生成
	auto player = m_objectManager->GetObject(m_playerText);
	if (player) {

	}

	//Weaponの生成
	auto weapon = m_objectManager->GetObject(m_weaponText);
	if (weapon){

		m_weaponArchetype.AddGameObjectName(m_weaponText);
		auto weaponTransform = weapon->GetComponent<WIS::TransformComponent>();
		auto weaponColl = weapon->GetComponent<WIS::CollisionComponent>();
		auto weaponModel = weapon->GetComponent<WIS::ModelComponent>();
		auto WeaponComponent = weapon->GetComponent<WIS::WeaponComponent>();

		//we

		//SetParentにnullptrを入れて親子関係を切る
		weaponTransform->SetParent(nullptr);
		WeaponMat = weaponTransform->GetWorldMatrix();
		WeaponPos = weaponTransform->GetPosition();
		weaponTransform->SetPosition({ -4.0f, 0.0f, 0.0f });
		DirectX::SimpleMath::Quaternion rot = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, 0.0f, 0.0f);
		weaponTransform->SetRotation(rot);
		weaponModel->SetRotate(0, { 0,0,DirectX::XMConvertToRadians(0) });

		
	}
	
	m_button->SetTexture(WIS::Button::STATE::Normal, L"Resources/Textures/GearSilhouette.png");
	m_BombButton->SetTexture(WIS::Button::STATE::Normal, L"Resources/Textures/BombSilhouette.png");
	m_LeftButton->SetTexture(WIS::Button::STATE::Normal, L"Resources/Textures/LeftArrowButton.png");
	m_RightButton->SetTexture(WIS::Button::STATE::Normal, L"Resources/Textures/RightArrowButton.png");
	m_SelectButton->SetTexture(WIS::Button::STATE::Normal, L"Resources/Textures/SelectButton.png");

	m_currentWeaponPartsNameIndex = -1;

	//↓これを直したい
	m_SelectButton->SetOnCnClickCallbacks([this]() {
		this->ChangeNextWeaponParts();
		});
	

}


void WeaponCreateScene::Update(float elapsedTime)
{

	DirectX::Mouse::Get().SetMode(DirectX::Mouse::Mode::MODE_ABSOLUTE);
	DirectX::Mouse::Get().SetVisible(true);



	auto key_state = DirectX::Keyboard::Get().GetState();
	auto mouse_state = DirectX::Mouse::Get().GetState();

	m_DebugCountUp++;
	if (m_DebugCountMax <= m_DebugCountUp)
	{
		m_DebugCountUp = 0;
	}


	m_objectManager->Update(
		elapsedTime,
		GetUserResources()
	);


	m_button->Update();
	m_BombButton->Update();
	//m_UpButton->Update();
	//m_DownButton->Update();
	m_RightButton->Update();
	m_LeftButton->Update();
	m_SelectButton->Update();
	std::string name;

	//@TODOここら辺を関数にする
	if (m_button->IsClick() && isClick == false) {
		auto player = m_objectManager->GetObject(m_playerText)->GetComponent<WIS::PlayerComponent>();
		
		int itemCount=player->GetItemCount(WIS::ItemKinds::GEAR);
		if (itemCount > 0) {


		clickCount++;
		itemCount--;
		player->SetItemCount(WIS::ItemKinds::GEAR, itemCount);
		auto weaponParts=m_objectManager->GenerateObject(m_weaponPartsText + std::to_string(clickCount), WIS::SceneTag::STACK_SCENE);
		name = weaponParts->GetName();
		m_currentWeaponPartsNameIndex++;

		m_objectManager->AddObjectSceneTag(name, WIS::SceneTag::GAMEPLAY_SCENE);
		m_weaponArchetype.AddGameObjectName(name);
		auto device = GetUserResources()->GetDeviceResources()->GetD3DDevice();
		auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();
		auto fxFactory = GetUserResources()->GetEffectFactory();

		auto Weapon = m_objectManager->GetObject(m_weaponText);
		auto WeaponWeaponComponent = Weapon->GetComponent<WIS::WeaponComponent>();

		m_objectManager->GetObject(name);
		auto weaponPartsTransform = weaponParts->GetComponent<WIS::TransformComponent>();
		auto weaponPartsColl = weaponParts->AddComponent<WIS::CollisionComponent>();
		auto weaponModel = weaponParts->AddComponent<WIS::ModelComponent>();

		auto WeaponPartsComp = weaponParts->AddComponent<WIS::WeaponPartsComponent>(Weapon);
		auto WeaponPartsStatsComp = weaponParts->AddComponent<WIS::PartStatsComponent>();

		weaponPartsColl->AddBoundingOrientedBox({ 0.5f,0.5f,0.5f }, { 0.0f,0.0f,0.0f });
		weaponPartsTransform->SetParent(m_objectManager->GetObject(m_weaponText)->GetComponent<WIS::TransformComponent>());
		weaponPartsTransform->SetPosition({ 3.0f, 0.0f, 0.0f });
		DirectX::SimpleMath::Quaternion rot = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, 0.0f, 0.0f);
		weaponPartsTransform->SetRotation(rot);

		int Handle = weaponModel->AddModel(device, context, L"Resources/Models/Weapon/Gear.sdkmesh", fxFactory);


		WeaponWeaponComponent->AddWeight(10.0f);
		WeaponWeaponComponent->AttachPart(weaponParts);


		WeaponPartsName.push_back(weaponParts->GetName());

		}
	}
	//上記のBomb版
	if (m_BombButton->IsClick() && isClick == false) {
		auto player = m_objectManager->GetObject(m_playerText)->GetComponent<WIS::PlayerComponent>();
		int itemCount = player->GetItemCount(WIS::ItemKinds::BOMB);
		if (itemCount > 0) {
			clickCount++;
			itemCount--;
			player->SetItemCount(WIS::ItemKinds::BOMB, itemCount);
			auto weaponParts = m_objectManager->GenerateObject(m_weaponPartsText + std::to_string(clickCount), WIS::SceneTag::STACK_SCENE);
			name = weaponParts->GetName();
			m_currentWeaponPartsNameIndex++;
			m_objectManager->AddObjectSceneTag(name, WIS::SceneTag::GAMEPLAY_SCENE);
			m_weaponArchetype.AddGameObjectName(name);
			auto device = GetUserResources()->GetDeviceResources()->GetD3DDevice();
			auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();
			auto fxFactory = GetUserResources()->GetEffectFactory();
			auto Weapon = m_objectManager->GetObject(m_weaponText);
			auto WeaponWeaponComponent = Weapon->GetComponent<WIS::WeaponComponent>();
			m_objectManager->GetObject(name);
			auto weaponPartsTransform = weaponParts->GetComponent<WIS::TransformComponent>();
			auto weaponPartsColl = weaponParts->AddComponent<WIS::CollisionComponent>();
			auto weaponModel = weaponParts->AddComponent<WIS::ModelComponent>();
			auto WeaponPartsComp = weaponParts->AddComponent<WIS::WeaponPartsComponent>(Weapon);
			auto WeaponPartsStatsComp = weaponParts->AddComponent<WIS::PartStatsComponent>();
			weaponPartsColl->AddBoundingOrientedBox({ 0.5f,0.5f,0.5f }, { 0.0f,0.0f,0.0f });
			weaponPartsTransform->SetParent(m_objectManager->GetObject(m_weaponText)->GetComponent<WIS::TransformComponent>());
			weaponPartsTransform->SetPosition({ 3.0f, 0.0f, 0.0f });
			DirectX::SimpleMath::Quaternion rot = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, 0.0f, 0.0f);
			weaponPartsTransform->SetRotation(rot);
			int Handle = weaponModel->AddModel(device, context, L"Resources/Models/Weapon/Bomb.sdkmesh", fxFactory);
			WeaponWeaponComponent->AddWeight(20.0f);
			WeaponWeaponComponent->AttachPart(weaponParts);
			WeaponPartsName.push_back(weaponParts->GetName());
		}

	}




	if (m_button->IsClick()) {
		
		isClick = true;
	}
	else {
		isClick = false;
	}

	//WeaponPartsの移動

	if (WeaponPartsName.size()>0&&m_objectManager->GetObject(WeaponPartsName[m_currentWeaponPartsNameIndex])) {

		name = WeaponPartsName[m_currentWeaponPartsNameIndex];

		auto weaponParts = m_objectManager->GetObject(name);
		auto weaponPartsTransform = weaponParts->GetComponent<WIS::TransformComponent>();
		DirectX::SimpleMath::Vector3 pos = weaponPartsTransform->GetPosition();
		
		if (m_RightButton->IsClick()) {
			pos.x += 0.1f;
			weaponPartsTransform->SetPosition(pos);
		}
		if (m_LeftButton->IsClick()) {
			pos.x -= 0.1f;
			weaponPartsTransform->SetPosition(pos);
		}
	}


	if (m_SelectButton->IsClick()) {
		
	}

	

	if (key_state.Space) {
		this->DetachStackScene();
	}


	//if (m_UpButton->IsClick()) {
	//	auto weapon = m_objectManager->GetObject(m_weaponPartsText + std::to_string(clickCount));
	//	auto weaponTransform = weapon->GetComponent<WIS::TransformComponent>();
	//	DirectX::SimpleMath::Vector3 pos = weaponTransform->GetPosition();
	//	pos.y += 0.1f;
	//	weaponTransform->SetPosition(pos);
	//}
	//if (m_DownButton->IsClick()) {
	//	auto weapon = m_objectManager->GetObject(m_weaponPartsText + std::to_string(clickCount));
	//	auto weaponTransform = weapon->GetComponent<WIS::TransformComponent>();
	//	DirectX::SimpleMath::Vector3 pos = weaponTransform->GetPosition();
	//	pos.y -= 0.1f;
	//	weaponTransform->SetPosition(pos);
	//}









}

void WeaponCreateScene::Render()
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

	m_button->Render();
	m_BombButton->Render();
	//m_UpButton->Render();
	//m_DownButton->Render();
	m_RightButton->Render();
	m_LeftButton->Render();
	m_SelectButton->Render();
	m_objectManager->Render(
		GetUserResources(),
		*p_cameraManager->GetView(),
		m_proj
	);


#ifdef _DEBUG
	font->AddString((L"clickCount:" + std::to_wstring(clickCount)).c_str(), { 10.0f, 90.0f });
	font->AddString((L"isClick:" + std::to_wstring(isClick)).c_str(), { 10.0f, 110.0f });
	font->AddString((L"
		
		WeaponPartsNameIndex:" + std::to_wstring(m_currentWeaponPartsNameIndex)).c_str(), { 10.0f,130.0f });
	
#endif // _DEBUG
}

void WeaponCreateScene::Finalize()
{

	p_cameraManager->RequestCameraByHandle(m_ReturnCameraHandle);
	SetCurrentSceneTag(WIS::SceneTag::GAMEPLAY_SCENE);
	m_objectManager->SetCurrentSceneTag(WIS::SceneTag::GAMEPLAY_SCENE);

	auto Player = m_objectManager->GetObject(m_playerText);
	auto PlayerTransform= Player->GetComponent<WIS::TransformComponent>();
	auto Weapon = m_objectManager->GetObject(m_weaponText);
	auto weaponTransform = Weapon->GetComponent<WIS::TransformComponent>();

	weaponTransform->SetParent(PlayerTransform);
	weaponTransform->SetPosition(WeaponPos);
	weaponTransform->SetWorld(WeaponMat);
	////残しておくゲームオブジェクトを除いて削除
	//std::vector<std::shared_ptr<WIS::GameObject>> holdObjects;

	//for (std::string name : WeaponPartsName) {
	//	auto obj = m_objectManager->GetObject(name);
	//	if (obj) {
	//		holdObjects.push_back(obj);
	//	}
	//}
	//auto weapon = m_objectManager->GetObject(m_weaponText);
	//if (weapon)
	//{
	//holdObjects.push_back(weapon);
	//}

	//m_weaponArchetype.HoaldingObjects(m_objectManager);
	//m_objectManager->Finalize();
	//m_weaponArchetype.SetHoldingObject(m_objectManager);
	//for (auto& obj : holdObjects) {
	//	m_objectManager->AddObject(obj);
	//}

}

void WeaponCreateScene::CreateDeviceDependentResources()
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

void WeaponCreateScene::CreateWindowSizeDependentResources()
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

	RECT rect = deviceResources->GetOutputSize();


	m_button = std::make_unique<WIS::Button>();
	m_button->Initialize(GetUserResources()->GetDeviceResources(), rect.right, rect.bottom);
	m_button->Add(L"Resources/Textures/Bullet.png",
		{ 100, 100 },
		{ 1.0f, 1.0f },
		kHorikawa::ANCHOR::TOP_LEFT);

	m_BombButton = std::make_unique<WIS::Button>();
	m_BombButton->Initialize(GetUserResources()->GetDeviceResources(), rect.right, rect.bottom);
	m_BombButton->Add(L"Resources/Textures/Bullet.png",
		{ 300, 100 },
		{ 1.0f, 1.0f },
		kHorikawa::ANCHOR::TOP_LEFT);

	m_UpButton = std::make_unique<WIS::Button>();
	m_UpButton->Initialize(GetUserResources()->GetDeviceResources(), rect.right, rect.bottom);
	m_UpButton->Add(L"Resources/Textures/Bullet.png",
		{ 1000, 300 },
		{ 1.0f, 1.0f },
		kHorikawa::ANCHOR::TOP_LEFT);
	m_DownButton = std::make_unique<WIS::Button>();
	m_DownButton->Initialize(GetUserResources()->GetDeviceResources(), rect.right, rect.bottom);
	m_DownButton->Add(L"Resources/Textures/Bullet.png",
		{ 1000, 500 },
		{ 1.0f, 1.0f },



		kHorikawa::ANCHOR::TOP_LEFT);
	m_RightButton = std::make_unique<WIS::Button>();
	m_RightButton->Initialize(GetUserResources()->GetDeviceResources(), rect.right, rect.bottom);
	m_RightButton->Add(L"Resources/Textures/Bullet.png",
		{1000, 500 },
		{ 1.0f, 1.0f },
		kHorikawa::ANCHOR::TOP_LEFT);
	m_LeftButton = std::make_unique<WIS::Button>();
	m_LeftButton->Initialize(GetUserResources()->GetDeviceResources(), rect.right, rect.bottom);
	m_LeftButton->Add(L"Resources/Textures/Bullet.png",
		{ 800, 500 },
		{ 1.0f, 1.0f },
		kHorikawa::ANCHOR::TOP_LEFT);



	m_SelectButton = std::make_unique<WIS::Button>();
	m_SelectButton->Initialize(GetUserResources()->GetDeviceResources(), rect.right, rect.bottom);
	m_SelectButton->Add(L"Resources/Textures/Bullet.png",
		{ 100, 500 },
		{ 1.0f, 1.0f },
		kHorikawa::ANCHOR::TOP_LEFT);






}

void WeaponCreateScene::OnDeviceLost()
{
}

void WeaponCreateScene::ChangeNextWeaponParts()
{
	//WeaponPartsNameがあることが前提
	if (WeaponPartsName.size() > 0) {


		//Indexを加算する
		m_currentWeaponPartsNameIndex++;


		//Sizeを超えた場合は0に戻す
		m_currentWeaponPartsNameIndex = (m_currentWeaponPartsNameIndex > WeaponPartsName.size()-1) ? 0: m_currentWeaponPartsNameIndex;


	}
}

