#pragma once
#include "ComponentBase.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <Model.h>

// DirectX Tool Kit のアニメーションクラスをインクルード
#include "WISLib/Common/Animation.h" // "Animation.h"への正しいパスに置き換えてください

namespace WIS {

    class ModelComponent; // 前方宣言

    class AnimationComponent : public ComponentBase
    {
    private:
        // アニメーションクリップを名前で管理するマップ
        std::unordered_map<std::string, std::unique_ptr<DX::AnimationSDKMESH>> m_animations;

        // ボーンの最終的な変換行列を格納する配列
        DirectX::ModelBone::TransformArray m_boneTransforms;

        // アニメーションを適用する対象のModelComponent（循環参照を避けるためweak_ptr）
        std::weak_ptr<ModelComponent> m_targetModelComponent;

        std::string m_currentAnimationName; // 現在再生中のアニメーション名
        bool m_isPlaying = false;
        int m_targetModelIndex = 0; // ModelComponent内のどのモデルかを示すインデックス


        bool m_isAnimEnd;
    public:
        AnimationComponent();
        ~AnimationComponent();


        bool LoadAnimation(const std::wstring& fileName, const std::string& animName, int targetModelIndex = 0, bool isLoop = true);

        // --- 再生コントロール ---
        void Play() { m_isPlaying = true; }
        void Stop() { m_isPlaying = false; }
        bool IsPlaying() const { return m_isPlaying; }

        // --- ゲッター ---
        DirectX::XMMATRIX* GetBoneTransforms() { return m_boneTransforms.get(); }
        size_t GetBoneCount() const;

        // 再生するアニメーションを名前で切り替える
        void SetCurrentAnimation(const std::string& animName);

        void SetAnimationFrame(int Frame);
        //
        bool IsAnimationEnd();

    public:
        // --- ComponentBase ライフサイクル関数 ---
        void OnStart(UserResources* userResources) override;
        void OnUpdate(const float elapsedTime, UserResources* userResources) override;
        void OnPostUpdate(const float elapsedTime, UserResources* userResources) override;

    };
}