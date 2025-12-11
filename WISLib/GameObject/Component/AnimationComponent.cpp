#include "pch.h"
#include "AnimationComponent.h"
#include "ModelComponent.h"
#include "WISLib/GameObject/GameObject.h"

using namespace WIS;
using namespace DirectX;

AnimationComponent::AnimationComponent() = default;
AnimationComponent::~AnimationComponent() = default;

bool AnimationComponent::LoadAnimation(const std::wstring& fileName, const std::string& animName, int targetModelIndex, bool isLoop)
{
    auto owner = GetOwner();
    if (!owner) return false;

    //ModelComponentをweak_ptrとして保持
    m_targetModelComponent = owner->GetComponent<ModelComponent>();

    // lock()で有効なポインタ（shared_ptr）が取得できるか確認
    auto modelComp = m_targetModelComponent.lock();
    if (!modelComp) return false;

    //ModelComponentから対象のモデルを取得
    Model* model = modelComp->GetModel(targetModelIndex);
    if (!model || model->bones.empty()) return false;

    m_targetModelIndex = targetModelIndex;

    //アニメーションをロード
    auto newAnimation = std::make_unique<DX::AnimationSDKMESH>();
    if (FAILED(newAnimation->Load(fileName.c_str(), isLoop))) { // isLoop引数付きのLoadを呼び出し
        return false;
    }

    //アニメーションとモデルのボーンをバインド
    if (!newAnimation->Bind(*model)) {
        // バインド失敗（モデルとアニメーションのボーン名が一致しないなど）
        return false;
    }

    //ボーン行列を格納する配列を確保
    m_boneTransforms = ModelBone::MakeArray(model->bones.size());

    //マップにアニメーションを格納
    m_animations[animName] = std::move(newAnimation);

    // 最初のアニメーションを現在のアニメーションとして設定
    if (m_currentAnimationName.empty()) {
        SetCurrentAnimation(animName);
    }

    return true;
}

size_t AnimationComponent::GetBoneCount() const
{
    if (auto modelComp = m_targetModelComponent.lock()) {
        auto model = modelComp->GetModel(m_targetModelIndex);
        return model ? model->bones.size() : 0;
    }
    return 0;
}

void AnimationComponent::SetCurrentAnimation(const std::string& animName)
{
    // 指定されたアニメーションが存在するかチェック
    if (m_animations.count(animName)) {
        m_currentAnimationName = animName;
        // アニメーションを最初から再生するためにリセット
        m_animations[m_currentAnimationName]->Reset();
    }
}

void WIS::AnimationComponent::SetAnimationFrame(int Frame)
{
}

bool WIS::AnimationComponent::IsAnimationEnd()
{

    return false;
}

void AnimationComponent::OnStart(UserResources* /*userResources*/)
{
    // BlueprintやScene側でLoadAnimationが呼ばれることを想定
}

void AnimationComponent::OnUpdate(const float elapsedTime, UserResources* /*userResources*/)
{
}

void WIS::AnimationComponent::OnPostUpdate(const float elapsedTime, UserResources* userResources)
{
    if (!m_isPlaying) return;

    // 現在のモデルコンポーネントとアニメーションクリップを安全に取得
    auto modelComp = m_targetModelComponent.lock();
    auto it = m_animations.find(m_currentAnimationName);

    if (modelComp && it != m_animations.end())
    {
        // 該当のアニメーションを取得
        auto& currentAnimation = it->second;

        // アニメーション時間を更新
        currentAnimation->Update(elapsedTime);
        // ボーン行列を計算
        Model* model = modelComp->GetModel(m_targetModelIndex);
        if (model && !model->bones.empty())
        {
            currentAnimation->Apply(*model, model->bones.size(), m_boneTransforms.get());
        }

        m_isAnimEnd = currentAnimation->IsEnd();

    }
    else {
    }


}
