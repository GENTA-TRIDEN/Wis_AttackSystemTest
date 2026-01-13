//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

// Scene
#include "Scene/AttackScene.h"
#include "Scene/WeaponTestScene.h"
#include "Scene/StackTestMain.h"
#include "Scene/WeaponCreateScene.h"
#include "Scene/ItemGetScene.h"
#include "Scene/EnemyScene.h"
#include "Scene/TitleScene.h"
#include "Scene/RigidBodyScene.h"

#include "Scene/RenderTargetViewScene.h"

// WISlib (Manager)
#include "WISLib/Camera/CameraManager.h"

#include "WISLib/GameObject/Component/CollisionComponent.h"

#include "InGameObject/ItemRequestManager.h"


extern void ExitGame() noexcept;

using namespace DirectX;
using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false) :
    p_cameraManager(nullptr),
    m_pCollisionManager(nullptr) // Initialize pointers with nullptr 
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_mouse = std::make_unique<DirectX::Mouse>();
    m_keyboard = std::make_unique<DirectX::Keyboard>();
	m_gamePad = std::make_unique<DirectX::GamePad>(); 
    m_deviceResources->RegisterDeviceNotify(this);
}

Game::~Game()
{
    // Release singleton instances
    if (m_pCollisionManager)
    {
        m_pCollisionManager = nullptr;
    }
    if (p_cameraManager)
    {
        p_cameraManager = nullptr;
    }
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_deviceResources->SetWindow(window, width, height);
    m_mouse->SetWindow(window);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

    // Get singleton manager instances
    p_cameraManager = WIS::CameraManager::GetInstance();
    m_pCollisionManager = WIS::CollisionManager::GetInstance();

    m_sceneManager->SetUserResources(m_userResources.get());
    // Set the initial scene
    //m_sceneManager->SetScene<AttackScene>();
    //m_sceneManager->SetScene<WeaponTestScene>();
    //m_sceneManager->SetScene<StackTestMain>();
    //m_sceneManager->SetScene<RenderTargetViewScene>();
    //m_sceneManager->SetScene<ItemGetScene>();
    //m_sceneManager->SetScene<EnemyScene>();
    m_sceneManager->SetScene<TitleScene>();


}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
        {
            Update(m_timer);
        });

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    float elapsedTime = float(timer.GetElapsedSeconds());

    
    auto mouse_state = m_mouse->GetState();
    m_kbTracker.Update(m_keyboard->GetState());
    m_mouseTracker.Update(mouse_state);
	m_gamePadTracker.Update(m_gamePad->GetState(0));

    // Update managers
    p_cameraManager->Update(mouse_state);
    m_pCollisionManager->Update();
    //if (m_sceneManager->GetCurrentSceneTag() == WIS::SceneTag::GAMEPLAY_SCENE)
    //    m_pCollisionManager->Update();

    // Update the current scene
    m_sceneManager->Update(elapsedTime);

    WIS::ItemRequestManager::ProcessRequests(m_objectManager.get(), m_userResources.get());

}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    m_deviceResources->PIXBeginEvent(L"Render");

    p_cameraManager->Rander();
    

    // Render the current scene
    m_sceneManager->Render();


    // Render debug font
    m_debugFont->Render(m_commonStates.get());

    m_deviceResources->PIXEndEvent();

    m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    auto const viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
void Game::OnActivated() {}
void Game::OnDeactivated() {}
void Game::OnSuspending() {}
void Game::OnResuming() { m_timer.ResetElapsedTime(); }
void Game::OnWindowMoved() { auto const r = m_deviceResources->GetOutputSize(); m_deviceResources->WindowSizeChanged(r.right, r.bottom); }
void Game::OnDisplayChange() { m_deviceResources->UpdateColorSpace(); }
void Game::OnWindowSizeChanged(int width, int height) { if (!m_deviceResources->WindowSizeChanged(width, height)) return; CreateWindowSizeDependentResources(); }
void Game::GetDefaultSize(int& width, int& height) const noexcept { width = 1280; height = 720; }
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();
    auto context = m_deviceResources->GetD3DDeviceContext();

    m_commonStates = std::make_unique<CommonStates>(device);
    m_effectFactory = std::make_unique<DirectX::EffectFactory>(m_deviceResources->GetD3DDevice());
    m_primitiveBatch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>(context);

    m_debugFont = std::make_unique<Ito::DebugFont>(device, context, L"Resources\\Font\\DebugFont.spritefont");

    if (!m_userResources)
    {
        m_userResources = std::make_unique<UserResources>();
    }
    if (!m_sceneManager)
    {
        m_sceneManager = std::make_unique<Ito::SceneManager<UserResources>>(m_userResources.get());

    }

    // --- Create Manager Instances ---
    m_objectManager = std::make_unique<WIS::ObjectManager>();

    // --- Set User Resources ---
    m_userResources->SetDeviceResources(m_deviceResources.get());
    m_userResources->SetCommonStates(m_commonStates.get());
    m_userResources->SetDebugFont(m_debugFont.get());
    m_userResources->SetKeyboardStateTracker(&m_kbTracker);
    m_userResources->SetMouseStateTracker(&m_mouseTracker);
    m_userResources->SetMouse(m_mouse.get());
    m_userResources->SetStepTimerStates(&m_timer);
    m_userResources->SetEffectFactory(m_effectFactory.get());
    m_userResources->SetPrimitiveBatch(m_primitiveBatch.get());

    // --- Register managers with UserResources ---
    m_userResources->SetObjectManager(m_objectManager.get());
    // Call CreateDeviceDependentResources for the current scene
	m_userResources->SetCameraManager(WIS::CameraManager::GetInstance());

    m_sceneManager->SetUserResources(m_userResources.get());

}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    m_sceneManager->CreateWindowSizeDependentResources();
}

void Game::OnDeviceLost()
{
    m_sceneManager->OnDeviceLost();
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();
    CreateWindowSizeDependentResources();
}
#pragma endregion

