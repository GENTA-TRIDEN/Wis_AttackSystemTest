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


// シーン切り替え先のインクルード
#include "Scene/AttackScene.h"
#include "Scene/WeaponComponentTestScene.h"
#include "Scene/PlayerTestScene.h"
#include "Scene/ItemUsingScene.h"
#include "Scene/ItemGetScene.h"

//
// ユーザーインターフェース要素のインクルード
#include "WISLib/UserInterface/Button.h"
#include "WISLib/UserInterface/UserInterface.h"


using namespace DirectX;

// コンストラクタ
WeaponCreateScene::WeaponCreateScene()
{
}

// デストラクタ
WeaponCreateScene::~WeaponCreateScene()
{
}

/**
 * @brief シーンの初期化処理
 * * オブジェクトマネージャー、カメラ、UI、初期ゲームオブジェクトの設定を行います。
 */
void WeaponCreateScene::Initialize()
{
	// 乱数のシードを初期化
	srand(static_cast<unsigned int>(time(NULL)));

	// リソース依存の初期化（DirectX関連のシェーダー、入力レイアウトなど）
	CreateDeviceDependentResources();
	// ウィンドウサイズ依存の初期化（プロジェクション行列、UIボタンの初期化と配置など）
	CreateWindowSizeDependentResources();

	auto device = GetUserResources()->GetDeviceResources()->GetD3DDevice();
	auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();
	// C4189警告抑制のため、使用しないローカル変数に(void)キャストを適用 (device, context)
	(void)device;
	(void)context;

	// --- マネージャーの初期化 ---
	// ObjectManagerを取得し、シーン内で利用できるように保持
	m_objectManager = GetUserResources()->GetObjectManager();
	//p_blueprintManager = GetUserResources()->GetBlueprintManager(); // コメントアウトされていますが、ブループリント機能の使用が想定されます

	// カメラマネージャーのシングルトンインスタンスを取得
	p_cameraManager = WIS::CameraManager::GetInstance();
	// カメラを追加し、ハンドルを取得
	p_cameraManager->AddCamera({ 00.0f, 5.0f, 0.0f }, WIS::CameraMode::MODE_FIXED, m_cameraHandle);
	// 固定カメラタイプの中から最初のカメラを現在のカメラとして設定
	p_cameraManager->RequestFirstCameraInType(WIS::CameraMode::MODE_FIXED);
	// カメラ位置をワールド原点に設定
	p_cameraManager->GetCamera(m_cameraHandle)->SetPosition({ 0.0f, 0.0f, 0.0f });

	// このシーンのタグをSTACK_SCENEに設定
	SetCurrentSceneTag(WIS::SceneTag::STACK_SCENE);
	// ObjectManagerに現在のシーンタグを設定 (PreUpdate/PostUpdateでのフィルタリングに使用)
	m_objectManager->SetCurrentSceneTag(WIS::SceneTag::STACK_SCENE);


	// マウスモードを相対座標（ゲーム内での視点操作など）に設定し、カーソルを表示
	DirectX::Mouse::Get().SetMode(DirectX::Mouse::Mode::MODE_RELATIVE);
	DirectX::Mouse::Get().SetVisible(true);

	// エフェクトファクトリの取得
	auto fxFactory = GetUserResources()->GetEffectFactory();


	// UIボタン（m_button）の初期化とテクスチャ設定
	m_button->Initialize(GetUserResources()->GetDeviceResources(), 1280, 720);
	m_button->SetTexture(WIS::Button::STATE::Normal, L"Resources/Textures/GearSilhouette.png");


	// 初期武器オブジェクトの生成または取得
	if (!m_objectManager->GetObject(m_weaponText)) {

		// オブジェクトマネージャーに基本武器オブジェクトを生成
		m_objectManager->GenerateObject(m_weaponText, WIS::SceneTag::GAMEPLAY_SCENE);
		// STACK_SCENEタグを追加（このシーンでの更新・描画対象とする）
		m_objectManager->AddObjectSceneTag(m_weaponText, WIS::SceneTag::STACK_SCENE);

		// 修正: AddObjectメソッドはArchetypeBaseの設計上存在しないため削除
		// m_weaponArchetype.AddObject(m_weaponText); 

		// コンポーネントの追加と設定
		auto weapon = m_objectManager->GetObject(m_weaponText);
		auto weaponTransform = weapon->GetComponent<WIS::TransformComponent>();
		auto weaponColl = weapon->AddComponent<WIS::CollisionComponent>();
		auto weaponModel = weapon->AddComponent<WIS::ModelComponent>();
		auto WeaponComponent = weapon->AddComponent<WIS::WeaponComponent>();

		// 衝突判定ボックスの設定
		weaponColl->AddBoundingOrientedBox({ 2.7f,0.2f,0.2f }, { 3.0f,0.0f,0.0f });
		weaponColl->SetDebugColor(DirectX::Colors::Red);

		// 武器の位置と回転を設定
		weaponTransform->SetPosition({ -4.0f, 0.0f, 0.0f });
		DirectX::SimpleMath::Quaternion rot = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, 0.0f, 0.0f);
		weaponTransform->SetRotation(rot);

		// モデルの読み込みと設定（棒状のモデル "stick.sdkmesh"）
		int Handle = weaponModel->AddModel(device, context, L"Resources/Models/Weapon/stick.sdkmesh", fxFactory);
		weaponModel->SetRotate(Handle, { 0,0,DirectX::XMConvertToRadians(-90) });
		weaponModel->SetScale(Handle, { 2.3f,2.3f,2.3f });
	}
	else {
		// オブジェクトが既に存在する場合、位置と角度を調整する（シーンに戻ってきた場合などを想定）
		auto weapon = m_objectManager->GetObject(m_weaponText);
		auto weaponTransform = weapon->GetComponent<WIS::TransformComponent>();
		auto weaponModel = weapon->GetComponent<WIS::ModelComponent>();
		weaponTransform->SetPosition({ -4.0f, 0.0f, 0.0f });

		DirectX::SimpleMath::Quaternion rot = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, 0.0f, 0.0f);
		weaponTransform->SetRotation(rot);
	}
}

