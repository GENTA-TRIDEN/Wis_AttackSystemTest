#include "pch.h"
#include "Messenger.h"

#include "WISLib/GameObject/Component/ComponentBase.h"

// s_messengerを初期化する
std::unique_ptr<Messenger> Messenger::s_messenger = nullptr;

// コンストラクタ
Messenger::Messenger()
	:
	m_elapsedTime{},
	m_objects{}
{
}

// Messengerクラスのインスタンスを取得する
Messenger* Messenger::GetInstance()
{
	// Messengerクラスのインスタンス生成されていない場合
	if (s_messenger == nullptr)
	{
		// Messengerクラスのインスタンスを生成する
		s_messenger = std::unique_ptr<Messenger>(new Messenger());
	}
	// Messengerクラスのインスタンスを返す
	return s_messenger.get();
}

// インスタンスを破棄する
void Messenger::DestroyInstance()
{
	// Messengerクラスのインスタンスをリセットする
	s_messenger.reset();
}

// オブジェクトを登録する
void Messenger::Register(int componentNumber, WIS::ComponentBase* object)
{
	// オブジェクトIDとオブジェクトを登録する
	m_objects.emplace(componentNumber, object);
}

// オブジェクトの登録を解除する
void Messenger::UnRegister(int componentNumber)
{
	// オブジェクトIDを指定してオブジェクトの登録を解除する
	m_objects.erase(componentNumber);
}

// オブジェクトにメッセージを送信する
void Messenger::Notify(int componentNumber, Message::MessageID messageID, const std::any& value)
{
	// メッセージを送信するオブジェクトを検索する
	auto it = m_objects.find(componentNumber);
	// メッセージを送信するオブジェクトが見つかった場合
	if (it != m_objects.end())
	{
		// 送信するオブジェクトのメッセージハンドラを呼び出す
		it->second->OnMessageAccepted(messageID, value);
	}
}

// オブジェクトを取得する
WIS::ComponentBase* Messenger::GetObject(int componentNumber)
{
	// メッセージを送信するオブジェクトを検索する
	auto it = m_objects.find(componentNumber);
	// メッセージを送信するオブジェクトが見つかった場合
	if (it != m_objects.end())
	{
		// オブジェクトを取得する
		return it->second;
	}
	return nullptr;
}

// 遅延メッセージを通知する
void Messenger::NotifyAfterDelay(int componentNumber, Message::MessageID messageID, const std::any& value, float delaySeconds)
{
	// 遅延メッセージを追加する
	m_delayedMessages.push_back({ componentNumber, messageID, value, delaySeconds });
}

// 更新する
void Messenger::Update(float elapsedTime)
{
	// 経過時間を保管する
	m_elapsedTime = elapsedTime;
	// 登録された遅延メッセージの時間を更新する
	for (auto it = m_delayedMessages.begin(); it != m_delayedMessages.end(); )
	{
		// 遅延時間から経過時間を減らす
		it->delayTime -= elapsedTime;
		// 遅延時間になった場合の処理を行う
		if (it->delayTime <= 0.0f)
		{
			// メッセージを通知する
			Notify(it->componentNumber, it->messageID, it->value);
			// 遅延メッセージ配列から通知済み遅延メッセージを削除する
			it = m_delayedMessages.erase(it);
		}
		else
		{
			it++;
		}
	}
}