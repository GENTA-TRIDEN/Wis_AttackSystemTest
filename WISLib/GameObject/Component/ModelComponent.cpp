#include "pch.h"
#include "ModelComponent.h"
#include "TransformComponent.h"
#include "WISLib/GameObject/GameObject.h"
#include "UserResources.h"

using namespace WIS;
using namespace DirectX;
using namespace DirectX::SimpleMath;

WIS::ModelComponent::ModelComponent() :
    m_DrawModel(true),
    m_stencilState(nullptr),
    m_showWireframe(false),
    m_showNormals(false),
    m_showFaceNormals(false)
{
    m_modelVec.clear();
    m_localMatrices.clear();
    m_textureResources.clear(); 
    VertexList.clear();
    IndexList.clear();
}

WIS::ModelComponent::~ModelComponent()
{
}

int WIS::ModelComponent::AddModel(ID3D11Device* device, ID3D11DeviceContext* context, wchar_t const* fileName, DirectX::EffectFactory* fxFactory)
{
    std::unique_ptr<DirectX::Model> tempModel = DirectX::Model::CreateFromSDKMESH(device, fileName, *fxFactory, DirectX::ModelLoader_Clockwise);

    fxFactory->SetDirectory(L"Resources/textures");
    if (tempModel && !tempModel->meshes.empty()) {
        auto part = tempModel->meshes[0]->meshParts[0].get();

        // 頂点のサイズ（バイト数）を取得
        size_t stride = part->vertexStride;

        // デバッグ出力
        wchar_t msg[256];
        swprintf_s(msg, L"★ Model Vertex Stride: %zu bytes\n", stride);
        OutputDebugStringW(msg);
    }


    if (!tempModel) {
        return -1;
    }

    // Vertex data extraction
    for (const auto& mesh : tempModel->meshes)
    {
        for (const auto& part : mesh->meshParts)
        {
            // --- 1. 頂点バッファの取得 ---
            auto gpuVertexBuffer = part->vertexBuffer.Get();
            D3D11_BUFFER_DESC vbDesc;
            gpuVertexBuffer->GetDesc(&vbDesc);

            //
            D3D11_BUFFER_DESC stagingDesc = vbDesc;
            stagingDesc.Usage = D3D11_USAGE_STAGING;
            stagingDesc.BindFlags = 0;
            stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
            stagingDesc.MiscFlags = 0;

            Microsoft::WRL::ComPtr<ID3D11Buffer> stagingVertexBuffer;
            device->CreateBuffer(&stagingDesc, nullptr, stagingVertexBuffer.GetAddressOf());
            context->CopyResource(stagingVertexBuffer.Get(), gpuVertexBuffer);


            auto gpuIndexBuffer = part->indexBuffer.Get();
            D3D11_BUFFER_DESC ibDesc;
            gpuIndexBuffer->GetDesc(&ibDesc);

            stagingDesc = ibDesc;
            stagingDesc.Usage = D3D11_USAGE_STAGING;
            stagingDesc.BindFlags = 0;
            stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
            stagingDesc.MiscFlags = 0;

            Microsoft::WRL::ComPtr<ID3D11Buffer> stagingIndexBuffer;
            device->CreateBuffer(&stagingDesc, nullptr, stagingIndexBuffer.GetAddressOf());
            context->CopyResource(stagingIndexBuffer.Get(), gpuIndexBuffer);

            // マップ（ロック）
            D3D11_MAPPED_SUBRESOURCE mappedVertex, mappedIndex;
            context->Map(stagingVertexBuffer.Get(), 0, D3D11_MAP_READ, 0, &mappedVertex);
            context->Map(stagingIndexBuffer.Get(), 0, D3D11_MAP_READ, 0, &mappedIndex);

            const uint8_t* pVertexBase = reinterpret_cast<const uint8_t*>(mappedVertex.pData);
            const uint16_t* pIndexBase = reinterpret_cast<const uint16_t*>(mappedIndex.pData);

            // 現在のVertexListのサイズを記録（インデックスのオフセット用）
            uint16_t indexOffset = static_cast<uint16_t>(VertexList.size());


            size_t totalVertexCountInPart = vbDesc.ByteWidth / part->vertexStride;

            for (size_t i = 0; i < totalVertexCountInPart; ++i)
            {
                // 先頭から順番に取得する
                const uint8_t* pV = pVertexBase + (i * part->vertexStride);

                auto vSrc = reinterpret_cast<const DirectX::VertexPositionNormalTexture*>(pV);

                VertexList.push_back(DirectX::VertexPositionColor(vSrc->position, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)));

                m_vertexNormals.push_back({ vSrc->position, vSrc->normal });
            }


            for (size_t i = 0; i < part->indexCount; ++i)
            {
                // インデックスバッファから値を読む
                uint16_t rawIndex = pIndexBase[part->startIndex + i];

                uint16_t finalIndex = static_cast<uint16_t>(indexOffset + part->vertexOffset + rawIndex);

                IndexList.push_back(finalIndex);
            }

            context->Unmap(stagingVertexBuffer.Get(), 0);
            context->Unmap(stagingIndexBuffer.Get(), 0);
        }
    }

    m_modelVec.push_back(std::move(tempModel));
    m_localMatrices.push_back(Matrix::Identity);

    return static_cast<int>(m_modelVec.size() - 1);
}

