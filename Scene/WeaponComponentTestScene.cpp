#include "pch.h"
#include "WeaponComponentTestScene.h"


//component
#include "WISLib/GameObject/Component/TransformComponent.h"
#include "WISLib/GameObject/Component/CollisionComponent.h"
#include "WISLib/GameObject/Component/CharacterComponent.h"
#include "WISLib/GameObject/Component/WeaponComponent.h"

WeaponComponentTestScene::WeaponComponentTestScene()
{
}

WeaponComponentTestScene::~WeaponComponentTestScene()
{
}

void WeaponComponentTestScene::Initialize()
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
	int handle = p_cameraManager->AddCamera({ 0.0f, 0.0f, 0.0f }, WIS::CameraMode::MODE_FREEMOVE, m_cameraHandle);
	p_cameraManager->RequestCameraByHandle(handle);
	SetCurrentSceneTag(WIS::SceneTag::GAMEPLAY_SCENE);

	//GameObjectの生成
	m_objectManager->GenerateObject(m_PlayerText, WIS::SceneTag::GAMEPLAY_SCENE);
	m_objectManager->GenerateObject(m_EnemyText, WIS::SceneTag::GAMEPLAY_SCENE);

	//playerの設定
	auto player = m_objectManager->GetObject(m_PlayerText);
	auto playerColl = player->AddComponent<WIS::CollisionComponent>();
	playerColl->AddBoundingSphere(1.0f, { 0.0f,0.0f,0.0f });


	//enemyの設定
	auto enemy = m_objectManager->GetObject(m_EnemyText);
	auto enemyColl = enemy->AddComponent<WIS::CollisionComponent>();
	auto enemyCharacter = enemy->AddComponent<WIS::CharacterComponent>();
	enemyColl->AddBoundingOrientedBox({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f });
	enemyCharacter->SetObjectType(WIS::ObjectType::ENEMY);

	//enemyTransform
	auto enemyTransform = enemy->GetComponent<WIS::TransformComponent>();
	enemyTransform->SetPosition({ 0.0f,0.0f,5.0f });
	DirectX::SimpleMath::Quaternion rot = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(30.0f), 0.0f, 0.0f);
	enemyTransform->SetRotation(rot);

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

	auto Weapon = m_objectManager->GetObject("Weapon");
	auto WeaponTransform = Weapon->GetComponent<WIS::TransformComponent>();
	WeaponTransform->SetPosition({ 0.0f, 0.0f, 0.0f });



}

void WeaponComponentTestScene::Update(float elapsedTime)
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

	auto Weapon = m_objectManager->GetObject("Weapon");
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

void WeaponComponentTestScene::Render()
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
	font->AddString((L"PlayerAttackLength:" + std::to_wstring(m_PlayerAttackLength)).c_str(), { 10.0f, 120.0f });
	font->AddString((L"PlayerAttackLength:" + std::to_wstring(m_PlayerAttackLength)).c_str(), { 10.0f, 120.0f });

	font->AddString((L"DebugCount:" + std::to_wstring(m_DebugCountUp)).c_str(), { 10.0f, 200.0f });
	font->AddString((L"m_WeaponAttackTime:" + std::to_wstring(m_WeaponAttackTime)).c_str(), { 10.0f, 250.0f });


	font->AddString(L"WeaponComponentTestScene:", { 0.0f, 0.0f });


}

void WeaponComponentTestScene::Finalize()
{
	m_objectManager->Finalize();
}

void WeaponComponentTestScene::CreateDeviceDependentResources()
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

void WeaponComponentTestScene::CreateWindowSizeDependentResources()
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

void WeaponComponentTestScene::OnDeviceLost()
{
}

void WeaponComponentTestScene::Attack()
{
}

