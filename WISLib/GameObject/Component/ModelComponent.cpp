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
    m_showWireframe(false), // 初期値を false に設定
    m_showNormals(false),
    m_showFaceNormals(false)
{
    m_modelVec.clear();
    m_localMatrices.clear();
    VertexList.clear();
    IndexList.clear();
}

WIS::ModelComponent::~ModelComponent()
{
}

int WIS::ModelComponent::AddModel(ID3D11Device* device, ID3D11DeviceContext* context, wchar_t const* fileName, DirectX::EffectFactory* fxFactory)
{
    // アニメーション用フラグを削除
    // 2019版でもClockwiseは使用可能です
    std::unique_ptr<DirectX::Model> tempModel = DirectX::Model::CreateFromSDKMESH(device, fileName, *fxFactory, DirectX::ModelLoader_Clockwise);

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

            // ステージングバッファ作成 (CPU読み取り用)
            D3D11_BUFFER_DESC stagingDesc = vbDesc;
            stagingDesc.Usage = D3D11_USAGE_STAGING;
            stagingDesc.BindFlags = 0;
            stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
            stagingDesc.MiscFlags = 0;

            Microsoft::WRL::ComPtr<ID3D11Buffer> stagingVertexBuffer;
            device->CreateBuffer(&stagingDesc, nullptr, stagingVertexBuffer.GetAddressOf());
            context->CopyResource(stagingVertexBuffer.Get(), gpuVertexBuffer);

            // --- 2. インデックスバッファの取得 ---
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

            // --- 3. データの読み出しと変換 ---

            // マップ（ロック）
            D3D11_MAPPED_SUBRESOURCE mappedVertex, mappedIndex;
            context->Map(stagingVertexBuffer.Get(), 0, D3D11_MAP_READ, 0, &mappedVertex);
            context->Map(stagingIndexBuffer.Get(), 0, D3D11_MAP_READ, 0, &mappedIndex);

            const uint8_t* pVertexBase = reinterpret_cast<const uint8_t*>(mappedVertex.pData);
            const uint16_t* pIndexBase = reinterpret_cast<const uint16_t*>(mappedIndex.pData);

            // 現在のVertexListのサイズを記録（インデックスのオフセット用）
            uint16_t indexOffset = static_cast<uint16_t>(VertexList.size());

            // ★修正点1: 頂点数をバッファサイズとストライドから計算する
            // part->vertexCount がない2019版向けの対応
            size_t totalVertexCountInPart = vbDesc.ByteWidth / part->vertexStride;

            // A. 頂点の抽出 (バッファ内の全頂点をコピー)
            for (size_t i = 0; i < totalVertexCountInPart; ++i)
            {
                // 先頭から順番に取得する
                const uint8_t* pV = pVertexBase + (i * part->vertexStride);

                // 座標と法線を取得 (キャストして読み取り)
                // SDKMESH形式は通常 VertexPositionNormalTexture を使用
                auto vSrc = reinterpret_cast<const DirectX::VertexPositionNormalTexture*>(pV);

                // 公開用リストに追加：VertexPositionColor に変換
                VertexList.push_back(DirectX::VertexPositionColor(vSrc->position, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)));

                // デバッグ用リストに追加
                m_vertexNormals.push_back({ vSrc->position, vSrc->normal });
            }

            // B. インデックスの抽出
            for (size_t i = 0; i < part->indexCount; ++i)
            {
                // インデックスバッファから値を読む
                uint16_t rawIndex = pIndexBase[part->startIndex + i];

                // ★修正点2: インデックスの計算
                // rawIndex: インデックスバッファの値 (0, 1, 2...)
                // part->vertexOffset: GPUが参照する頂点の開始位置補正 (BaseVertexLocation)
                // indexOffset: 今回結合したVertexListの開始位置
                // これらを足すことで、VertexList上の正しい要素を指すようにする
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

    m_stencilState = userResources->GetCommonStates()->DepthDefault();
}

void WIS::ModelComponent::OnPreUpdate(const float elapsedTime, UserResources* userResources) {}
void WIS::ModelComponent::OnUpdate(const float elapsedTime, UserResources* userResources)
{
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

   
    // --- Main Model Rendering ---
    for (size_t i = 0; i < m_modelVec.size(); i++)
    {
        if (m_showWireframe) context->RSSetState(m_wireframeRasterizerState.Get());

        Matrix worldMatrix = m_localMatrices[i] * world;
        m_modelVec[i]->Draw(context, *state, worldMatrix, view, proj, false, [&]() {
            context->OMSetDepthStencilState(m_stencilState, 0);
            });
    }

}

void WIS::ModelComponent::OnRelease()
{
    m_lineEffect.reset();
    m_lineInputLayout.Reset();
    m_wireframeRasterizerState.Reset();
    m_modelVec.clear();
    m_localMatrices.clear();
    m_vertexNormals.clear();
    VertexList.clear();
    IndexList.clear();
}