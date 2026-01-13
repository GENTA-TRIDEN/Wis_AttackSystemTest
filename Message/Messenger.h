#pragma once
#ifndef MESSENGER_DEFINED
#define MESSENGER_DEFINED

#include "Message.h"
// ComponentBase.h へのパスは環境に合わせて調整してください
#include <any>
#include <unordered_map>
#include <vector>
#include <memory>

// 遅延メッセージ
struct DelayedMessage
{
	// コンポーネント番号
	int componentNumber;
	// メッセージID
	Message::MessageID messageID;
	// 値
	std::any value;
	// 通知までの残り時間
	float delayTime;
};

namespace WIS
{
	class ComponentBase;

}

// Messengerクラス
class Messenger
{
public:
	// オブジェクトを取得する
	WIS::ComponentBase* GetObject(int componentNumber);
	// 経過時間を取得する
	float GetElapsedTime() const { return m_elapsedTime; }
	// 経過時間を設定する
	void SetElapsedTime(const float& elapsedTime) { m_elapsedTime = elapsedTime; }

public:
	// Messengerクラスのインスタンスを取得する
	static Messenger* GetInstance();
	// Messengerクラスのインスタンスを破棄する
	static void DestroyInstance();

	// オブジェクトを登録する
	// componentNumber: 任意のID（例: プレイヤーなら0, 敵なら1...など管理しやすい番号）
	void Register(int componentNumber, WIS::ComponentBase* component);

	// オブジェクトの登録を解除する
	void UnRegister(int componentNumber);

	// メッセージを送信する
	void Notify(int componentNumber, Message::MessageID messageID, const std::any& value);

	// 遅延メッセージを通知する
	void NotifyAfterDelay(int componentNumber, Message::MessageID messageID, const std::any& value, float delaySeconds);

	// 更新する
	void Update(float deltaTime);

private:
	// コピー・ムーブ禁止
	Messenger(const Messenger&) = delete;
	Messenger& operator=(const Messenger&) = delete;
	Messenger(Messenger&&) = delete;
	Messenger& operator=(Messenger&&) = delete;
	// コンストラクタ
	Messenger();

private:
	// メッセンジャーインスタンス
	static std::unique_ptr<Messenger> s_messenger;

	// オブジェクトIDとコンポーネントのマッピング
	// 生ポインタで保持しますが所有権は持ちません
	std::unordered_map<int, WIS::ComponentBase*> m_objects;

	// 経過時間
	float m_elapsedTime;
	// 遅延メッセージ
	std::vector<DelayedMessage> m_delayedMessages;
};

#endif		// MESSENGER_DEFINED