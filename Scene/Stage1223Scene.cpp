#include "pch.h"
#include "Stage1223Scene.h"


//component
#include "WISLib/GameObject/Component/TransformComponent.h"
#include "WISLib/GameObject/Component/CollisionComponent.h"
#include "WISLib/GameObject/Component/CharacterComponent.h"
#include "WISLib/GameObject/Component/WeaponComponent.h"
#include "WISLib/GameObject/Component/ModelComponent.h"
#include "WISLib/GameObject/Component/RigidBodyComponent.h"
#include "WISLib/GameObject/Component/FloorComponent.h"
#include "WISLib/GameObject/Component/RigidBodyComponent.h"


#include "GameComponent/PlayerComponent.h"
#include "GameComponent/ItemComponent.h"
#include "GameComponent/EnemyComponent.h"


#include "InGameObject/Archetype/PlayerArchetype.h"
#include "InGameObject/Archetype/EnemyArchetype.h"
#include "InGameObject/Archetype/FloorArchetype.h"
#include "InGameObject/Archetype/WeapoArchetype.h"
#include "InGameObject/Archetype/PartsArchetype.h"

#include "Scene/WeaponCreateScene.h"
#include "Scene/TitleScene.h"

#include "Scene/ResultScene.h"




Stage1223Scene::Stage1223Scene()
{
}

Stage1223Scene::~Stage1223Scene()
{
}

