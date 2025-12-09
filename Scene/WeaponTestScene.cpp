#include "pch.h"
#include "WeaponTestScene.h"


//component
#include "WISLib/GameObject/Component/TransformComponent.h"
#include "WISLib/GameObject/Component/CollisionComponent.h"
#include "WISLib/GameObject/Component/CharacterComponent.h"
#include "WISLib/GameObject/Component/ModelComponent.h"

WeaponTestScene::WeaponTestScene()
{
}

WeaponTestScene::~WeaponTestScene()
{
}

void WeaponTestScene::Initialize()
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
	p_cameraManager->AddCamera({ 00.0f, 5.0f, 0.0f }, WIS::CameraMode::MODE_FREEMOVE, m_cameraHandle);
	SetCurrentSceneTag(WIS::SceneTag::GAMEPLAY_SCENE);

	//GameObjectの生成
	m_objectManager->GenerateObject(m_weaponName, WIS::SceneTag::GAMEPLAY_SCENE);
	m_objectManager->GenerateObject(m_weaponPartsText, WIS::SceneTag::GAMEPLAY_SCENE);

	m_objectManager->GenerateObject(m_EnemyText, WIS::SceneTag::GAMEPLAY_SCENE);

	auto fxFactory = GetUserResources()->GetEffectFactory();


	//enemyの設定
	auto enemy = m_objectManager->GetObject(m_EnemyText);
	auto enemyColl = enemy->AddComponent<WIS::CollisionComponent>();
	auto enemyCharacter = enemy->AddComponent<WIS::CharacterComponent>();
	enemyColl->AddBoundingOrientedBox({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f });
	//enemyTransform
	auto enemyTransform = enemy->GetComponent<WIS::TransformComponent>();
	enemyTransform->SetPosition({ 0.0f,0.0f,5.0f });
	DirectX::SimpleMath::Quaternion rot = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(30.0f), 0.0f, 0.0f);
	enemyTransform->SetRotation(rot);

	



	//Weaponの生成
	auto weapon = m_objectManager->GetObject(m_weaponName);
	auto weaponTransform = weapon->GetComponent<WIS::TransformComponent>();
	auto weaponColl = weapon->AddComponent<WIS::CollisionComponent>();
	auto weaponModel = weapon->AddComponent<WIS::ModelComponent>();
	weaponColl->AddBoundingOrientedBox({ 2.7f,0.2f,0.2f }, { 3.0f,0.0f,0.0f });
	weaponColl->SetDebugColor(DirectX::Colors::Red);
	//we
	weaponTransform->SetPosition({1.0f, 0.0f, 0.0f});
	 rot = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, 0.0f, 0.0f );
	weaponTransform->SetRotation(rot);
	int Handle=weaponModel->AddModel(device, context, L"Resources/Models/Weapon/stick.sdkmesh", fxFactory);
	weaponModel->SetRotate(Handle, { 0,0,DirectX::XMConvertToRadians(-90)});
	weaponModel->SetScale(Handle, { 2.3f,2.3f,2.3f });


	//m_weaponPartsTextの生成
	auto weaponParts = m_objectManager->GetObject(m_weaponPartsText);
	auto weaponPartsTransform = weaponParts->GetComponent<WIS::TransformComponent>();
	auto weaponPartsColl = weaponParts->AddComponent<WIS::CollisionComponent>();
	weaponPartsColl->AddBoundingOrientedBox({ 0.5f,0.5f,0.5f }, { 0.0f,0.0f,0.0f });
	weaponPartsTransform->SetParent(weaponTransform);
	weaponPartsTransform->SetPosition({ 3.0f, 0.0f, 0.0f });
	rot = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, 0.0f, 0.0f);
	weaponPartsTransform->SetRotation(rot);


		






}

void WeaponTestScene::Update(float elapsedTime)
{

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


	//Weaponの回転0,1,0の軸で回転
	auto weapon = m_objectManager->GetObject(m_weaponName);
	auto weaponTransform = weapon->GetComponent<WIS::TransformComponent>();
	DirectX::SimpleMath::Quaternion rot = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle({ 0.0f,1.0f,0.0f }, DirectX::XMConvertToRadians(1.0f) * elapsedTime * 60.0f);
	weaponTransform->SetRotation(rot * weaponTransform->GetRotation());

	////Weaponの回転をWeaponの原点を中心に回転させる
	//auto weapon = m_objectManager->GetObject(m_weaponName);
	//auto weaponTransform = weapon->GetComponent<WIS::TransformComponent>();
	//DirectX::SimpleMath::Vector3 weaponPos = weaponTransform->GetPosition();
	//	
	//DirectX::SimpleMath::Quaternion rot = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle({ 0.0f,1.0f,0.0f }, DirectX::XMConvertToRadians(1.0f) * elapsedTime * 60.0f);
	//weaponTransform->SetRotation(rot * weaponTransform->GetRotation());
	//DirectX::SimpleMath::Vector3 newPos = weaponTransform->GetPosition();
	//weaponTransform->SetPosition(weaponPos);



	






}

void WeaponTestScene::Render()
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
	auto enemyCharacter = enemy->GetComponent<WIS::CharacterComponent>();
	enemyHP = enemyCharacter->GetHP();
	//void DebugFont::AddString(const wchar_t* string, DirectX::SimpleMath::Vector2 pos, FXMVECTOR color, float scale)
	font->AddString((L"enemyHP:" + std::to_wstring(enemyHP)).c_str(), { 10.0f, 90.0f });
	font->AddString((L"DebugCount:" + std::to_wstring(m_DebugCountUp)).c_str(), { 10.0f, 120.0f });

	font->AddString(L"WeaponTestScene:", { 0.0f, 0.0f });

}

void WeaponTestScene::Finalize()
{
	m_objectManager->Finalize();
}

void WeaponTestScene::CreateDeviceDependentResources()
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

void WeaponTestScene::CreateWindowSizeDependentResources()
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

void WeaponTestScene::OnDeviceLost()
{
}

