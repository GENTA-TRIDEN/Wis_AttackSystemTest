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


#include "Scene/AttackScene.h"
#include "Scene/WeaponComponentTestScene.h"

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
	p_cameraManager->AddCamera({ 00.0f, 5.0f, 0.0f }, WIS::CameraMode::MODE_FIXED, m_cameraHandle);
	p_cameraManager->RequestFirstCameraInType(WIS::CameraMode::MODE_FIXED);
	p_cameraManager->GetCamera(m_cameraHandle)->SetPosition({ 0.0f, 0.0f, 0.0f });

	SetCurrentSceneTag(WIS::SceneTag::GAMEPLAY_SCENE);

	DirectX::Mouse::Get().SetMode(DirectX::Mouse::Mode::MODE_RELATIVE);
	DirectX::Mouse::Get().SetVisible(true);	
	//GameObjectの生成
	auto fxFactory = GetUserResources()->GetEffectFactory();



	m_button->Initialize(GetUserResources()->GetDeviceResources(), 1280, 720);



	//オブジェクトの生成
	m_objectManager->GenerateObject(m_weaponText, WIS::SceneTag::GAMEPLAY_SCENE);


	//Weaponの生成
	auto weapon = m_objectManager->GetObject(m_weaponText);
	m_weaponArchetype.AddGameObjectName(m_weaponText);
	auto weaponTransform = weapon->GetComponent<WIS::TransformComponent>();
	auto weaponColl = weapon->AddComponent<WIS::CollisionComponent>();
	auto weaponModel = weapon->AddComponent<WIS::ModelComponent>();
	auto WeaponComponent = weapon->AddComponent<WIS::WeaponComponent>();
	weaponColl->AddBoundingOrientedBox({ 2.7f,0.2f,0.2f }, { 3.0f,0.0f,0.0f });
	weaponColl->SetDebugColor(DirectX::Colors::Red);
	//we
	weaponTransform->SetPosition({ -4.0f, 0.0f, 0.0f });
	DirectX::SimpleMath::Quaternion rot = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, 0.0f, 0.0f);
	weaponTransform->SetRotation(rot);
	int Handle = weaponModel->AddModel(device, context, L"Resources/Models/Weapon/stick.sdkmesh", fxFactory);
	weaponModel->SetRotate(Handle, { 0,0,DirectX::XMConvertToRadians(-90) });
	weaponModel->SetScale(Handle, { 2.3f,2.3f,2.3f });
	
	m_button->SetTexture(WIS::Button::STATE::Normal, L"Resources/Textures/GearSilhouette.png");
}

void WeaponCreateScene::Update(float elapsedTime)
{

	DirectX::Mouse::Get().SetMode(DirectX::Mouse::Mode::MODE_ABSOLUTE);

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
	m_UpButton->Update();
	m_DownButton->Update();
	m_RightButton->Update();
	m_LeftButton->Update();


	if (m_button->IsClick() && isClick == false) {
		clickCount++;
		m_objectManager->GenerateObject(m_weaponPartsText + std::to_string(clickCount), WIS::SceneTag::GAMEPLAY_SCENE);
		m_weaponArchetype.AddGameObjectName(m_weaponPartsText + std::to_string(clickCount));
		auto device = GetUserResources()->GetDeviceResources()->GetD3DDevice();
		auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();
		auto fxFactory = GetUserResources()->GetEffectFactory();

		auto Weapon = m_objectManager->GetObject(m_weaponText);
		auto WeaponWeaponComponent = Weapon->GetComponent<WIS::WeaponComponent>();

		auto weaponParts = m_objectManager->GetObject(m_weaponPartsText + std::to_string(clickCount));
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

	}
	if (m_button->IsClick()) {
		
		isClick = true;
	}
	else {
		isClick = false;
	}
	//WeaponPartsの移動
	auto weaponParts = m_objectManager->GetObject(m_weaponPartsText + std::to_string(clickCount));
	if (weaponParts) {
		WeaponPartsName.push_back(weaponParts->GetName());
		auto weaponPartsTransform = weaponParts->GetComponent<WIS::TransformComponent>();
		DirectX::SimpleMath::Vector3 pos = weaponPartsTransform->GetPosition();
		if (m_RightButton->IsClick()) {
			pos.x -= 0.1f;
			weaponPartsTransform->SetPosition(pos);
		}
		if (m_LeftButton->IsClick()) {
			pos.x += 0.1f;
			weaponPartsTransform->SetPosition(pos);
		}
	}

	

	if (key_state.Space) {
		this->ChangeScene<WeaponComponentTestScene>();
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
	m_UpButton->Render();
	m_DownButton->Render();
	m_RightButton->Render();
	m_LeftButton->Render();

	m_objectManager->Render(
		GetUserResources(),
		*p_cameraManager->GetView(),
		m_proj
	);


	font->AddString((L"clickCount:" + std::to_wstring(clickCount)).c_str(), { 10.0f, 90.0f });
	font->AddString((L"isClick:" + std::to_wstring(isClick)).c_str(), { 10.0f, 100.0f });
	
}

void WeaponCreateScene::Finalize()
{

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

	m_weaponArchetype.HoaldingObjects(m_objectManager);
	m_objectManager->Finalize();
	m_weaponArchetype.SetHoldingObject(m_objectManager);
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
	//m_UpButton;
	//m_DownButton;
	//m_RightButton;
	//m_LeftButton;

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
		{800, 500 },
		{ 1.0f, 1.0f },
		kHorikawa::ANCHOR::TOP_LEFT);
	m_LeftButton = std::make_unique<WIS::Button>();
	m_LeftButton->Initialize(GetUserResources()->GetDeviceResources(), rect.right, rect.bottom);
	m_LeftButton->Add(L"Resources/Textures/Bullet.png",
		{ 1200, 500 },
		{ 1.0f, 1.0f },
		kHorikawa::ANCHOR::TOP_LEFT);




}

void WeaponCreateScene::OnDeviceLost()
{
}

