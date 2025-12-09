#pragma once
#include "ComponentBase.h"
#include "Model.h"
#include <Effects.h>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <wrl/client.h>
#include <vector>
#include <string>

namespace WIS {

    // ボーン階層情報を格納するための構造体
    struct BoneInfo
    {
        std::wstring name;
        int parentIndex;
        DirectX::SimpleMath::Matrix localTransform; // バインドポーズのローカルトランスフォーム行列
        DirectX::SimpleMath::Matrix inverseBindPose; // インバースバインドポーズ行列
    };

    class ModelComponent : public WIS::ComponentBase
    {
    private:
        DirectX::SimpleMath::Matrix m_worldMatrix = DirectX::SimpleMath::Matrix::Identity;
        std::vector<DirectX::SimpleMath::Matrix> m_localMatrices;
        std::vector<std::unique_ptr<DirectX::Model>> m_modelVec;

        bool m_DrawModel;
        // --- Debug Visualization Members ---
        // Toggles
        bool m_showNormals = false;
        bool m_showFaceNormals = false;
        bool m_showWireframe = true;
        bool m_showBones = false; // 静的ボーン表示フラグ


        std::wstring m_names;

        // Data and States
        std::vector<std::pair<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3>> m_vertexNormals;
        std::unique_ptr<DirectX::BasicEffect> m_lineEffect;
        Microsoft::WRL::ComPtr<ID3D11InputLayout> m_lineInputLayout;
        Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_wireframeRasterizerState;

        // --- Static Bone Hierarchy Data ---
        std::vector<BoneInfo> m_boneHierarchy;

        ID3D11DepthStencilState* m_stencilState = nullptr;
    public:
        ModelComponent();
        ~ModelComponent();

        int AddModel(ID3D11Device* device, ID3D11DeviceContext* context, wchar_t const* fileName, DirectX::EffectFactory* fxFactory);

        
        int GetModelNum() { return static_cast<int>(m_modelVec.size()); }

        DirectX::Model* GetModel(int index)
        {
            if (index < 0 || index >= m_modelVec.size()) return nullptr;
            return m_modelVec[index].get();
        }

        //モデルごとの設定
        void SetScale(int index, const DirectX::SimpleMath::Vector3& scale);
        void SetRotate(int index, const DirectX::SimpleMath::Vector3& rotate);


        void SetLocalMatrix(int index, const DirectX::SimpleMath::Matrix& localMatrix)
        {
            if (index < 0 || index >= m_localMatrices.size()) return;
            m_localMatrices[index] = localMatrix;
        }


        void SetAddLocalMatrix(int index, const DirectX::SimpleMath::Matrix& localMatrix)
        {
            if (index < 0 || index >= m_localMatrices.size()) return;
            m_localMatrices[index] *= localMatrix;
        }
        void IsDrawModel(bool isDraw) { m_DrawModel = isDraw; }


        const BoneInfo& GetBoneInfo(size_t boneIndex) const;


        // --- Debug Control Functions ---
        void ShowNormals(bool show) { m_showNormals = show; }
        void ShowFaceNormals(bool show) { m_showFaceNormals = show; }
        void ShowWireframe(bool show) { m_showWireframe = show; }
        void SetShowBones(bool show) { m_showBones = show; }

        DirectX::ModelBone GetBoneByIndex(int modelIndex, size_t boneIndex);

        bool GetBoneWorldPosition(size_t boneIndex, DirectX::SimpleMath::Vector3& outPosition);
        bool GetBoneWorldMatrix(size_t boneIndex, DirectX::SimpleMath::Matrix& outMatrix);
        bool GetAnimatedBoneWorldMatrix(size_t boneIndex, DirectX::SimpleMath::Matrix& outMatrix);


        void SetDepthStencilState(ID3D11DepthStencilState* state) {
            m_stencilState = state;
        }

    public:
        void OnStart(UserResources* userResources) override;
        void OnPreUpdate(const float elapsedTime, UserResources* userResources) override;
        void OnUpdate(const float elapsedTime, UserResources* userResources) override;
        void OnPostUpdate(const float elapsedTime, UserResources* userResources) override;
        void OnRender(UserResources* userResources,
            const DirectX::SimpleMath::Matrix& view,
            const DirectX::SimpleMath::Matrix& proj) override;
        void OnRelease() override;

    private:
        // 静的ボーンのワールド座標計算用ヘルパー関数
        void CalculateStaticBoneWorldTransforms(
            std::vector<DirectX::SimpleMath::Matrix>& outWorldTransforms,
            const DirectX::SimpleMath::Matrix& rootTransform);

    };
}
