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



	//m_button->Initialize(GetUserResources()->GetDeviceResources(), 1280, 720);
	//m_BombButton->Initialize(GetUserResources()->GetDeviceResources(), 1280, 720);
	////m_UpButton->Initialize(GetUserResources()->GetDeviceResources(), 1280, 720);
	////m_DownButton->Initialize(GetUserResources()->GetDeviceResources(), 1280, 720);
	//m_RightButton->Initialize(GetUserResources()->GetDeviceResources(), 1280, 720);
	//m_LeftButton->Initialize(GetUserResources()->GetDeviceResources(), 1280, 720);
	//m_SelectButton->Initialize(GetUserResources()->GetDeviceResources(), 1280, 720);

	for (auto& button : m_buttonList) {
		button->Initialize(GetUserResources()->GetDeviceResources(), 1280, 720);
	}




	//オブジェクトの生成
	auto player = m_objectManager->GetObject(m_playerText);
	if (player) {

	}

	//Weaponの生成
	auto weapon = m_objectManager->GetObject(m_weaponText);
	if (weapon){

		//m_weaponArchetype.AddGameObjectName(m_weaponText);
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
		weaponModel->SetDepthStencilState(GetUserResources()->GetCommonStates()->DepthRead());
		
	}
	
	//m_button->SetTexture(WIS::Button::STATE::Normal, L"Resources/Textures/GearSilhouette.png");
	//m_BombButton->SetTexture(WIS::Button::STATE::Normal, L"Resources/Textures/BombSilhouette.png");
	//m_LeftButton->SetTexture(WIS::Button::STATE::Normal, L"Resources/Textures/LeftArrowButton.png");
	//m_RightButton->SetTexture(WIS::Button::STATE::Normal, L"Resources/Textures/RightArrowButton.png");
	//m_SelectButton->SetTexture(WIS::Button::STATE::Normal, L"Resources/Textures/SelectButton.png");

	m_buttonList[Buttons::GEAR_BUTTON]->SetTexture(WIS::Button::STATE::Normal, L"Resources/Textures/GearSilhouette.png");
	m_buttonList[Buttons::BOMB_BUTTON]->SetTexture(WIS::Button::STATE::Normal, L"Resources/Textures/BombSilhouette.png");
	m_buttonList[Buttons::LEFT_BUTTON]->SetTexture(WIS::Button::STATE::Normal, L"Resources/Textures/LeftArrowButton.png");
	m_buttonList[Buttons::RIGHT_BUTTON]->SetTexture(WIS::Button::STATE::Normal, L"Resources/Textures/RightArrowButton.png");
	//m_buttonList[Buttons::UP_BUTTON]->SetTexture(WIS::Button::STATE::Normal, L"Resources/Textures/UpArrowButton.png");
	//m_buttonList[Buttons::DOWN_BUTTON]->SetTexture(WIS::Button::STATE::Normal, L"Resources/Textures/DownArrowButton.png");
	m_buttonList[Buttons::SELECT_BUTTON]->SetTexture(WIS::Button::STATE::Normal, L"Resources/Textures/SelectButton.png");


	m_buttonList[Buttons::GEAR_BUTTON]->SetTexture(WIS::Button::STATE::Hover, L"Resources/Textures/GearSilhouetteHover.png");
	m_buttonList[Buttons::BOMB_BUTTON]->SetTexture(WIS::Button::STATE::Hover, L"Resources/Textures/BombSilhouetteHover.png");
	m_buttonList[Buttons::LEFT_BUTTON]->SetTexture(WIS::Button::STATE::Hover, L"Resources/Textures/LeftArrowButtonHover.png");
	m_buttonList[Buttons::RIGHT_BUTTON]->SetTexture(WIS::Button::STATE::Hover, L"Resources/Textures/RightArrowButtonHover.png");
	//m_buttonList[Buttons::UP_BUTTON]->SetTexture(WIS::Button::STATE::Hover, L"Resources/Textures/UpArrowButtonHover.png");
	//m_buttonList[Buttons::DOWN_BUTTON]->SetTexture(WIS::Button::STATE::Hover, L"Resources/Textures/DownArrowButtonHover.png");
	m_buttonList[Buttons::SELECT_BUTTON]->SetTexture(WIS::Button::STATE::Hover, L"Resources/Textures/SelectButtonHover.png");



	m_currentWeaponPartsNameIndex = -1;
	m_SceneChangeTimer = 0.0f;
	//↓これを直したい
	m_buttonList[Buttons::SELECT_BUTTON]->SetOnCnClickCallbacks([this]() {
		this->ChangeNextWeaponParts();
		});
	m_currentButton = Buttons::GEAR_BUTTON;
}