/**
 * @brief シーンの更新処理
 * * 入力処理、オブジェクトマネージャーの更新、UIボタンの処理、武器パーツの生成・移動処理を行います。
 * * @param elapsedTime 前フレームからの経過時間
 */
void WeaponCreateScene::Update(float elapsedTime)
{

	// マウスモードを絶対座標（UI操作など）に設定
	DirectX::Mouse::Get().SetMode(DirectX::Mouse::Mode::MODE_ABSOLUTE);

	// キーボードとマウスの状態を取得
	auto key_state = DirectX::Keyboard::Get().GetState();
	auto mouse_state = DirectX::Mouse::Get().GetState();

	// デバッグ用カウンタの更新
	m_DebugCountUp++;
	if (m_DebugCountMax <= m_DebugCountUp)
	{
		m_DebugCountUp = 0;
	}

	// オブジェクトマネージャーの更新処理を呼び出す
	m_objectManager->Update(
		elapsedTime,
		GetUserResources()
	);

	// UIボタンの更新処理
	m_button->Update();
	m_UpButton->Update();
	m_DownButton->Update();
	m_RightButton->Update();
	m_LeftButton->Update();

	// 武器パーツの生成処理（m_buttonがクリックされたとき）
	if (m_button->IsClick() && isClick == false) {
		clickCount++;

		// 武器パーツに一意な名前を生成するロジック
		std::string name = m_weaponPartsText + std::to_string(WeaponPartsName.size());

		// 生成された名前が既存のリストと重複しないかチェックし、重複する場合はインクリメント
		while (true) {
			bool isExist = false;
			for (const auto& existingName : WeaponPartsName) {
				if (existingName == name) {
					isExist = true;
					break;
				}
			}
			if (!isExist) {
				break; // 名前が重複していなければループを抜ける
			}
			// 新しい名前を生成
			name = m_weaponPartsText + std::to_string(WeaponPartsName.size() + 1);
		}

		// 新しい武器パーツの名前をリストに追加
		WeaponPartsName.push_back(name);

		// オブジェクトの生成とシーンタグの追加
		m_objectManager->GenerateObject(name, WIS::SceneTag::GAMEPLAY_SCENE);
		m_objectManager->AddObjectSceneTag(name, WIS::SceneTag::STACK_SCENE);

		// 修正: AddObjectメソッドはArchetypeBaseの設計上存在しないため削除
		// m_weaponArchetype.AddObject(name); 

		// リソースを取得
		auto device = GetUserResources()->GetDeviceResources()->GetD3DDevice();
		auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();
		auto fxFactory = GetUserResources()->GetEffectFactory();

		// 親オブジェクト（基本武器）を取得
		auto Weapon = m_objectManager->GetObject(m_weaponText);
		auto WeaponWeaponComponent = Weapon->GetComponent<WIS::WeaponComponent>();

		// 武器パーツオブジェクトを取得し、コンポーネントを追加・設定
		auto weaponParts = m_objectManager->GetObject(name);
		auto weaponPartsTransform = weaponParts->GetComponent<WIS::TransformComponent>();
		auto weaponPartsColl = weaponParts->AddComponent<WIS::CollisionComponent>();
		auto weaponModel = weaponParts->AddComponent<WIS::ModelComponent>();

		// 武器パーツコンポーネントを追加し、親武器を設定
		auto WeaponPartsComp = weaponParts->AddComponent<WIS::WeaponPartsComponent>(Weapon);
		// パートのステータスコンポーネントを追加
		auto WeaponPartsStatsComp = weaponParts->AddComponent<WIS::PartStatsComponent>();

		// 衝突判定ボックスの設定
		weaponPartsColl->AddBoundingOrientedBox({ 0.5f,0.5f,0.5f }, { 0.0f,0.0f,0.0f });

		// トランスフォームコンポーネントに親を設定（基本武器にアタッチ）
		weaponPartsTransform->SetParent(m_objectManager->GetObject(m_weaponText)->GetComponent<WIS::TransformComponent>());
		// 親からの相対位置を設定
		weaponPartsTransform->SetPosition({ 3.0f, 0.0f, 0.0f });
		// 回転を設定
		DirectX::SimpleMath::Quaternion rot = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, 0.0f, 0.0f);
		weaponPartsTransform->SetRotation(rot);

		// モデルの読み込み（歯車モデル "Gear.sdkmesh"）
		int Handle = weaponModel->AddModel(device, context, L"Resources/Models/Weapon/Gear.sdkmesh", fxFactory);

		// 親武器の重みに加算
		WeaponWeaponComponent->AddWeight(10.0f);

	}

	// クリック状態の更新（連打防止用？）
	if (m_button->IsClick()) {
		isClick = true;
	}
	else {
		isClick = false;
	}

	// 武器パーツの移動処理（最後のパーツのみ）
	if (WeaponPartsName.size() != 0) {

		// 最新の武器パーツオブジェクトを取得
		std::shared_ptr<WIS::GameObject> weaponParts = m_objectManager->GetObject(WeaponPartsName.back());
		if (weaponParts) {
			auto weaponPartsTransform = weaponParts->GetComponent<WIS::TransformComponent>();
			DirectX::SimpleMath::Vector3 pos = weaponPartsTransform->GetPosition();

			// UIボタンによるパーツの相対位置の移動
			if (m_RightButton->IsClick()) {
				pos.x -= 0.1f; // X軸の移動方向はコードの座標系に依存
				weaponPartsTransform->SetPosition(pos);
			}
			if (m_LeftButton->IsClick()) {
				pos.x += 0.1f; // X軸の移動方向はコードの座標系に依存
				weaponPartsTransform->SetPosition(pos);
			}

		}

	}


	// スペースキーが押されたらシーンを ItemUsingScene に変更
	if (key_state.Space) {
		this->ChangeScene<ItemUsingScene>();
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

/**
 * @brief シーンの描画処理
 * * グリッド描画、UI描画、オブジェクトマネージャーによるゲームオブジェクトの描画を行います。
 */
void WeaponCreateScene::Render()
{
	auto device = GetUserResources()->GetDeviceResources()->GetD3DDevice();
	auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();
	auto font = GetUserResources()->GetDebugFont();
	auto primitiveBatch = GetUserResources()->GetPrimitiveBatch();

	// ワールド、ビュー、プロジェクション行列の設定
	auto world = DirectX::SimpleMath::Matrix::Identity;
	m_basicEffect->SetWorld(world);
	m_basicEffect->SetView(*p_cameraManager->GetView());
	m_basicEffect->SetProjection(m_proj);

	// エフェクトの適用と入力レイアウトの設定
	m_basicEffect->Apply(context);
	context->IASetInputLayout(m_inputLayout.Get());

	// グリッド描画の開始
	primitiveBatch->Begin();

	// デバッグ用の地面グリッドの描画 (XZ平面)
	DX::DrawGrid(
		primitiveBatch,
		DirectX::XMVectorSet(10.0f, 0.0f, 0.0f, 0.0f), // X軸方向のベクトル
		DirectX::XMVectorSet(0.0f, 0.0f, 10.0f, 0.0f), // Z軸方向のベクトル
		DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), // 原点
		20, // X軸の分割数
		20, // Z軸の分割数
		DirectX::Colors::White
	);

	// グリッド描画の終了
	primitiveBatch->End();

	// UIボタンの描画
	m_button->Render();
	m_UpButton->Render();
	m_DownButton->Render();
	m_RightButton->Render();
	m_LeftButton->Render();

	// オブジェクトマネージャーに管理されているゲームオブジェクトの描画を依頼
	m_objectManager->Render(
		GetUserResources(),
		*p_cameraManager->GetView(),
		m_proj
	);

	// デバッグ情報の描画
	font->AddString((L"clickCount:" + std::to_wstring(clickCount)).c_str(), { 10.0f, 90.0f });
	font->AddString((L"isClick:" + std::to_wstring(isClick)).c_str(), { 10.0f, 100.0f });

}

