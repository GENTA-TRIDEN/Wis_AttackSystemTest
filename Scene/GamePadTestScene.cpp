#include "pch.h"
#include "GamePadTestScene.h"


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

#include <GamePad.h>

#include "Scene/ResultScene.h"




GamePadTestScene::GamePadTestScene()
{
}

GamePadTestScene::~GamePadTestScene()
{
}

void GamePadTestScene::Initialize()
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
	int handle = p_cameraManager->AddCamera({ 0.0f, 2.0f, 0.0f }, WIS::CameraMode::MODE_FREEMOVE, m_cameraHandle);
	p_cameraManager->RequestCameraByHandle(handle);
	SetCurrentSceneTag(WIS::SceneTag::GAMEPLAY_SCENE);
	m_objectManager->SetCurrentSceneTag(WIS::SceneTag::GAMEPLAY_SCENE);





}

void GamePadTestScene::Update(float elapsedTime)
{
	auto key_state = DirectX::Keyboard::Get().GetState();
	auto mouse_state = DirectX::Mouse::Get().GetState();
	auto gamepad_state = DirectX::GamePad::Get().GetState(0);
	auto font = GetUserResources()->GetDebugFont();

	gamepad_state.thumbSticks.leftX;
	gamepad_state.thumbSticks.leftY;

	gamepad_state.thumbSticks.rightX;
	gamepad_state.thumbSticks.rightY;

	font->AddString((L"LeftX:" + std::to_wstring(gamepad_state.thumbSticks.leftX)).c_str(), { 0.0f, 20.0f });
	font->AddString((L"LeftY:" + std::to_wstring(gamepad_state.thumbSticks.leftY)).c_str(), { 0.0f, 40.0f });
	font->AddString((L"RightX:" + std::to_wstring(gamepad_state.thumbSticks.rightX)).c_str(), { 0.0f, 60.0f });
	font->AddString((L"RightY:" + std::to_wstring(gamepad_state.thumbSticks.rightY)).c_str(), { 0.0f, 80.0f });


}

void GamePadTestScene::Render()
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

	auto cam = p_cameraManager->GetCamera();


	font->AddString(L"GamePlayScene(GamePadTestScene):", { 0.0f, 0.0f });


}

void GamePadTestScene::Finalize()
{
	m_objectManager->Finalize();

	p_cameraManager->RequestFirstCamera();

}

void GamePadTestScene::CreateDeviceDependentResources()
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

void GamePadTestScene::CreateWindowSizeDependentResources()
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

void GamePadTestScene::OnDeviceLost()
{
}