void Stage1223Scene::Initialize()
{
	srand(static_cast<unsigned int>(time(NULL)));
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
	auto device = GetUserResources()->GetDeviceResources()->GetD3DDevice();
	auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();
	auto fxFactory = GetUserResources()->GetEffectFactory();

	fxFactory->SetDirectory(L"Resources/textures");
	m_enemyKill = 0;
	m_objectManager = GetUserResources()->GetObjectManager();
	p_cameraManager = WIS::CameraManager::GetInstance();
	int handle = p_cameraManager->AddCamera({ 0.0f, 2.0f, 0.0f }, WIS::CameraMode::MODE_TPSCAM, m_cameraHandle);
	p_cameraManager->RequestCameraByHandle(handle);
	SetCurrentSceneTag(WIS::SceneTag::GAMEPLAY_SCENE);
	m_objectManager->SetCurrentSceneTag(WIS::SceneTag::GAMEPLAY_SCENE);



	WIS::PlayerArchetype playerArchetype = WIS::PlayerArchetype();
	playerArchetype.Create(m_objectManager, GetUserResources(), m_PlayerText, m_PlayerModelHandle);



	m_PlayerAttackLength = 1.0f;


	auto player = m_objectManager->GetObject(m_PlayerText);
	auto PlayerTransform = player->GetComponent<WIS::TransformComponent>();
	auto playerRigid = player->GetComponent<WIS::RigidBodyComponent>();


	for (int i = 0; i < 5; i++) {

		ItemGenerate({ static_cast<float>(rand() % 10 - 5),2.0f,static_cast<float>(rand() % 10 - 5) }, WIS::ItemKinds::GEAR);
		//PartsArchetype
		WIS::PartsArchetype partsArchetype = WIS::PartsArchetype();
		partsArchetype.Create(m_objectManager,
			GetUserResources(),
			"Item_" + std::to_string(m_itemNames.size()),
			0, m_itemNames, WIS::ItemKinds::BOMB,
			DirectX::SimpleMath::Vector3{
			static_cast<float>(rand() % 10 - 5),
			2.0f,
			static_cast<float>(rand() % 10 - 5)
			}
		);


	}


	WIS::WeapoArchetype weaponArchetype = WIS::WeapoArchetype();
	m_weaponName = weaponArchetype.Create(m_objectManager, GetUserResources(), m_weaponName, m_WeaponModelHandle, m_PlayerText).lock()->GetName();
	m_effect->Create(GetUserResources()->GetDeviceResources());
	m_objectManager->GetObject(m_weaponName)->GetComponent<WIS::WeaponComponent>()->SetEffect(m_effect.get());

	if (player) {
		auto pComp = player->GetComponent<WIS::PlayerComponent>();
		if (pComp) {
			pComp->SetWeaponName(m_weaponName);
		}
	}


	WIS::FloorArchetype floorArchetype = WIS::FloorArchetype();
	auto floor = floorArchetype.Create(m_objectManager, GetUserResources(), m_floorText, m_FloorModelHandle, m_PlayerText);
	std::string floorName = floor.lock()->GetName();
	m_floorText = floorName;

	WIS::EnemyArchetype enemyArchetype = WIS::EnemyArchetype();
	m_EnemyText = enemyArchetype.Create(m_objectManager, GetUserResources(), m_EnemyText, m_EnemyModelHandle, floorName).lock()->GetName();

	auto enemy = m_objectManager->GetObject(m_EnemyText);
	auto enemyTransform = enemy->GetComponent<WIS::TransformComponent>();
	enemyTransform->SetPosition({ 5.0f,3.0f,5.0f });

	WIS::EnemyArchetype enemyArchetype2 = WIS::EnemyArchetype();
	m_Enemy2Text = enemyArchetype2.Create(m_objectManager, GetUserResources(), m_Enemy2Text, m_EnemyModelHandle, floorName).lock()->GetName();

	m_objectManager->GetObject(m_Enemy2Text)->GetComponent<WIS::TransformComponent>()->SetPosition({ 2,3,8 });
	WIS::EnemyArchetype enemyArchetype3 = WIS::EnemyArchetype();
	m_Enemy3Text = enemyArchetype3.Create(m_objectManager, GetUserResources(), m_Enemy3Text, m_EnemyModelHandle, floorName).lock()->GetName();

	m_objectManager->GetObject(m_Enemy3Text)->GetComponent<WIS::TransformComponent>()->SetPosition({ -2,3,-8 });

	WIS::EnemyArchetype enemyArchetype4 = WIS::EnemyArchetype();
	m_Enemy4Text = enemyArchetype4.Create(m_objectManager, GetUserResources(), m_Enemy4Text, m_EnemyModelHandle, floorName).lock()->GetName();
	m_objectManager->GetObject(m_Enemy4Text)->GetComponent<WIS::TransformComponent>()->SetPosition({ 5,3,8 });





	auto SkyTransform = m_skyDome->AddComponent<WIS::TransformComponent>();
	auto Skymodel = m_skyDome->AddComponent<WIS::ModelComponent>();



	int index = Skymodel->AddModel(device, context, L"Resources/Models/sky1.sdkmesh", fxFactory);
	Skymodel->SetTextureFromFile(device, index, L"Resources/Models/sky.dds");
	Skymodel->SetDepthStencilState(GetUserResources()->GetCommonStates()->DepthRead());

	auto model = Skymodel->GetModel(index);

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> myTexture;

	m_skyDome->Start(GetUserResources());



	m_basicEffect->SetLightingEnabled(true);
	m_basicEffect->SetPerPixelLighting(true);

	m_basicEffect->EnableDefaultLighting();
	m_basicEffect->SetAmbientLightColor(DirectX::SimpleMath::Vector3(2.5f, 2.5f, 2.5f));






}

