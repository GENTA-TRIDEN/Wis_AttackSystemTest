#include "pch.h"
#include "TitleScene.h"


//component
#include "WISLib/GameObject/Component/TransformComponent.h"
#include "WISLib/GameObject/Component/CollisionComponent.h"
#include "WISLib/GameObject/Component/CharacterComponent.h"


#include "Scene/EnemyScene.h"
#include "Scene/RigidBodyScene.h"
#include "Scene/RenderTargetViewScene.h"
#include "Scene/EffectScene.h"
#include "Scene/SkyDomeScene.h"

TitleScene::TitleScene()
{
}

TitleScene::~TitleScene()
{
}

void TitleScene::Initialize()
{
	// 乱数のシードを初期化
	srand(static_cast<unsigned int>(time(NULL)));
	//初期化
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
	auto device = GetUserResources()->GetDeviceResources()->GetD3DDevice();
	auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();

	// --- マネージャーの初期化 ---

	p_cameraManager = WIS::CameraManager::GetInstance();
	int Handle=0;
	p_cameraManager->AddCamera({ 00.0f, 5.0f, 0.0f }, WIS::CameraMode::MODE_FIXED, Handle);
	p_cameraManager->RequestCameraByHandle(Handle);
	p_cameraManager->GetCamera(Handle)->SetPosition({ 0.0f, 0.0f, 0.0f });


	
	m_button->Initialize(GetUserResources()->GetDeviceResources(), 1280, 720);



	m_button->SetTexture(WIS::Button::Normal, L"Resources/Textures/GameStart.png");
	m_button->SetTexture(WIS::Button::Hover, L"Resources/Textures/GameStartHover.png");
	m_button->SetTexture(WIS::Button::Push, L"Resources/Textures/PushGameStart.png");
	count = 0;
}

void TitleScene::Update(float elapsedTime)
{
	DirectX::Mouse::Get().SetMode(DirectX::Mouse::Mode::MODE_ABSOLUTE);
	DirectX::Mouse::Get().SetVisible(true);
	auto key_state = DirectX::Keyboard::Get().GetState();
	auto mouse_state = DirectX::Mouse::Get().GetState();

	count += elapsedTime;

	m_button->Update();

	if (m_button->IsClick() && count >= 1) {

		ChangeScene<SkyDomeScene>();

	}

}

void TitleScene::Render()
{

	auto device = GetUserResources()->GetDeviceResources()->GetD3DDevice();
	auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();
	auto font = GetUserResources()->GetDebugFont();
	auto primitiveBatch = GetUserResources()->GetPrimitiveBatch();
	auto world = DirectX::SimpleMath::Matrix::Identity;
	Background->Render();
	m_button->Render();

	font->AddString(L"TitleScene:", { 0.0f, 0.0f });


}

void TitleScene::Finalize()
{
}

void TitleScene::CreateDeviceDependentResources()
{
	using namespace DirectX;
	auto device = GetUserResources()->GetDeviceResources()->GetD3DDevice();

	m_basicEffect = std::make_unique<DirectX::BasicEffect>(device);


	void const* shaderByteCode;
	size_t byteCodeLength;

	m_basicEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);


	RECT rect = GetUserResources()->GetDeviceResources()->GetOutputSize();

	device->CreateInputLayout(
		VertexPositionColor::InputElements,
		VertexPositionColor::InputElementCount,
		shaderByteCode,
		byteCodeLength,
		m_inputLayout.ReleaseAndGetAddressOf()
	);

	m_button = std::make_unique<WIS::Button>();
	m_button->Initialize(GetUserResources()->GetDeviceResources(), rect.right, rect.bottom);
	m_button->Add(L"Resources/Textures/GameStart.png",
		{ (float)rect.right/2-260, (float)rect.bottom/2+200 },
		{ 1.0f, 1.0f },
		kHorikawa::ANCHOR::TOP_LEFT);

	Background = std::make_unique<kHorikawa::UserInterface>();


	Background->Create(GetUserResources()->GetDeviceResources()
		, L"Resources/Textures/TitleBackground.png"
		, { 0,0 }
		, { 0.667f,0.667f }
	, kHorikawa::ANCHOR::TOP_LEFT);
	Background->SetWindowSize(rect.right, rect.bottom);
}

void TitleScene::CreateWindowSizeDependentResources()
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

void TitleScene::OnDeviceLost()
{
}


