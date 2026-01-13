//--------------------------------------------------------------------------------------
// File: Button.cpp
//
// Button class
//
//-------------------------------------------------------------------------------------

#pragma once

#include "UserInterface.h"
#include "StepTimer.h"
#include "UserResources.h"
#include <DeviceResources.h>
#include <SimpleMath.h>
#include <Effects.h>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <WICTextureLoader.h>
#include <CommonStates.h>
#include <unordered_map>
#include <vector>
#include "Keyboard.h"

namespace WIS
{
	class Button
	{
	public:
		//	buttonの状態
		enum STATE
		{
			Normal,
			Push,
			Hover
		};


		//	変数
	private:

		//	定数
		STATE m_state = Normal;

		DX::DeviceResources* m_pDR;

		std::unique_ptr<kHorikawa::UserInterface> m_button;

		int m_windowWidth, m_windowHeight;

		DirectX::Keyboard::KeyboardStateTracker m_tracker;
		DirectX::Mouse::ButtonStateTracker m_MouseState;
		DirectX::GamePad::ButtonStateTracker m_GamePadState;

		std::unordered_map<Button::STATE, const wchar_t*> StateTexturePath;

		//テキストを中央に入れる
		const wchar_t* m_text = L"Button";

		//当たった時に実行される関数を格納
		std::vector<std::function<void(void)>> m_onClickCallbacks;

		//Clickしたときに一回だけ実行される
		bool OnClick;

		bool m_Isselected = false;
		//	関数
	public:
		Button();
		~Button();

		void Initialize(DX::DeviceResources* pDR, int width, int height);
		void Update();
		void Render();

		void Add(const wchar_t* path
			, DirectX::SimpleMath::Vector2 position
			, DirectX::SimpleMath::Vector2 scale
			, kHorikawa::ANCHOR anchor);

		bool IsClick() {
			return m_state == STATE::Push;
		}


		void Select() {
			m_Isselected = true;
		}


		//コールバック関数の登録
		void SetOnCnClickCallbacks(std::function<void(void)> func) {
			m_onClickCallbacks.push_back(func);
		}


		//コールバック関数のクリア
		void ClearOnClickCallbacks() {
			m_onClickCallbacks.clear();
		}



		void SetTexture(Button::STATE state, const wchar_t* path);
	};
}