void Stage1223Scene::Update(float elapsedTime)
{
	auto key_state = DirectX::Keyboard::Get().GetState();
	auto key = GetUserResources()->GetKeyboardStateTracker();
	
	auto mouse_state = DirectX::Mouse::Get().GetState();


	if (key->released.P) {
		WIS::CollisionManager::SetAllCollisionDebugDraw(!WIS::CollisionManager::GetAllCollisionDebugDraw());
	}

	m_objectManager->Update(
		elapsedTime,
		GetUserResources()
	);
	m_effect->Update(elapsedTime);
	m_DebugCountUp++;
	if (m_DebugCountMax <= m_DebugCountUp)
	{
		m_DebugCountUp = 0;
	}


	if (key_state.Tab) {
		this->StackScene<WeaponCreateScene>();
		Stacking = true;
	}

	auto floor = m_objectManager->GetObject(m_floorText)->GetComponent<WIS::FloorComponent>();

	auto player = m_objectManager->GetObject(m_PlayerText);
	auto playerTranc = player->GetComponent<WIS::TransformComponent>();
	auto playerCharacter = player->GetComponent<WIS::CharacterComponent>();
	auto enemy = m_objectManager->GetObject(m_EnemyText);
	auto enemy2 = m_objectManager->GetObject(m_Enemy2Text);
	auto enemy3 = m_objectManager->GetObject(m_Enemy3Text);
	auto enemy4 = m_objectManager->GetObject(m_Enemy4Text);

	float groundY = floor->GetHeight(playerTranc->GetPosition());

	if (!enemy && !enemy2 && !enemy3 && !enemy4) {
		ChangeScene<ResultScene>();
	}
	playerCharacter->SetFloorHeight(floor->GetHeight(playerTranc->GetPosition()));

	if (groundY > -1000.0f)
	{

		playerCharacter->SetFloorHeight(groundY);

	}
	else
	{

	}

	if (enemy) {
		auto enemyEnemy = enemy->GetComponent<WIS::EnemyComponent>();
		auto enemyCharacter = enemy->GetComponent<WIS::CharacterComponent>();
		enemyEnemy->SetPlayerPos(playerTranc->GetPosition());

		float enemyGroundY = floor->GetHeight(enemy->GetComponent<WIS::TransformComponent>()->GetPosition());
		if (enemyGroundY > -1000.0f)
		{
			enemyCharacter->SetFloorHeight(enemyGroundY);
		}
		else
		{
		}
	}
	if (enemy2) {
		auto enemyEnemy = enemy2->GetComponent<WIS::EnemyComponent>();
		auto enemyCharacter = enemy2->GetComponent<WIS::CharacterComponent>();
		enemyEnemy->SetPlayerPos(playerTranc->GetPosition());

		float enemyGroundY = floor->GetHeight(enemy2->GetComponent<WIS::TransformComponent>()->GetPosition());
		if (enemyGroundY > -1000.0f)
		{
			enemyCharacter->SetFloorHeight(enemyGroundY);
		}
		else
		{
		}
	}

	DirectX::SimpleMath::Vector3 pos = p_cameraManager->GetCamera()->GetPosition();
	pos.y += 0.2;
	m_skyDome->GetComponent<WIS::TransformComponent>()->SetPosition(pos);


}

