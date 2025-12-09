#include "pch.h"
#include "WISLib/GameObject/GameObject.h"
#include "UserResources.h" // UserResources の定義が必要なのでインクルード

#include "WISLib/WISLibTags.h"

using namespace WIS;

GameObject::GameObject()
{
    // 初期化が必要ならここに記述
}

GameObject::~GameObject()
{
    // SceneTags のメモリ解放
    if (m_pSceneTags) {
        delete m_pSceneTags;
        m_pSceneTags = nullptr;
    }
}

// --- ライフサイクル関数 ---

void GameObject::Start(UserResources* userResources) {
    if (!m_isStarted) {
        for (auto& comp : m_components) {
            comp->OnStart(userResources);
        }
        m_isStarted = true;
    }
}

void GameObject::PreUpdate(const float elapsedTime, UserResources* userResources) {
    for (auto& comp : m_components) comp->OnPreUpdate(elapsedTime, userResources);
}

void GameObject::Update(const float elapsedTime, UserResources* userResources) {
    for (auto& comp : m_components) comp->OnUpdate(elapsedTime, userResources);
}

void GameObject::PostUpdate(const float elapsedTime, UserResources* userResources) {
    for (auto& comp : m_components) comp->OnPostUpdate(elapsedTime, userResources);
}

void GameObject::OnRender(UserResources* userResources, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj) {
    for (auto& comp : m_components) comp->OnRender(userResources, view, proj);
}

void GameObject::OnRelease() {
    for (auto& comp : m_components)
        if (comp)comp->OnRelease();
}

// --- ゲッター/セッター ---

void GameObject::SetName(const std::string& name) {
    m_name = name;
}

const std::string& GameObject::GetName() const {
    return m_name;
}

void GameObject::SetActive(bool isActive) {
    m_isActive = isActive;
}

bool GameObject::CheckActive() const {
    return m_isActive;
}

// --- イベントディスパッチ ---

void GameObject::DispatchCollision(std::shared_ptr<GameObject> otherObject)
{
    for (auto& comp : m_components) {
        comp->OnCollision(otherObject);
    }
}

// --- タグ管理 ---

void GameObject::SetSceneTags(WIS::SceneTag sceneTags) {
    if (m_pSceneTags) {
        m_pSceneTags->push_back(sceneTags);
    }
    else {
        m_pSceneTags = new std::vector<WIS::SceneTag>;
        m_pSceneTags->push_back(sceneTags);
    }
}

std::vector<WIS::SceneTag>* GameObject::GetSceneTags() {
    return m_pSceneTags;
}