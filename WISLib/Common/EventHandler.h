#pragma once

namespace WISLib {
class EventHandler
{
private:
	std::vector<std::function<void()>> m_eventListeners;


public :
	EventHandler() = default;
	~EventHandler() = default;
	// イベントリスナーの追加
	void ResistEventListener(const std::function<void()>& listener)
	{
		m_eventListeners.push_back(listener);
	}


	// イベントの発火
	void TriggerEvent()
	{
		for (const auto& listener : m_eventListeners)
		{
			listener();
		}
	}

};

}

