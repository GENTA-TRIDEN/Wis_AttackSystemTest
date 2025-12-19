#include "pch.h"
#include "SkyDomeScene.h"


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




SkyDomeScene::SkyDomeScene()
{
}

SkyDomeScene::~SkyDomeScene()
{
}

void SkyDomeScene::Initialize()
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
	m_objectManager->GetObject(m_weaponName)->GetComponent<WIS::WeaponComponent>()->SetEffect(m_effect.get());




	WIS::FloorArchetype floorArchetype = WIS::FloorArchetype();
	auto floor = floorArchetype.Create(m_objectManager, GetUserResources(), m_floorText, m_FloorModelHandle, m_PlayerText);
	std::string floorName = floor.lock()->GetName();


	WIS::EnemyArchetype enemyArchetype = WIS::EnemyArchetype();
	m_EnemyText = enemyArchetype.Create(m_objectManager, GetUserResources(), m_EnemyText, m_EnemyModelHandle, floorName).lock()->GetName();

	//スカイドームの初期化

	auto SkyTransform = m_skyDome->AddComponent<WIS::TransformComponent>();
	auto Skymodel=m_skyDome->AddComponent<WIS::ModelComponent>();



	int index=Skymodel->AddModel(device, context, L"Resources/Models/sky1.sdkmesh", fxFactory);
	Skymodel->SetTextureFromFile(device, index, L"Resources/Models/sky.dds");
	// テクスチャリソースを作成 (UserResourcesなどで管理している想定)
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> myTexture;
	//DirectX::CreateDDSTextureFromFile(device, L"Resources/Models/sky.dds", nullptr, myTexture.GetAddressOf());

	//Skymodel->SetTexture(index, myTexture.Get());
	Skymodel->SetDepthStencilState(GetUserResources()->GetCommonStates()->DepthRead());

	m_skyDome->Start(GetUserResources());



	m_basicEffect->SetLightingEnabled(true);
	m_basicEffect->SetPerPixelLighting(true);

	m_basicEffect->EnableDefaultLighting();
	m_basicEffect->SetAmbientLightColor(DirectX::SimpleMath::Vector3(2.5f, 2.5f, 2.5f));

}

void SkyDomeScene::Update(float elapsedTime)
{
	auto key_state = DirectX::Keyboard::Get().GetState();
	auto mouse_state = DirectX::Mouse::Get().GetState();


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
		Stacking = true;
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

	DirectX::SimpleMath::Vector3 pos = p_cameraManager->GetCamera()->GetPosition();
	pos.y += 0.2;
	m_skyDome->GetComponent<WIS::TransformComponent>()->SetPosition(pos);
	//m_skyDome->GetComponent<WIS::TransformComponent>()->SetPosition({0,5,0});





}

