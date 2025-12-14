#pragma once
#include <vector>
#include <memory>
#include <string>

// 前方宣言
namespace WIS {
    class GameObject;
}

namespace WIS {

    // 全てのGameObjectの衝突判定を管理するシングルトンクラス
    class CollisionManager
    {
    public:
        // シングルトンのインスタンスを取得
        static CollisionManager* GetInstance();

        // 衝突判定の対象としてオブジェクトを登録
        void ResistCollisionObject(std::shared_ptr<GameObject> obj);

        // 登録解除
        void RemoveCollisionObject(std::shared_ptr<GameObject> obj);

        // 毎フレームの更新処理
        void Update();

    private:
        // プライベートコンストラクタ（シングルトンのため）
        CollisionManager() = default;
        ~CollisionManager() = default;

        
        friend class std::default_delete<CollisionManager>;

        // 衝突判定のメインループ
        void CheckCollisionObjects();

        // 2つのオブジェクトの衝突をチェック
        bool CheckHit(const std::shared_ptr<GameObject>& obj1, const std::shared_ptr<GameObject>& obj2);

		template<typename T, typename Y>
		bool CheckBoundingCollision(
			const T& bounding1,
			const DirectX::SimpleMath::Matrix& worldMatrix1,
			const Y& bounding2,
			const DirectX::SimpleMath::Matrix& worldMatrix2);
    private:
        // シングルトンのインスタンス
        static std::unique_ptr<CollisionManager> s_collisionManager;

        //  オブジェクトのリストを weak_ptr で保持 
        // 所有権は持たず、オブジェクトを監視するだけ
        std::vector<std::weak_ptr<GameObject>> m_CollisionObjects;

        bool m_IsDrawCollision;
    };


    template<typename T, typename Y>
    inline bool CollisionManager::CheckBoundingCollision(
        const T& bounding1,
        const DirectX::SimpleMath::Matrix& worldMatrix1,
        const Y& bounding2,
        const DirectX::SimpleMath::Matrix& worldMatrix2)
    {
        // ワールド空間でのバウンディングボリュームを作成するための変数
        T worldBounding1;
        Y worldBounding2;

        // ローカルのバウンディングボリュームをワールド行列で変換
        // Transform関数は、位置・回転・スケールを適用して worldBounding に結果を格納します
        bounding1.Transform(worldBounding1, worldMatrix1);
        bounding2.Transform(worldBounding2, worldMatrix2);

        // ワールド空間同士で交差判定 (IntersectsはDirectXCollisionの機能)
        return worldBounding1.Intersects(worldBounding2);
    }

}