void WeaponCreateScene::Update(float elapsedTime)
{

	DirectX::Mouse::Get().SetMode(DirectX::Mouse::Mode::MODE_ABSOLUTE);
	DirectX::Mouse::Get().SetVisible(true);


	m_SceneChangeTimer += elapsedTime;

	auto key_state = DirectX::Keyboard::Get().GetState();
	auto mouse_state = DirectX::Mouse::Get().GetState();

	auto gamePad = DirectX::GamePad::Get().GetState(0);
	m_GamePadState.Update(gamePad);
	m_DebugCountUp++;
	if (m_DebugCountMax <= m_DebugCountUp)
	{
		m_DebugCountUp = 0;
	}

	
	m_objectManager->Update(
		elapsedTime,
		GetUserResources()
	);
	m_buttonList[m_currentButton]->Select();


	for (auto& button : m_buttonList) {
		button->Update();
	}

	std::string name;

	//@TODOここら辺を関数にする
	if (m_buttonList[Buttons::GEAR_BUTTON]->IsClick() && isClick == false) {
		AddWeaponPartsToWeapon(L"Resources/Models/Weapon/Gear.sdkmesh", WIS::ItemKinds::GEAR);
	}

	//上記のBomb版
	if (m_buttonList[Buttons::BOMB_BUTTON]->IsClick() && isClick == false) {

		AddWeaponPartsToWeapon(L"Resources/Models/Weapon/Bomb.sdkmesh", WIS::ItemKinds::BOMB);

	}




	if (m_buttonList[Buttons::BOMB_BUTTON]->IsClick()) {
		
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
		
		//if (m_RightButton->IsClick()) {
		//	pos.x += 0.1f;
		//	weaponPartsTransform->SetPosition(pos);
		//}
		//if (m_LeftButton->IsClick()) {
		//	pos.x -= 0.1f;
		//	weaponPartsTransform->SetPosition(pos);
		//}
		if (m_buttonList[Buttons::RIGHT_BUTTON]->IsClick()) {
			pos.x += 0.1f;
			weaponPartsTransform->SetPosition(pos);
		}
		if (m_buttonList[Buttons::LEFT_BUTTON]->IsClick()) {
			pos.x -= 0.1f;
			weaponPartsTransform->SetPosition(pos);
		}

		
	}


	if (m_buttonList[Buttons::SELECT_BUTTON]->IsClick()) {
		ChangeNextWeaponParts();
	}



	

	if ((key_state.Tab|| gamePad.buttons.y)&& m_SceneChangeTimer>m_SceneChangeDelay){
		this->DetachStackScene();
	}

	if (m_GamePadState.dpadRight== m_GamePadState.RELEASED) {
		if (m_currentButton != Buttons::BUTTON_MAX - 1)
		{
			m_currentButton = static_cast<Buttons>(static_cast<int>(m_currentButton) + 1);
		}
		else
		{
			m_currentButton = Buttons::GEAR_BUTTON;
		}

	}

	if (m_GamePadState.dpadLeft == m_GamePadState.RELEASED) {

		if (m_currentButton != Buttons::GEAR_BUTTON)
		{
			m_currentButton = static_cast<Buttons>(static_cast<int>(m_currentButton) - 1);
		}
		else
		{
			m_currentButton = static_cast<Buttons>(Buttons::BUTTON_MAX - 1);
		}
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




	for (size_t i = 0; i < m_buttonList.size(); i++) {
		m_buttonList[i]->Render();
	}
	
	m_objectManager->Render(
		GetUserResources(),
		*p_cameraManager->GetView(),
		m_proj
	);


#ifdef _DEBUG

	font->AddString((L"clickCount:" + std::to_wstring(clickCount)).c_str(), { 10.0f, 90.0f });


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


	Weapon->GetComponent<WIS::ModelComponent>()->SetDepthStencilState(GetUserResources()->GetCommonStates()->DepthDefault());
	DirectX::Mouse::Get().SetVisible(false);

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

	//ButtonListの初期化

	m_buttonList.clear();
	for (size_t i = 0; i < 7; i++)
	{
		m_buttonList.push_back(std::make_unique<WIS::Button>());
	}



	m_buttonList[0]->Initialize(GetUserResources()->GetDeviceResources(), rect.right, rect.bottom);
	m_buttonList[0]->Add(L"Resources/Textures/Bullet.png",
		{ 100, 100 },
		{ 1.0f, 1.0f },
		kHorikawa::ANCHOR::TOP_LEFT);

	m_buttonList[1]->Initialize(GetUserResources()->GetDeviceResources(), rect.right, rect.bottom);
	m_buttonList[1]->Add(L"Resources/Textures/Bullet.png",
		{ 300, 100 },
		{ 1.0f, 1.0f },
		kHorikawa::ANCHOR::TOP_LEFT);

	m_buttonList[2]->Initialize(GetUserResources()->GetDeviceResources(), rect.right, rect.bottom);
	m_buttonList[2]->Add(L"Resources/Textures/Bullet.png",
		{ 1000, 300 },
		{ 1.0f, 1.0f },
		kHorikawa::ANCHOR::TOP_LEFT);
	m_buttonList[3]->Initialize(GetUserResources()->GetDeviceResources(), rect.right, rect.bottom);
	m_buttonList[3]->Add(L"Resources/Textures/Bullet.png",
		{ 1000, 500 },
		{ 1.0f, 1.0f },
		kHorikawa::ANCHOR::TOP_LEFT);
	m_buttonList[4]->Initialize(GetUserResources()->GetDeviceResources(), rect.right, rect.bottom);
	m_buttonList[4]->Add(L"Resources/Textures/Bullet.png",
		{ 1000, 500 },
		{ 1.0f, 1.0f },
		kHorikawa::ANCHOR::TOP_LEFT);
	m_buttonList[5]->Initialize(GetUserResources()->GetDeviceResources(), rect.right, rect.bottom);
	m_buttonList[5]->Add(L"Resources/Textures/Bullet.png",
		{ 800, 500 },
		{ 1.0f, 1.0f },
		kHorikawa::ANCHOR::TOP_LEFT);
	m_buttonList[6]->Initialize(GetUserResources()->GetDeviceResources(), rect.right, rect.bottom);
	m_buttonList[6]->Add(L"Resources/Textures/Bullet.png",
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

void WeaponCreateScene::AddWeaponPartsToWeapon(const wchar_t* path, WIS::ItemKinds itemKinds)
{

	std::string name;
	auto player = m_objectManager->GetObject(m_playerText)->GetComponent<WIS::PlayerComponent>();

	int itemCount = player->GetItemCount(itemKinds);
	//アイテム:Gearの所持数が0より大きい場合
	if (itemCount > 0) {

		//こんな名前の付け方をしないでもかぶらないようになってる
		clickCount++;

		//所持数を1減らし、プレイヤーコンポーネントに反映
		itemCount--;
		player->SetItemCount(itemKinds, itemCount);

		//WeaponPartsオブジェクトの生成
		auto weaponParts = m_objectManager->GenerateObject(m_weaponPartsText, WIS::SceneTag::STACK_SCENE);
		name = weaponParts->GetName();
		m_currentWeaponPartsNameIndex++;
		m_objectManager->AddObjectSceneTag(name, WIS::SceneTag::GAMEPLAY_SCENE);
		//m_weaponArchetype.AddGameObjectName(name);

		//使用するデバイス関連の取得
		auto device = GetUserResources()->GetDeviceResources()->GetD3DDevice();
		auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();
		auto fxFactory = GetUserResources()->GetEffectFactory();


		//---Weaponオブジェクトの取得---
		auto Weapon = m_objectManager->GetObject(m_weaponText);
		//Weaponコンポーネントの取得
		auto WeaponWeaponComponent = Weapon->GetComponent<WIS::WeaponComponent>();

		/*m_objectManager->GetObject(name);*/


		//WeaponPartsコンポーネント、Transformコンポーネント、Collisionコンポーネント、Modelコンポーネントの追加
		auto weaponPartsColl = weaponParts->AddComponent<WIS::CollisionComponent>();

		//Modelコンポーネントの追加
		auto weaponModel = weaponParts->AddComponent<WIS::ModelComponent>();

		//パーツコンポーネントの追加
		weaponParts->AddComponent<WIS::WeaponPartsComponent>(Weapon);
		weaponParts->AddComponent<WIS::PartStatsComponent>(itemKinds);


		auto weaponPartsTransform = weaponParts->GetComponent<WIS::TransformComponent>();



		//コリジョンをつける
		weaponPartsColl->AddBoundingOrientedBox({ 0.5f,0.5f,0.5f }, { 0.0f,0.0f,0.0f });



		weaponPartsTransform->SetParent(m_objectManager->GetObject(m_weaponText)->GetComponent<WIS::TransformComponent>());
		weaponPartsTransform->SetPosition({ 3.0f, 0.0f, 0.0f });
		DirectX::SimpleMath::Quaternion rot = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, 0.0f, 0.0f);
		weaponPartsTransform->SetRotation(rot);


		//モデルの追加
		int Handle = weaponModel->AddModel(device, context, path, fxFactory);
		weaponModel->SetDepthStencilState(GetUserResources()->GetCommonStates()->DepthNone());



		WeaponWeaponComponent->AddWeight(10.0f);
		WeaponWeaponComponent->AttachPart(weaponParts);


		WeaponPartsName.push_back(weaponParts->GetName());


	}

}





//auto player = m_objectManager->GetObject(m_playerText)->GetComponent<WIS::PlayerComponent>();
//int itemCount = player->GetItemCount(WIS::ItemKinds::BOMB);
//if (itemCount > 0) {
//	clickCount++;
//	itemCount--;
//	player->SetItemCount(WIS::ItemKinds::BOMB, itemCount);
//	auto weaponParts = m_objectManager->GenerateObject(m_weaponPartsText + std::to_string(clickCount), WIS::SceneTag::STACK_SCENE);
//	name = weaponParts->GetName();
//	m_currentWeaponPartsNameIndex++;
//	m_objectManager->AddObjectSceneTag(name, WIS::SceneTag::GAMEPLAY_SCENE);
//	//m_weaponArchetype.AddGameObjectName(name);
//	auto device = GetUserResources()->GetDeviceResources()->GetD3DDevice();
//	auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();
//	auto fxFactory = GetUserResources()->GetEffectFactory();
//	auto Weapon = m_objectManager->GetObject(m_weaponText);
//	auto WeaponWeaponComponent = Weapon->GetComponent<WIS::WeaponComponent>();
//	m_objectManager->GetObject(name);
//	auto weaponPartsTransform = weaponParts->GetComponent<WIS::TransformComponent>();
//	auto weaponPartsColl = weaponParts->AddComponent<WIS::CollisionComponent>();
//	auto weaponModel = weaponParts->AddComponent<WIS::ModelComponent>();
//	auto WeaponPartsComp = weaponParts->AddComponent<WIS::WeaponPartsComponent>(Weapon);
//	//constructorにWeaponKindsを追加してBombを設定
//	auto WeaponPartsStatsComp = weaponParts->AddComponent<WIS::PartStatsComponent>(WIS::ItemKinds::BOMB);
//	weaponPartsColl->AddBoundingOrientedBox({ 0.5f,0.5f,0.5f }, { 0.0f,0.0f,0.0f });
//	weaponPartsTransform->SetParent(m_objectManager->GetObject(m_weaponText)->GetComponent<WIS::TransformComponent>());
//	weaponPartsTransform->SetPosition({ 3.0f, 0.0f, 0.0f });
//	DirectX::SimpleMath::Quaternion rot = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, 0.0f, 0.0f);
//	weaponPartsTransform->SetRotation(rot);
//	int Handle = weaponModel->AddModel(device, context, L"Resources/Models/Weapon/Bomb.sdkmesh", fxFactory);
//	weaponModel->SetDepthStencilState(GetUserResources()->GetCommonStates()->DepthNone());
//	WeaponWeaponComponent->AddWeight(20.0f);
//	WeaponWeaponComponent->AttachPart(weaponParts);
//	WeaponPartsName.push_back(weaponParts->GetName());
//
//auto player = m_objectManager->GetObject(m_playerText)->GetComponent<WIS::PlayerComponent>();
//
//int itemCount = player->GetItemCount(WIS::ItemKinds::GEAR);
////アイテム:Gearの所持数が0より大きい場合
//if (itemCount > 0) {
//
//	//こんな名前の付け方をしないでもかぶらないようになってる
//	clickCount++;
//
//	//所持数を1減らし、プレイヤーコンポーネントに反映
//	itemCount--;
//	player->SetItemCount(WIS::ItemKinds::GEAR, itemCount);
//
//	//WeaponPartsオブジェクトの生成
//	auto weaponParts = m_objectManager->GenerateObject(m_weaponPartsText, WIS::SceneTag::STACK_SCENE);
//	name = weaponParts->GetName();
//	m_currentWeaponPartsNameIndex++;
//	m_objectManager->AddObjectSceneTag(name, WIS::SceneTag::GAMEPLAY_SCENE);
//	//m_weaponArchetype.AddGameObjectName(name);
//
//	//使用するデバイス関連の取得
//	auto device = GetUserResources()->GetDeviceResources()->GetD3DDevice();
//	auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();
//	auto fxFactory = GetUserResources()->GetEffectFactory();
//
//
//	//---Weaponオブジェクトの取得---
//	auto Weapon = m_objectManager->GetObject(m_weaponText);
//	//Weaponコンポーネントの取得
//	auto WeaponWeaponComponent = Weapon->GetComponent<WIS::WeaponComponent>();
//
//	/*m_objectManager->GetObject(name);*/
//
//
//	//WeaponPartsコンポーネント、Transformコンポーネント、Collisionコンポーネント、Modelコンポーネントの追加
//	auto weaponPartsColl = weaponParts->AddComponent<WIS::CollisionComponent>();
//
//	//Modelコンポーネントの追加
//	auto weaponModel = weaponParts->AddComponent<WIS::ModelComponent>();
//
//	//パーツコンポーネントの追加
//	weaponParts->AddComponent<WIS::WeaponPartsComponent>(Weapon);
//	weaponParts->AddComponent<WIS::PartStatsComponent>(WIS::ItemKinds::GEAR);
//
//
//	auto weaponPartsTransform = weaponParts->GetComponent<WIS::TransformComponent>();
//
//
//
//	//コリジョンをつける
//	weaponPartsColl->AddBoundingOrientedBox({ 0.5f,0.5f,0.5f }, { 0.0f,0.0f,0.0f });
//
//
//
//	weaponPartsTransform->SetParent(m_objectManager->GetObject(m_weaponText)->GetComponent<WIS::TransformComponent>());
//	weaponPartsTransform->SetPosition({ 3.0f, 0.0f, 0.0f });
//	DirectX::SimpleMath::Quaternion rot = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, 0.0f, 0.0f);
//	weaponPartsTransform->SetRotation(rot);
//
//
//	//モデルの追加
//	int Handle = weaponModel->AddModel(device, context, L"Resources/Models/Weapon/Gear.sdkmesh", fxFactory);
//	weaponModel->SetDepthStencilState(GetUserResources()->GetCommonStates()->DepthNone());
//
//
//
//	WeaponWeaponComponent->AddWeight(10.0f);
//	WeaponWeaponComponent->AttachPart(weaponParts);
//
//
//	WeaponPartsName.push_back(weaponParts->GetName());



