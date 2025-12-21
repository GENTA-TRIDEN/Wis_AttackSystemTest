#pragma once
#include "ComponentBase.h"
#include "WISLib/WISLibTags.h"

namespace WIS {


class PolygonComponent:public ComponentBase 
{
private:
    //頂点リスト
    std::vector<DirectX::VertexPositionColor> m_VertexList;
    //インデックスリスト
    std::vector<uint16_t> m_IndexList;


    // デバッグ描画用のBasicEffect
    std::unique_ptr<DirectX::BasicEffect> m_basicEffect = nullptr;
    //入力レイアウト
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

public:
    PolygonComponent();
    ~PolygonComponent();


    void OnStart(UserResources* userResources)override;
    //void OnPreUpdate(const float elapsedTime, UserResources* userResources) override;
    void OnUpdate(const float elapsedTime, UserResources* userResources) override;
    //void OnPostUpdate(const float elapsedTime, UserResources* userResources) override;
    void OnRender(UserResources* userResources, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj) override;

    //四角polygonを生成
    void AddPolygon(
        DirectX::VertexPositionColor p1,
        DirectX::VertexPositionColor p2,
        DirectX::VertexPositionColor p3,
        DirectX::VertexPositionColor p4);


    std::vector<DirectX::VertexPositionColor>& GetVertices() { return m_VertexList; }
    std::vector<uint16_t>& GetIndices() { return m_IndexList; }

};




}

