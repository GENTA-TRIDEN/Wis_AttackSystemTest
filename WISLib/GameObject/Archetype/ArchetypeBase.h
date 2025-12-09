#pragma once
#include <vector>
#include <string>
#include <memory>
#include <WISLib/GameObject/GameObject.h>
#include <WISLib/GameObject/ObjectManager.h>

namespace WIS
{
class ArchetypeBase
{
private:
	// Archetypeが保持するGameObjectの名前リスト
	std::vector<std::string> m_gameObjectNames;
	// 保持するオブジェクトのリスト
	std::vector<std::shared_ptr<class WIS::GameObject>> m_HoldingObjects;

public:
	ArchetypeBase() {}
	virtual ~ArchetypeBase() {}



	void AddGameObjectName(const std::string& name) {
		m_gameObjectNames.push_back(name);
	}
	const std::vector<std::string>& GetGameObjectNames() const {
		return m_gameObjectNames;
	}

	void HoaldingObjects(WIS::ObjectManager* objectManager) {
		m_HoldingObjects.clear();
		for (const auto& name : m_gameObjectNames) {
			auto obj = objectManager->GetObject(name);
			if (obj) {
				m_HoldingObjects.push_back(obj);
			}
		}
	}

	void SetHoldingObject(WIS::ObjectManager* objectManager) {
		for (const auto& obj : m_HoldingObjects) {
			objectManager->AddObject(obj);
		}
	}


};
}

