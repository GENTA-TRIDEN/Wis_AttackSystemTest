// WISLib/Archetype/ArchetypeDefManager.h (例)
#pragma once
#include <map>

namespace WIS
{
    class ArchetypeDefManager
    {
    private:
        // 全Archetypeの定義を保持するマップ
        // Key: Archetypeの名前 (例: "WeaponArchetype_Default")
        // Value: 含まれるGameObjectの名前リスト
        static std::map<std::string, std::vector<std::string>> s_Definitions;

    public:
        // 定義の登録 (ゲーム起動時やレベルロード時に一度だけ行う)
        static void RegisterDefinition(const std::string& key, const std::vector<std::string>& names) {
            s_Definitions[key] = names;
        }

        // 定義の取得
        static const std::vector<std::string>* GetDefinition(const std::string& key) {
            auto it = s_Definitions.find(key);
            return (it != s_Definitions.end()) ? &it->second : nullptr;
        }
    };

    // 実装ファイル (.cpp) で静的メンバーを定義
    // std::map<std::string, std::vector<std::string>> WIS::ArchetypeDefManager::s_Definitions;
}