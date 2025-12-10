#pragma once
#include <vector>
#include <string>
#include <memory>
#include <WISLib/GameObject/GameObject.h>
#include <WISLib/GameObject/ObjectManager.h>
#include "InGameObject/Archetype/ArchetypeDefManager.h"

namespace WIS
{
	// GameObjectクラスの前方宣言 (m_HoldingObjectsのメンバ型として使用)
	class GameObject;
	class ArchetypeBase
	{
	private:
		// どのArchetype定義を参照するかを示す一意なキー
		std::string m_archetypeKey;

		// 保持するオブジェクトのリスト（これはランタイムデータとして保持し続けます）
		std::vector<std::shared_ptr<class WIS::GameObject>> m_HoldingObjects;

	public:
		// コンストラクタでキーを受け取るように変更 (引数なしコンストラクタは削除)
		ArchetypeBase(const std::string& key) : m_archetypeKey(key) {}
		virtual ~ArchetypeBase() {}

		// ArchetypeDefManagerを参照し、m_archetypeKeyに対応するオブジェクトの名前リストを取得する
		const std::vector<std::string>* GetGameObjectNames() const {
			// Managerから定義を取得
			return WIS::ArchetypeDefManager::GetDefinition(m_archetypeKey);
		}

		void HoaldingObjects(WIS::ObjectManager* objectManager) {
			m_HoldingObjects.clear();
			// Managerから、このアーキタイプが保持すべきオブジェクトの名前リストを取得
			const auto* names = GetGameObjectNames();

			if (names) {
				for (const auto& name : *names) {
					// ObjectManagerから名前を使ってオブジェクトのshared_ptrを取得
					auto obj = objectManager->GetObject(name);
					if (obj) {
						// 取得したオブジェクトを内部リスト m_HoldingObjects に追加
						m_HoldingObjects.push_back(obj);
					}
				}
			}
		}
	};
}