void WIS::ModelComponent::SetScale(int index, const DirectX::SimpleMath::Vector3& scale)
{
    if (index < 0 || index >= m_localMatrices.size())
    {
        return;
    }
    m_localMatrices[index] *= Matrix::CreateScale(scale);
}

void WIS::ModelComponent::SetRotate(int index, const DirectX::SimpleMath::Vector3& rotate)
{
    if (index < 0 || index >= m_localMatrices.size())
    {
        return;
    }
    m_localMatrices[index] *= Matrix::CreateFromQuaternion(Quaternion::CreateFromYawPitchRoll(rotate.y, rotate.x, rotate.z));
}

void WIS::ModelComponent::SetPosition(int index, const DirectX::SimpleMath::Vector3& position)
{
    if (index < 0 || index >= m_localMatrices.size())
    {
        return;
    }
    m_localMatrices[index] *= Matrix::CreateTranslation(position);

}

void WIS::ModelComponent::SetTexture(int index, ID3D11ShaderResourceView* texture)
{
    if (index < 0 || index >= m_modelVec.size()|| !texture)
    {
        return;
    }

    auto& model = m_modelVec[index];

    // モデル内の全メッシュ・全パーツを巡回してテクスチャをセット
    for (const auto& mesh : model->meshes)
    {
        for (const auto& part : mesh->meshParts)
        {
            // エフェクトを BasicEffect にキャスト
            auto effect = dynamic_cast<DirectX::BasicEffect*>(part->effect.get());
            if (effect)
            {
                // テクスチャをセット
                effect->SetTexture(texture);
                // テクスチャ有効化フラグをオンにする
                effect->SetTextureEnabled(true);
               
            }
        }
    }
}


void WIS::ModelComponent::SetColor(int index, DirectX::SimpleMath::Vector4 color)
{
    // インデックスチェック
    if (index < 0 || index >= m_modelVec.size())
    {
        return;
    }

    auto& model = m_modelVec[index];

    // モデル内の全パーツを巡回
    for (const auto& mesh : model->meshes)
    {
        for (const auto& part : mesh->meshParts)
        {
            auto effect = dynamic_cast<DirectX::BasicEffect*>(part->effect.get());
            if (effect)
            {

                effect->SetTextureEnabled(false);
                effect->SetLightingEnabled(true);

                effect->EnableDefaultLighting();


                effect->SetDiffuseColor(color);

                // 必要なら環境光も少し入れる（真っ黒防止）
                effect->SetAmbientLightColor(DirectX::SimpleMath::Vector3(0.3f, 0.3f, 0.3f));
            }
        }
    }
}