void Stage1223Scene::Render()
{
	auto deviceResources = GetUserResources()->GetDeviceResources();
	auto device = deviceResources->GetD3DDevice();
	auto context = deviceResources->GetD3DDeviceContext();

	ID3D11RenderTargetView* oldRTV = nullptr;
	ID3D11DepthStencilView* oldDSV = nullptr;
	context->OMGetRenderTargets(1, &oldRTV, &oldDSV);


	if (!this->IsStack()) {

		context->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());


		float clearColor[4] = { 0.0f, 0.1f, 0.2f, 1.0f };
		context->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);

		context->RSSetState(GetUserResources()->GetCommonStates()->CullNone());


		m_skyDome->GetComponent<WIS::ModelComponent>()->GetModel(0)->UpdateEffects([](DirectX::IEffect* effect)
			{
				auto lights = dynamic_cast<DirectX::IEffectLights*>(effect);
				if (lights)
				{
					lights->SetLightEnabled(0, true);
					lights->SetLightEnabled(1, true);
					lights->SetLightEnabled(2, true);

					// ベクトルは「光が進む方向」です（光源の位置ではありません）

					auto dir0 = DirectX::SimpleMath::Vector3(-1.0f, -1.0f, 0.0f);
					dir0.Normalize(); // 長さを1にする（重要）
					lights->SetLightDirection(0, dir0);
					lights->SetAmbientLightColor(DirectX::Colors::DarkGray);


					auto dir1 = DirectX::SimpleMath::Vector3(1.0f, -1.0f, 1.0f);
					dir1.Normalize();
					lights->SetLightDirection(1, dir1);
					lights->SetAmbientLightColor(DirectX::Colors::DarkGray);


					auto dir2 = DirectX::SimpleMath::Vector3(-1.0f, -1.0f, -1.0f);
					dir2.Normalize();
					lights->SetLightDirection(1, dir2);
					lights->SetAmbientLightColor(DirectX::Colors::Blue);


					// 環境光（全体的な明るさの底上げ）の設定
					lights->SetAmbientLightColor(DirectX::Colors::DarkGray);
				}
				auto basicEffect = dynamic_cast<DirectX::BasicEffect*>(effect);
				basicEffect->SetTextureEnabled(true);
			}

		);
		m_skyDome->OnRender(GetUserResources(), *p_cameraManager->GetView(), m_proj);

		context->ClearDepthStencilView(
			m_depthStencilView.Get(),
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
			1.0f,
			0
		);
	}
	else {

	}




	m_objectManager->Render(GetUserResources(), *p_cameraManager->GetView(), m_proj);
	m_effect->CreateBillboard(
		p_cameraManager->GetCamera()->GetDirection(),
		p_cameraManager->GetCamera()->GetPosition(),
		DirectX::SimpleMath::Vector3::Up
	);
	m_effect->Render(
		*p_cameraManager->GetView(),
		m_proj
	);

	auto playerCharacter = m_objectManager->GetObject(m_PlayerText)->GetComponent<WIS::CharacterComponent>();

	playerCharacter->DrawShadow(
		context,
		GetUserResources()->GetCommonStates(),
		m_objectManager->GetObject(m_PlayerText)->GetComponent<WIS::TransformComponent>()->GetPosition(),
		2.0f,
		*p_cameraManager->GetView(),
		m_proj
	);


	context->OMSetRenderTargets(1, &oldRTV, oldDSV);

	m_spriteBatch->Begin();

	DirectX::XMVECTOR outputColor;


	if (!this->IsStack()) {
		outputColor = DirectX::Colors::White;
		m_spriteBatch->Draw(m_shaderResourceView.Get(), DirectX::XMFLOAT2(0, 0), nullptr);
	}
	else {
		outputColor = DirectX::Colors::Gray;
		m_spriteBatch->Draw(m_shaderResourceView.Get(), DirectX::XMFLOAT2(0, 0), nullptr, outputColor);
	}



	m_spriteBatch->End();

	auto font = GetUserResources()->GetDebugFont();


#ifdef _DEBUG

	auto floor = m_objectManager->GetObject(m_floorText)->GetComponent<WIS::FloorComponent>();
	int enemyHP = 0;
	auto enemy = m_objectManager->GetObject(m_EnemyText);
	if (enemy)
	{
		auto enemyCharacter = enemy->GetComponent<WIS::CharacterComponent>();
		enemyHP = enemyCharacter->GetHP();
		font->
			((L"てきHP:" + std::to_wstring(enemyHP)).c_str(), { 10.0f, 90.0f });
	}

	auto cam = p_cameraManager->GetCamera();


	int PlayerHP = 0;
	auto player = m_objectManager->GetObject(m_PlayerText);
	if (player) {
		auto playerTrans = player->GetComponent<WIS::TransformComponent>();
		auto playerCharacter = player->GetComponent<WIS::CharacterComponent>();
		PlayerHP = playerCharacter->GetHP();
		DirectX::SimpleMath::Vector3 pos = playerTrans->GetPosition();

		font->AddString((L"PlayerPosX:" + std::to_wstring(pos.x)).c_str(), { 150.0f, 350.0f });
		font->AddString((L"PlayerPosY:" + std::to_wstring(pos.y)).c_str(), { 150.0f, 380.0f });
		font->AddString((L"PlayerPosZ:" + std::to_wstring(pos.z)).c_str(), { 150.0f, 410.0f });
		font->AddString((L"FloorHeight:" + std::to_wstring(floor->GetHeight(playerTrans->GetPosition()))).c_str(), { 150.0f, 440.0f });


		font->AddString((L"PlayerHP:" + std::to_wstring(PlayerHP)).c_str(), { 10.0f, 50.0f });
		font->AddString((L"PlayerAttackLength:" + std::to_wstring(m_PlayerAttackLength)).c_str(), { 10.0f, 120.0f });

		auto PlayerComp = player->GetComponent<WIS::PlayerComponent>();
		if (PlayerComp) {
			font->AddString((L"GEAR Count:" + std::to_wstring(PlayerComp->GetItemCount(WIS::ItemKinds::GEAR))).c_str(), { 10.0f, 300.0f });
		}
	}

	if (enemy) {
		auto enemyTrans = enemy->GetComponent<WIS::TransformComponent>();
		DirectX::SimpleMath::Vector3 epos = enemyTrans->GetPosition();
		font->AddString((L"EnemyPosX:" + std::to_wstring(epos.x)).c_str(), { 150.0f, 450.0f });
		font->AddString((L"EnemyPosY:" + std::to_wstring(epos.y)).c_str(), { 150.0f, 480.0f });
		font->AddString((L"EnemyPosZ:" + std::to_wstring(epos.z)).c_str(), { 150.0f, 510.0f });
		font->AddString((L"EnemyFloorHeight:" + std::to_wstring(floor->GetHeight(enemyTrans->GetPosition()))).c_str(), { 150.0f, 540.0f });

	}

	font->AddString((L"DebugCount:" + std::to_wstring(m_DebugCountUp)).c_str(), { 10.0f, 200.0f });

	font->AddString(L"GamePlayScene(Stage1223Scene):", { 0.0f, 0.0f });

