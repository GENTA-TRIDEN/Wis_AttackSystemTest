#pragma once
// 基底クラスのインクルード
#include "WISLib/GameObject/Archetype/ArchetypeBase.h"

namespace WIS
{

	/**
	 * @brief 武器のアーキタイプを定義するクラス
	 * * ArchetypeBaseを継承し、武器オブジェクト群をシーン遷移時に保持するために使用されます。
	 */
	class WeapoArchetype :public WIS::ArchetypeBase
	{
	private:
		// 将来的に武器固有のデータをここに追加できます


	public:
		// コンストラクタ (実装ファイルで基底クラスのコンストラクタを呼び出す必要があります)
		WeapoArchetype();
		// デストラクタ
		~WeapoArchetype();



	};

}