#pragma once

#include <string>
#include <unordered_map>

namespace WIS {
	class NumberManager
	{
		// 生成する番号の型を定義
		using Number = int;
	private:

		// キーごとの現在の最大番号を保持
		// (連番のIDを効率的にO(1)で生成するために変更)
		std::unordered_map<std::string, Number> m_numberMap;

	private:
		// プライベートコンストラクタ
		NumberManager() = default;
		// シングルトンのためのコンストラクタと代入演算子の削除
		NumberManager(const NumberManager&) = delete;
		NumberManager& operator=(const NumberManager&) = delete;

	public:
		//シングルトン
		static NumberManager* GetInstance()
		{

			static NumberManager instance;
			return &instance;
		}

		//番号を生成
		// 連番の次の番号を生成し、それを登録する
		Number GenerateNumber(const std::string& key)
		{
			// マップにキーが存在しない場合、Number型(int)のデフォルト値0が初期値として使われ、
			// インクリメントされて1が最初の番号となる。
			m_numberMap[key]++;
			return m_numberMap[key];
		}

		//番号を取得
		// 現在の最大番号（最後に生成された番号）を取得
		// 元のGetNumbers(const Numbers&)とは返り値の型が異なる点に注意
		const Number GetLastNumber(const std::string& key) const
		{
			auto it = m_numberMap.find(key);
			if (it != m_numberMap.end()) {
				return it->second;
			}
			// キーが存在しない場合は0を返す（次に生成される番号は1）
			return 0;
		}

		// 外部から現在の最大番号を設定
		void SetMaxNumber(const std::string& key, Number maxNumber)
		{
			m_numberMap[key] = maxNumber;
		}
	};


}