#endif // _DEBUG
	ID3D11ShaderResourceView* nullSRV = nullptr;
	context->PSSetShaderResources(0, 1, &nullSRV);


	if (!this->IsStack()) {


		//黒色
		auto player = m_objectManager->GetObject(m_PlayerText);
		auto PlayerComp = player->GetComponent<WIS::PlayerComponent>();
		auto PlayerChar = player->GetComponent<WIS::CharacterComponent>();

		m_BOMBSilhouette->Render();

		m_GEARSilhouette->Render();

		m_HPRed->Render();

		float a = (static_cast<float>(PlayerChar->GetHP()) / static_cast<float>(PlayerChar->GetMaxHP()));

		m_HPGreen->SetScale({a ,1.0f });

		m_HPGreen->Render();

		if (PlayerComp) 
		{
			//黒で
			font->AddString((std::to_wstring(PlayerComp->GetItemCount(WIS::ItemKinds::BOMB))).c_str(), { 200.0f, 21.0f }, DirectX::Colors::Black, 2.0f);
			font->AddString((std::to_wstring(PlayerComp->GetItemCount(WIS::ItemKinds::GEAR))).c_str(), { 200.0f, 90.0f }, DirectX::Colors::Black, 2.0f);
		}

	}

	if (oldRTV) oldRTV->Release();
	if (oldDSV) oldDSV->Release();

}


void Stage1223Scene::Finalize()
{
	m_objectManager->Finalize();

	p_cameraManager->RequestFirstCamera();
	m_skyDome = nullptr;

}

void Stage1223Scene::CreateDeviceDependentResources()
{

	using namespace DirectX;
	auto device = GetUserResources()->GetDeviceResources()->GetD3DDevice();
	auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();
	m_basicEffect = std::make_unique<DirectX::BasicEffect>(device);


	void const* shaderByteCode;
	size_t byteCodeLength;

	m_basicEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	m_basicEffect->SetLightingEnabled(true);
	m_basicEffect->SetPerPixelLighting(true);

	m_basicEffect->EnableDefaultLighting();
	m_basicEffect->SetAmbientLightColor(SimpleMath::Vector3(0.9f, 0.9f, 0.9f));



	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);


	device->CreateInputLayout(
		VertexPositionColor::InputElements,
		VertexPositionColor::InputElementCount,
		shaderByteCode,
		byteCodeLength,
		m_inputLayout.ReleaseAndGetAddressOf()
	);

	m_effect = std::make_unique<WIS::HitEffect>();
	m_effect->Create(GetUserResources()->GetDeviceResources());


	m_skyDome = std::make_shared<WIS::GameObject>();

}