void WIS::ModelComponent::SetTextureFromFile(ID3D11Device* device, int index, const wchar_t* filePath)
{
    // インデックスチェック
    if (index < 0 || index >= m_modelVec.size())
    {
        return;
    }

    // テクスチャ管理用配列が足りなければリサイズ
    if (m_textureResources.size() <= m_modelVec.size())
    {
        m_textureResources.resize(m_modelVec.size());
    }

    // DDSテクスチャの読み込み
    Microsoft::WRL::ComPtr<ID3D11Resource> resource;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;


    HRESULT hr = DirectX::CreateDDSTextureFromFile(device, filePath, resource.GetAddressOf(), srv.GetAddressOf());

    if (SUCCEEDED(hr))
    {
        // メンバ変数に保持（所有権を持つ）
        m_textureResources[index] = srv;

        SetTexture(index, srv.Get());

     
    }
}

void WIS::ModelComponent::OnStart(UserResources* userResources)
{
    auto device = userResources->GetDeviceResources()->GetD3DDevice();

    m_lineEffect = std::make_unique<BasicEffect>(device);
    m_lineEffect->SetVertexColorEnabled(true);

    void const* shaderByteCode;
    size_t byteCodeLength;
    m_lineEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

    device->CreateInputLayout(
        VertexPositionColor::InputElements,
        VertexPositionColor::InputElementCount,
        shaderByteCode,
        byteCodeLength,
        m_lineInputLayout.ReleaseAndGetAddressOf()
    );

    D3D11_RASTERIZER_DESC rasterizerDesc = {};
    rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
    rasterizerDesc.CullMode = D3D11_CULL_NONE;
    rasterizerDesc.DepthClipEnable = true;
    device->CreateRasterizerState(&rasterizerDesc, m_wireframeRasterizerState.GetAddressOf());



    //m_stencilState = userResources->GetCommonStates()->DepthDefault();
}

void WIS::ModelComponent::OnPreUpdate(const float elapsedTime, UserResources* userResources) {}
void WIS::ModelComponent::OnUpdate(const float elapsedTime, UserResources* userResources)
{
    m_showWireframe = false;
}
void WIS::ModelComponent::OnPostUpdate(const float elapsedTime, UserResources* userResources) {

}

void WIS::ModelComponent::OnRender(UserResources* userResources, const Matrix& view, const Matrix& proj)
{
    if (!GetOwner()->CheckActive()) {
        return;
    }

    if (!m_DrawModel)return;

    auto state = userResources->GetCommonStates();
    auto context = userResources->GetDeviceResources()->GetD3DDeviceContext();
    auto primitiveBatch = userResources->GetPrimitiveBatch();

    auto transformComp = GetOwner()->GetComponent<WIS::TransformComponent>();
    if (!transformComp) return;

    Matrix world = transformComp->GetWorldMatrix();


    context->OMSetDepthStencilState(state->DepthDefault(), 0);

    context->OMSetBlendState(state->Opaque(), nullptr, 0xFFFFFFFF);

   // context->OMSetDepthStencilState(m_stencilState, 0);

    // --- Main Model Rendering ---
    for (size_t i = 0; i < m_modelVec.size(); i++)
    {
        if (m_showWireframe) context->RSSetState(m_wireframeRasterizerState.Get());

        Matrix worldMatrix = m_localMatrices[i] * world;
        m_modelVec[i]->Draw(context, *state, worldMatrix, view, proj, false, [&]() {
            context->OMSetDepthStencilState(m_stencilState, 0);
            });
    }

    context->OMSetDepthStencilState(m_stencilState, 0);

}

void WIS::ModelComponent::OnRelease()
{
    m_lineEffect.reset();
    m_lineInputLayout.Reset();
    m_wireframeRasterizerState.Reset();
    m_modelVec.clear();
    m_localMatrices.clear();
    m_textureResources.clear();
    m_vertexNormals.clear();
    VertexList.clear();
    IndexList.clear();
}