/**
 * @brief シーンの終了処理
 * * シーン遷移に際し、次のシーンへ持ち越すオブジェクトの処理と、マネージャーの終了処理を行います。
 */
void WeaponCreateScene::Finalize()
{

	// シーン遷移時に残すべきオブジェクトをWeaponArchetypeに登録する
	m_weaponArchetype.HoaldingObjects(m_objectManager);

	// オブジェクトマネージャーの終了処理（通常、全てのオブジェクトの解放を含む）
	m_objectManager->Finalize();

	// m_weaponArchetypeに登録されたオブジェクト名を取得
	m_weaponArchetype.GetGameObjectNames();

	// 持ち越すオブジェクトをObjectManagerに再度追加する
	std::vector<std::string> names = *m_weaponArchetype.GetGameObjectNames();
	for (auto& obj : names) {
		// Finalizeで一度全て解放された（と仮定される）後、持ち越すオブジェクトを再登録している
		m_objectManager->AddObject(m_objectManager->GetObject(obj));
	}

}

/**
 * @brief デバイス依存のリソース（シェーダー、入力レイアウト）の作成
 */
void WeaponCreateScene::CreateDeviceDependentResources()
{
	using namespace DirectX;
	// C4189警告抑制のため、deviceの取得後すぐに使用
	auto device = GetUserResources()->GetDeviceResources()->GetD3DDevice();

	// BasicEffect (DirectX Tool Kit) の初期化
	m_basicEffect = std::make_unique<DirectX::BasicEffect>(device);


	void const* shaderByteCode;
	size_t byteCodeLength;

	// グリッド描画用のシェーダーバイトコードと入力レイアウトの作成
	m_basicEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);


	device->CreateInputLayout(
		VertexPositionColor::InputElements,
		VertexPositionColor::InputElementCount,
		shaderByteCode,
		byteCodeLength,
		m_inputLayout.ReleaseAndGetAddressOf()
	);
}