void Stage1223Scene::CreateWindowSizeDependentResources()
{
	auto deviceResources = GetUserResources()->GetDeviceResources();
	auto device = GetUserResources()->GetDeviceResources()->GetD3DDevice();
	auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();

	auto size = deviceResources->GetOutputSize();
	float aspectRatio = float(size.right) / float(size.bottom);
	m_proj = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(
		DirectX::XMConvertToRadians(45.0f),
		aspectRatio,
		0.1f,
		1000.0f
	);




	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = size.right;
	textureDesc.Height = size.bottom;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	device->CreateTexture2D(&textureDesc, nullptr, m_renderTargetTexture.ReleaseAndGetAddressOf());

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = textureDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	device->CreateRenderTargetView(m_renderTargetTexture.Get(), &rtvDesc, m_renderTargetView.ReleaseAndGetAddressOf());

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	device->CreateShaderResourceView(m_renderTargetTexture.Get(), &srvDesc, m_shaderResourceView.ReleaseAndGetAddressOf());
	device->CreateShaderResourceView(m_renderTargetTexture.Get(), &srvDesc, m_SAVEshaderResourceView.ReleaseAndGetAddressOf());

	D3D11_TEXTURE2D_DESC depthDesc = textureDesc;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	device->CreateTexture2D(&depthDesc, nullptr, m_depthStencilTexture.ReleaseAndGetAddressOf());

	device->CreateDepthStencilView(m_depthStencilTexture.Get(), nullptr, m_depthStencilView.ReleaseAndGetAddressOf());


	m_BOMBSilhouette = std::make_unique<kHorikawa::UserInterface>();
	m_GEARSilhouette = std::make_unique<kHorikawa::UserInterface>();



	m_BOMBSilhouette->Create(GetUserResources()->GetDeviceResources()
		, L"Resources/Textures/GEARNum.png"
		, { 50.0f,10.0f }
		, { 1.5f,1.5f }
	, kHorikawa::ANCHOR::TOP_LEFT);

	m_GEARSilhouette->Create(GetUserResources()->GetDeviceResources()
		, L"Resources/Textures/BOMBNum.png"
		, { 50.0f,90.0f }
		, { 1.5f,1.5f }
	, kHorikawa::ANCHOR::TOP_LEFT);
	m_BOMBSilhouette->SetWindowSize(size.right, size.bottom);
	m_GEARSilhouette->SetWindowSize(size.right, size.bottom);

	//m_HPGreen
	m_HPGreen = std::make_unique<kHorikawa::UserInterface>();
	m_HPGreen->Create(GetUserResources()->GetDeviceResources()
		, L"Resources/Textures/GF_HPGage.png"
		, { 20.0f, size.bottom - 60.0f }
		, { 1.0f,1.0f }
	, kHorikawa::ANCHOR::TOP_LEFT);
	m_HPGreen->SetWindowSize(size.right, size.bottom);


	//m_HPRed
	m_HPRed = std::make_unique<kHorikawa::UserInterface>();
	m_HPRed->Create(GetUserResources()->GetDeviceResources()
		, L"Resources/Textures/GF_HPGageBackground.png"
		, { 20.0f, size.bottom - 60.0f }
		, { 1.0f,1.0f }
	, kHorikawa::ANCHOR::TOP_LEFT);
	m_HPRed->SetWindowSize(size.right, size.bottom);


}

void Stage1223Scene::OnDeviceLost()
{
}

void Stage1223Scene::Attack()
{
}

std::string Stage1223Scene::ItemGenerate(DirectX::SimpleMath::Vector3 Pos, WIS::ItemKinds kinds)
{
	std::string name = "Item_" + std::to_string(m_itemNames.size());
	m_itemNames.push_back(name);
	auto item = m_objectManager->GenerateObject(name, WIS::SceneTag::GAMEPLAY_SCENE);

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
	//itemModel->SetColor(Handle, { 2.0f / 255.0f,2.0f / 255.0f,2.0f / 255.0f,1.0f });
	itemModel->SetScale(Handle, { 0.8f,0.8f,0.8f });
	return name;
}