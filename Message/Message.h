#pragma once
#ifndef MESSAGE_DEFINED
#define MESSAGE_DEFINED

// Messageクラス
class Message
{
public:
	// メッセージID
	enum MessageID : int  
	{ 
		//Player
		PLAYER_MOVE_FORWARD = 1,	// プレイヤー前進
		PLAYER_MOVE_BACKWARD = 2,	// プレイヤー後退
		PLAYER_MOVE_LEFT = 3,		// プレイヤー左移動
		PLAYER_MOVE_RIGHT = 4,		// プレイヤー右移動

		//Controller用のMove
		PLAYER_MOVE_CONTROLLER= 10,	// コントローラー

		// 「衝突」メッセージID
		DETECT_BULLET_COLLISION = 100	,	// 「砲弾」衝突検出

		// 「マウス」メッセージID
		LEFT_BUTTON_CLICKED = 200				// 「左ボタン」がクリックされた
	};
};

#endif		// MESSAGE_DEFINED