void SkyDomeScene::Render()
{
	auto deviceResources = GetUserResources()->GetDeviceResources();
	auto device = deviceResources->GetD3DDevice();
	auto context = deviceResources->GetD3DDeviceContext();

	// 現在のバックバッファ（画面）と深度バッファを保存しておく
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
					lights->SetLightEnabled(1, false);
					lights->SetLightEnabled(2, false);
					lights->SetAmbientLightColor(DirectX::Colors::White);
				}
				auto basicEffect = dynamic_cast<DirectX::BasicEffect*>(effect);
				basicEffect->SetTextureEnabled(true);
				//if (basicEffect)
				//{
				//	basicEffect->SetEmissiveColor(DirectX::Colors::White);
				//}

			}
		);
		m_skyDome->OnRender(GetUserResources(), *p_cameraManager->GetView(), m_proj);

		context->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}
	else {

	}




	m_objectManager->Render(GetUserResources(), *p_cameraManager->GetView(), m_proj);








	// 描画先を本来のバックバッファ（画面）に戻す
	context->OMSetRenderTargets(1, &oldRTV, oldDSV);

	m_spriteBatch->Begin();

	//表示する際の色を条件によって変える
	DirectX::XMVECTOR outputColor;


	//Stack中ははいけいを暗くする
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

	int enemyHP = 0;
	auto enemy = m_objectManager->GetObject(m_EnemyText);
	if (enemy)
	{
		auto enemyCharacter = enemy->GetComponent<WIS::CharacterComponent>();
		enemyHP = enemyCharacter->GetHP();
		font->AddString((L"enemyHP:" + std::to_wstring(enemyHP)).c_str(), { 10.0f, 90.0f });
	}

	auto cam = p_cameraManager->GetCamera();


	m_effect->CreateBillboard(cam->GetDirection(), cam->GetPosition(), DirectX::SimpleMath::Vector3::Up);
	m_effect->Render(*p_cameraManager->GetView(),
		m_proj);

	int PlayerHP = 0;
	auto player = m_objectManager->GetObject(m_PlayerText);
	// playerが存在するか確認（念のため）
	if (player) {
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

		auto PlayerComp = player->GetComponent<WIS::PlayerComponent>();
		if (PlayerComp) {
			//ItemKindsの中身を表示
			font->AddString((L"GEAR Count:" + std::to_wstring(PlayerComp->GetItemCount(WIS::ItemKinds::GEAR))).c_str(), { 10.0f, 300.0f });
		}
	}

	font->AddString((L"DebugCount:" + std::to_wstring(m_DebugCountUp)).c_str(), { 10.0f, 200.0f });
	font->AddString((L"m_WeaponAttackTime:" + std::to_wstring(m_WeaponAttackTime)).c_str(), { 10.0f, 250.0f });

	font->AddString(L"GamePlayScene(SkyDomeScene):", { 0.0f, 0.0f });

	// シェーダーリソースの解除 (次のフレームの書き込み競合防止)
	// ※SpriteBatch::End()内で解除されていることが多いですが、念のため行うと安全です
	ID3D11ShaderResourceView* nullSRV = nullptr;
	context->PSSetShaderResources(0, 1, &nullSRV);

	// 参照カウンタの解放 (GetRenderTargetsで増えた分)
	if (oldRTV) oldRTV->Release();
	if (oldDSV) oldDSV->Release();
}


void SkyDomeScene::Finalize()
{
	m_objectManager->Finalize();

	p_cameraManager->RequestFirstCamera();
	m_skyDome = nullptr;

}

void SkyDomeScene::CreateDeviceDependentResources()
{
	using namespace DirectX;
	auto device = GetUserResources()->GetDeviceResources()->GetD3DDevice();
	auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();
	m_basicEffect = std::make_unique<DirectX::BasicEffect>(device);


	void const* shaderByteCode;
	size_t byteCodeLength;

	m_basicEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	//ライティング計算を有効にする
	m_basicEffect->SetLightingEnabled(true);
	m_basicEffect->SetPerPixelLighting(true);

	m_basicEffect->EnableDefaultLighting();
	m_basicEffect->SetAmbientLightColor(SimpleMath::Vector3(5.1f, 5.1f,5.1f));
	


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


	m_skyDome= std::make_shared<WIS::GameObject>();

}

void SkyDomeScene::CreateWindowSizeDependentResources()
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

	// RTV (書き込み用)
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = textureDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	device->CreateRenderTargetView(m_renderTargetTexture.Get(), &rtvDesc, m_renderTargetView.ReleaseAndGetAddressOf());

	// SRV (読み込み用)
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	device->CreateShaderResourceView(m_renderTargetTexture.Get(), &srvDesc, m_shaderResourceView.ReleaseAndGetAddressOf());
	device->CreateShaderResourceView(m_renderTargetTexture.Get(), &srvDesc, m_SAVEshaderResourceView.ReleaseAndGetAddressOf());

	// 2. 深度バッファの作成 (これがないと3Dが正しく重ならない)
	D3D11_TEXTURE2D_DESC depthDesc = textureDesc;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // 一般的な深度フォーマット
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	device->CreateTexture2D(&depthDesc, nullptr, m_depthStencilTexture.ReleaseAndGetAddressOf());

	device->CreateDepthStencilView(m_depthStencilTexture.Get(), nullptr, m_depthStencilView.ReleaseAndGetAddressOf());



}

void SkyDomeScene::OnDeviceLost()
{
}

void SkyDomeScene::Attack()
{
}

std::string SkyDomeScene::ItemGenerate(DirectX::SimpleMath::Vector3 Pos, WIS::ItemKinds kinds)
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

