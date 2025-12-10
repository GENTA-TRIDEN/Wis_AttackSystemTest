#include "pch.h"
#include "WeapoArchetype.h"
#include <iostream> // デストラクタで何か処理を行う可能性を考慮

// 基底クラス ArchetypeBase のコンストラクタは引数 (const std::string& key) を要求するため、
// 派生クラス WeapoArchetype のコンストラクタで明示的にこれを呼び出す必要があります。

namespace WIS
{

    WeapoArchetype::WeapoArchetype()
        // 初期化子リストで基底クラスのコンストラクタを呼び出す
        // "WeaponArchetype" は、このアーキタイプを識別するためにArchetypeDefManagerに登録されているキーと仮定します。
        : ArchetypeBase("WeaponArchetype")
    {
        // 武器アーキタイプ固有の初期化処理があればここに記述
    }

    /**
     * @brief WeapoArchetypeのデストラクタ
     */
    WeapoArchetype::~WeapoArchetype()
    {
        // 解放処理など
    }

}