/**
 * @brief ウィンドウサイズ依存のリソース（プロジェクション行列、UI要素）の作成
 */
void WeaponCreateScene::CreateWindowSizeDependentResources()
{
	auto deviceResources = GetUserResources()->GetDeviceResources();
	auto size = deviceResources->GetOutputSize();

	// アスペクト比の計算
	float aspectRatio = float(size.right) / float(size.bottom);

	// パースペクティブプロジェクション行列の作成
	m_proj = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(
		DirectX::XMConvertToRadians(45.0f), // 視野角
		aspectRatio,
		0.1f, // near clip
		100.0f // far clip
	);

	RECT rect = deviceResources->GetOutputSize();

	// --- UIボタンの初期化と配置 ---
	// m_button (パーツ生成ボタン) の設定
	m_button = std::make_unique<WIS::Button>();
	m_button->Initialize(GetUserResources()->GetDeviceResources(), rect.right, rect.bottom);
	m_button->Add(L"Resources/Textures/Bullet.png",
		{ 100, 100 }, // ボタンの座標
		{ 1.0f, 1.0f }, // スケール
		kHorikawa::ANCHOR::TOP_LEFT); // アンカーポイント

	// m_UpButton (パーツ移動 上) の設定
	m_UpButton = std::make_unique<WIS::Button>();
	m_UpButton->Initialize(GetUserResources()->GetDeviceResources(), rect.right, rect.bottom);
	m_UpButton->Add(L"Resources/Textures/Bullet.png",
		{ 1000, 300 },
		{ 1.0f, 1.0f },
		kHorikawa::ANCHOR::TOP_LEFT);

	// m_DownButton (パーツ移動 下) の設定
	m_DownButton = std::make_unique<WIS::Button>();
	m_DownButton->Initialize(GetUserResources()->GetDeviceResources(), rect.right, rect.bottom);
	m_DownButton->Add(L"Resources/Textures/Bullet.png",
		{ 1000, 500 },
		{ 1.0f, 1.0f },
		kHorikawa::ANCHOR::TOP_LEFT);

	// m_RightButton (パーツ移動 右) の設定
	m_RightButton = std::make_unique<WIS::Button>();
	m_RightButton->Initialize(GetUserResources()->GetDeviceResources(), rect.right, rect.bottom);
	m_RightButton->Add(L"Resources/Textures/Bullet.png",
		{ 800, 500 },
		{ 1.0f, 1.0f },
		kHorikawa::ANCHOR::TOP_LEFT);

	// m_LeftButton (パーツ移動 左) の設定
	m_LeftButton = std::make_unique<WIS::Button>();
	m_LeftButton->Initialize(GetUserResources()->GetDeviceResources(), rect.right, rect.bottom);
	m_LeftButton->Add(L"Resources/Textures/Bullet.png",
		{ 1200, 500 },
		{ 1.0f, 1.0f },
		kHorikawa::ANCHOR::TOP_LEFT);
}

/**
 * @brief デバイスロスト時の処理
 * * デバイスが失われた（例：フルスクリーン切り替え、ドライバ更新など）際のリソース解放・再構築処理を記述する（現在は空）。
 */
void WeaponCreateScene::OnDeviceLost()
{
}