//--------------------------------------------------------------------------------------
// File: Button.cpp
//
// Button class
//
//-------------------------------------------------------------------------------------

#include "pch.h"
#include "UserInterface.h"
#include "Button.h"

#include "BinaryFile.h"
#include "DeviceResources.h"
#include <SimpleMath.h>
#include <Effects.h>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <WICTextureLoader.h>
#include <CommonStates.h>
#include <vector>

using namespace DirectX;
/// <summary>
/// コンストラクタ
/// </summary>
WIS::Button::Button()
	: m_windowHeight(0)
	, m_windowWidth(0)
	, m_pDR(nullptr)
	, m_button(nullptr)
{
}

/// <summary>
/// デストラクタ
/// </summary>
WIS::Button::~Button()
{
}
/// <summary>
/// 初期化
/// </summary>
/// <param name="pDR">デバイスリソース</param>
/// <param name="width">ウインドウ幅</param>
/// <param name="height">ウインドウ高さ</param>
void WIS::Button::Initialize(DX::DeviceResources* pDR, int width, int height)
{
	m_pDR = pDR;
	m_windowWidth = width;
	m_windowHeight = height;
	SetTexture(Normal, L"Resources/Textures/NormalButton.png");
	SetTexture(Hover, L"Resources/Textures/HoverButton.png");
	SetTexture(Push, L"Resources/Textures/PushButton.png");


}
/// <summary>
/// 更新
/// </summary>
void WIS::Button::Update()
{
	//  キーボード情報の更新
	DirectX::Keyboard::State keystate = DirectX::Keyboard::Get().GetState();
	auto mouse = DirectX::Mouse::Get().GetState();


	m_tracker.Update(keystate);

	//マウスの位置から状態変化させる

	//マウスがボタンの上にあるか判定
	DirectX::SimpleMath::Vector2 mousePos =
		DirectX::SimpleMath::Vector2(
			static_cast<float>(mouse.x),
			static_cast<float>(mouse.y));

	// m_button が nullptr の場合は終了
	if (m_button == nullptr) return;

	DirectX::SimpleMath::Vector2 buttonPos = m_button->GetPosition();


	DirectX::SimpleMath::Vector2 textureSize = m_button->GetTextureSize(); // UserInterface.hに追加した関数
	DirectX::SimpleMath::Vector2 scaleFactor = m_button->GetScale();

	DirectX::SimpleMath::Vector2 buttonScale = textureSize * scaleFactor;


		// 判定には計算された buttonScale を使用
	if (mousePos.x >= buttonPos.x && mousePos.x <= buttonPos.x + buttonScale.x
		&& mousePos.y >= buttonPos.y && mousePos.y <= buttonPos.y + buttonScale.y)
	{
		//ボタンの上にマウスがある
		if (mouse.leftButton && m_state == Hover)
		{
			//押された
			m_state = Push;
			m_button->LoadTexture(StateTexturePath[Push]);
			
			if (OnClick == false) {

				for (std::function<void()> func: m_onClickCallbacks) {
					func();
				}
				OnClick = true;
			}
		}
		else if (m_state != Push || !mouse.leftButton)
		{
			//ホバー
			m_state = Hover;
			m_button->LoadTexture(StateTexturePath[Hover]);
			OnClick = false;
		}
	}
	else
	{
		//通常
		m_state = Normal;
		m_button->LoadTexture(StateTexturePath[Normal]);
		OnClick = false;
	}



	//






}

/// <summary>
/// 描画
/// </summary>
void WIS::Button::Render()
{
	if (m_button == nullptr)return;

	m_button->Render();
	//m_textをぼたんのちゅうおうに表示









}

/// <summary>
/// 追加関数
/// </summary>
/// <param name="path">画像のパス</param>
/// <param name="position">アンカー座標</param>
/// <param name="scale">元の画像に対するスケール</param>
/// <param name="anchor">アンカー</param>
void WIS::Button::Add(const wchar_t* path, DirectX::SimpleMath::Vector2 position, DirectX::SimpleMath::Vector2 scale, kHorikawa::ANCHOR anchor)
{
	std::unique_ptr<kHorikawa::UserInterface> userInterface = std::make_unique<kHorikawa::UserInterface>();
	userInterface->Create(m_pDR
		, path
		, position
		, scale
		, anchor);
	userInterface->SetWindowSize(m_windowWidth, m_windowHeight);

	m_button = std::move(userInterface);
}

void WIS::Button::SetTexture(Button::STATE state, const wchar_t* path)
{
	StateTexturePath[state] = path;
}