#include "pch.h"
#include "PolygonComponent.h"
#include "UserResources.h"
#include <BufferHelpers.h>

WIS::PolygonComponent::PolygonComponent():
    m_inputLayout{nullptr}
{

}

WIS::PolygonComponent::~PolygonComponent()
{

}

void WIS::PolygonComponent::OnStart(UserResources* userResources)
{
    auto device = userResources->GetDeviceResources()->GetD3DDevice();


    m_basicEffect = std::make_unique<DirectX::BasicEffect>(device);
    m_basicEffect->SetVertexColorEnabled(true);

    void const* shaderByteCode;
    size_t byteCodeLength;
    m_basicEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

    device->CreateInputLayout(
        DirectX::VertexPositionColor::InputElements,
        DirectX::VertexPositionColor::InputElementCount,
        shaderByteCode,
        byteCodeLength,
        m_inputLayout.ReleaseAndGetAddressOf()
    );

    std::vector<DirectX::VertexPositionColor> VertexList;
    std::vector<uint16_t> IndexList;


    VertexList = {


    //{ DirectX::XMFLOAT3(-2.0f+4.0f,0,-2.0f), DirectX::XMFLOAT4(1,0,0,1)},//0->赤
    //{ DirectX::XMFLOAT3(2.0f + 4.0f,0,-2.0f), DirectX::XMFLOAT4(0,1,0,1)},//1->緑
    //{ DirectX::XMFLOAT3(2.0f + 4.0f,0, 2.0f), DirectX::XMFLOAT4(0,0,1,1)},//2->青
    //{ DirectX::XMFLOAT3(-2.0f + 4.0f,0, 2.0f), DirectX::XMFLOAT4(0,0,0,1)}//3->黒

    };

    IndexList = { 2,1,0,0,3,2 };

    m_IndexList = IndexList;
    m_VertexList = VertexList;


    AddPolygon(
        { DirectX::XMFLOAT3(-2.0f,0,-2.0f), DirectX::XMFLOAT4(1,0,0,1) },//0->赤
        { DirectX::XMFLOAT3(2.0f,0,-2.0f), DirectX::XMFLOAT4(0,1,0,1) },//1->緑
        { DirectX::XMFLOAT3(2.0f,0, 2.0f), DirectX::XMFLOAT4(0,0,1,1) },//2->青
        { DirectX::XMFLOAT3(-2.0f,0, 2.0f), DirectX::XMFLOAT4(0,0,0,1) }//3->黒
    );
    AddPolygon(
        { DirectX::XMFLOAT3(6.0f,0, 2.0f), DirectX::XMFLOAT4(0,0,1,1) },//2->青
        { DirectX::XMFLOAT3(2.0f,0,-2.0f), DirectX::XMFLOAT4(1,0,0,1) },//0->赤
        { DirectX::XMFLOAT3(6.0f,0,-2.0f), DirectX::XMFLOAT4(0,1,0,1) },//1->緑
        { DirectX::XMFLOAT3(2.0f,0, 2.0f), DirectX::XMFLOAT4(0,0,0,1) }//3->黒
    );



    //int size = IndexList.size();
    ////for (int i = 0; i < size; i++)
    ////{
    ////    IndexList.push_back(IndexList[i] + 4);
    ////}

}

void WIS::PolygonComponent::OnUpdate(const float elapsedTime, UserResources* userResources)
{



}

void WIS::PolygonComponent::OnRender(UserResources* userResources, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{
	auto context = userResources->GetDeviceResources()->GetD3DDeviceContext();
    auto batch = userResources->GetPrimitiveBatch();
    auto states = userResources->GetCommonStates();

    DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::Identity;

    // 行列の適用
    m_basicEffect->SetWorld(world);
    m_basicEffect->SetView(view);
    m_basicEffect->SetProjection(proj);

    // エフェクトの適用
    context->OMSetBlendState(states->Opaque(), nullptr, 0xFFFFFFFF);
    context->OMSetDepthStencilState(states->DepthNone(), 0);
    context->RSSetState(states->CullNone());

    m_basicEffect->Apply(context);
    context->IASetInputLayout(m_inputLayout.Get());
    context->RSSetState(states->CullClockwise());

    // 描画開始
    batch->Begin();

    // DrawIndexedの使用
    // 引数: トポロジー, インデックス配列のポインタ, インデックス数, 頂点配列のポインタ, 頂点数
    batch->DrawIndexed(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
        m_IndexList.data(),
        m_IndexList.size(),
        m_VertexList.data(),
        m_VertexList.size()
    );

    // 描画終了
    batch->End();
}

void WIS::PolygonComponent::AddPolygon(DirectX::VertexPositionColor p1, DirectX::VertexPositionColor p2, DirectX::VertexPositionColor p3, DirectX::VertexPositionColor p4)
{

    int m_VListSize = m_VertexList.size();
    std::vector<DirectX::VertexPositionColor> VertexList;
    VertexList.push_back(p1);
    VertexList.push_back(p2);
    VertexList.push_back(p3);
    VertexList.push_back(p4);

    std::sort(VertexList.begin(), VertexList.end(),
        [](DirectX::VertexPositionColor const& lhs, DirectX::VertexPositionColor const& rhs) {
        return lhs.position.z < rhs.position.z;
        });


    //std::sort(VertexList.begin(), VertexList.begin()+1,
    //    [](DirectX::VertexPositionColor const& lhs, DirectX::VertexPositionColor const& rhs) {
    //        return lhs.position.x > rhs.position.x;
    //    });

    if (VertexList[0].position.x > VertexList[1].position.x) {
        std::swap(VertexList[0], VertexList[1]);
    } 
    
    if (VertexList[2].position.x < VertexList[3].position.x) {
        std::swap(VertexList[2], VertexList[3]);
    }

    //std::sort(VertexList.begin()+2, VertexList.begin() + 3,
    //    [](DirectX::VertexPositionColor const& lhs, DirectX::VertexPositionColor const& rhs) {
    //        return lhs.position.x < rhs.position.x;
    //    });

    //if(VertexList[0].position.x> VertexList[1].position.x)
    //VertexList;



    //リストに追加
    m_VertexList.push_back(VertexList[0]);
    m_VertexList.push_back(VertexList[1]);
    m_VertexList.push_back(VertexList[2]);
    m_VertexList.push_back(VertexList[3]);

    //
    std::vector<uint16_t> IndexList = { 2,1,0,0,3,2 };



    int size = IndexList.size();
    for (int i = 0; i < 6; i++)
    {
        IndexList.push_back(IndexList[i] + m_VListSize);
    }


    m_IndexList = IndexList;

}
