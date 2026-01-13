//--------------------------------------------------------------------------------------
// File: UserResources.h
//
// シーンへ渡すユーザーリソースクラス（サンプル）
//
// Date: 2025.2.28
//--------------------------------------------------------------------------------------
#pragma once

#include "StepTimer.h"
#include "DeviceResources.h"
#include "ItoLib/DebugFont.h"

#include "WISLib/GameObject/ObjectManager.h"
//カメラマネージャー
#include "WISLib/Camera/CameraManager.h"

// クラスの前方宣言
namespace WIS {
	class ObjectManager;
	//class BlueprintManager;
}

// 各シーンに渡す共通リソースを記述してください
class UserResources
{
private:
	// ステップタイマー
	DX::StepTimer* m_timer;
	// デバイスリソース
	DX::DeviceResources* m_deviceResources;
	// キーボードステートトラッカー
	DirectX::Keyboard::KeyboardStateTracker* m_keyboardTracker;
	// マウスステートトラッカー
	DirectX::Mouse::ButtonStateTracker* m_mouseTracker;
	// マウス
	DirectX::Mouse* m_mouse;
	// デバッグ用文字列表示へのポインタ
	Ito::DebugFont* m_debugFont;
	// 共通ステート
	DirectX::CommonStates* m_states;
	//エフェクトファクトリー
	DirectX::EffectFactory* m_fxFactory;
	//PrimitiveBatch
	DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* m_primitiveBatch;

	// --- マネージャーへのポインタ ---
	WIS::ObjectManager* m_objectManager;
	WIS::CameraManager* p_cameraManager;
// ;

public:
	// コンストラクタ
	UserResources()
		: m_timer(nullptr)
		, m_deviceResources(nullptr)
		, m_keyboardTracker(nullptr)
		, m_mouseTracker(nullptr)
		, m_mouse(nullptr)
		, m_debugFont(nullptr)
		, m_states(nullptr)
		, m_fxFactory(nullptr)
		, m_primitiveBatch(nullptr)
		, m_objectManager(nullptr)   // マネージャーポインタをnullptrで初期化
		, p_cameraManager(nullptr)
		//, m_blueprintManager(nullptr) // マネージャーポインタをnullptrで初期化
	{
	}

	// ------------------------------------------ //
	// ステップタイマー	
	// ------------------------------------------ //
	void SetStepTimerStates(DX::StepTimer* timer) { m_timer = timer; }
	DX::StepTimer* GetStepTimer() { return m_timer; }

	// ------------------------------------------ //
	// デバイスリソース	
	// ------------------------------------------ //
	void SetDeviceResources(DX::DeviceResources* deviceResources) { m_deviceResources = deviceResources; }
	DX::DeviceResources* GetDeviceResources() { return m_deviceResources; }

	// ------------------------------------------ //
	// キーボードステートトラッカー	
	// ------------------------------------------ //
	void SetKeyboardStateTracker(DirectX::Keyboard::KeyboardStateTracker* tracker) { m_keyboardTracker = tracker; }
	DirectX::Keyboard::KeyboardStateTracker* GetKeyboardStateTracker() { return m_keyboardTracker; }

	// ------------------------------------------ //
	// マウスステートトラッカー	
	// ------------------------------------------ //
	void SetMouseStateTracker(DirectX::Mouse::ButtonStateTracker* tracker) { m_mouseTracker = tracker; }
	DirectX::Mouse::ButtonStateTracker* GetMouseStateTracker() { return m_mouseTracker; }

	// ------------------------------------------ //
	// マウス	
	// ------------------------------------------ //
	void SetMouse(DirectX::Mouse* mouse) { m_mouse = mouse; }
	DirectX::Mouse::State GetMouseState() { return m_mouse->GetState(); }

	// ------------------------------------------ //
	// 共通ステート	
	// ------------------------------------------ //
	void SetCommonStates(DirectX::CommonStates* states) { m_states = states; }
	DirectX::CommonStates* GetCommonStates() { return m_states; }

	// ------------------------------------------ //
	// デバッグフォント	
	// ------------------------------------------ //
	void SetDebugFont(Ito::DebugFont* debugFont) { m_debugFont = debugFont; }
	Ito::DebugFont* GetDebugFont() { return m_debugFont; }

	// ------------------------------------------ //
	// エフェクトファクトリー	
	// ------------------------------------------ //
	void SetEffectFactory(DirectX::EffectFactory* fxFactory) { m_fxFactory = fxFactory; }
	DirectX::EffectFactory* GetEffectFactory() { return m_fxFactory; }

	// ------------------------------------------ //
	// プリミティブバッチ
	// ------------------------------------------ //
	void SetPrimitiveBatch(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* primitiveBatch) { m_primitiveBatch = primitiveBatch; }
	DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* GetPrimitiveBatch() { return m_primitiveBatch; }

	// ------------------------------------------ //
	// Object Manager
	// ------------------------------------------ //
	void SetObjectManager(WIS::ObjectManager* manager) { m_objectManager = manager; }
	WIS::ObjectManager* GetObjectManager() { return m_objectManager; }


	// ------------------------------------------ //
	// Camera Manager
	// ------------------------------------------ //
	void SetCameraManager(WIS::CameraManager* manager) { p_cameraManager = manager; }
	WIS::CameraManager* GetCameraManager() { return p_cameraManager; }

};

