#pragma once
#include "ComponentBase.h"
#include "Model.h"
#include <Effects.h>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <DirectXColors.h> // 色情報(Colors::White)用
#include <wrl/client.h>
#include <vector>
#include <string>
#include <DDSTextureLoader.h> 

namespace WIS {

    class ModelComponent : public WIS::ComponentBase
    {
    public:
        // 当たり判定や解析用に抽出したデータ
        std::vector<DirectX::VertexPositionColor> VertexList;
        std::vector<uint16_t> IndexList;

    private:
        DirectX::SimpleMath::Matrix m_worldMatrix = DirectX::SimpleMath::Matrix::Identity;
        std::vector<DirectX::SimpleMath::Matrix> m_localMatrices;
        std::vector<std::unique_ptr<DirectX::Model>> m_modelVec;


        std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_textureResources;

        bool m_DrawModel;


        bool m_showNormals = false;
        bool m_showFaceNormals = false;
        bool m_showWireframe = true;

        //Textureを適応するか
		bool m_isUseTexture = false;

        std::wstring m_names;

        // Data and States
        // デバッグ表示用に残している法線データ
        std::vector<std::pair<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3>> m_vertexNormals;

        std::unique_ptr<DirectX::BasicEffect> m_lineEffect;
        Microsoft::WRL::ComPtr<ID3D11InputLayout> m_lineInputLayout;
        Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_wireframeRasterizerState;

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
        void SetPosition(int index, const DirectX::SimpleMath::Vector3& position);


        void SetTexture(int index, ID3D11ShaderResourceView* texture);

        void SetColors(int index, DirectX::SimpleMath::Vector4 color);

        void SetTextureFromFile(ID3D11Device* device, int index, const wchar_t* filePath);

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

        // --- Debug Control Functions ---
        void ShowNormals(bool show) { m_showNormals = show; }
        void ShowFaceNormals(bool show) { m_showFaceNormals = show; }
        void ShowWireframe(bool show) { m_showWireframe = show; }

        void SetDepthStencilState(ID3D11DepthStencilState* state) {
            m_stencilState = state;
        }

        const DirectX::SimpleMath::Matrix& GetLocalMatrix(int index) const
        {
            // 範囲外なら単位行列（回転なし）を返す安全策
            if (index < 0 || index >= m_localMatrices.size())
            {
                static DirectX::SimpleMath::Matrix identity = DirectX::SimpleMath::Matrix::Identity;
                return identity;
            }
            return m_localMatrices[index];
        }

        std::vector<DirectX::VertexPositionColor>& GetVertices() { return VertexList; }
        std::vector<uint16_t>& GetIndices() { return IndexList; }

		//Textureを使用するかどうかの設定
        void SetUseTexture(bool isUse,int index);

    public:
        void OnStart(UserResources* userResources) override;
        void OnPreUpdate(const float elapsedTime, UserResources* userResources) override;
        void OnUpdate(const float elapsedTime, UserResources* userResources) override;
        void OnPostUpdate(const float elapsedTime, UserResources* userResources) override;
        void OnRender(UserResources* userResources,
            const DirectX::SimpleMath::Matrix& view,
            const DirectX::SimpleMath::Matrix& proj) override;
        void OnRelease() override;

    };
}