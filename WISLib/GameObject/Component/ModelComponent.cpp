#include "pch.h"
#include "ModelComponent.h"
#include "TransformComponent.h"
#include "WISLib/GameObject/GameObject.h"
#include "WISLib/GameObject/Component/AnimationComponent.h"
#include "UserResources.h"

using namespace WIS;
using namespace DirectX;
using namespace DirectX::SimpleMath;

WIS::ModelComponent::ModelComponent() :
    m_DrawModel(true),
    m_stencilState(nullptr),
    m_showWireframe(false), // 初期値を false に設定
    m_showNormals(false),
    m_showFaceNormals(false),
    m_showBones(false)
{
    m_modelVec.clear();
    m_localMatrices.clear();
    m_boneHierarchy.clear();
}

WIS::ModelComponent::~ModelComponent()
{
}

int WIS::ModelComponent::AddModel(ID3D11Device* device, ID3D11DeviceContext* context, wchar_t const* fileName, DirectX::EffectFactory* fxFactory)
{
    std::unique_ptr<DirectX::Model> tempModel = DirectX::Model::CreateFromSDKMESH(device, fileName, *fxFactory, DirectX::ModelLoader_IncludeBones | DirectX::ModelLoader_Clockwise);

    if (!tempModel) {
        return -1;
    }
    // Vertex data extraction
    for (const auto& mesh : tempModel->meshes)
    {
        for (const auto& part : mesh->meshParts)
        {
            auto gpuVertexBuffer = part->vertexBuffer.Get();
            D3D11_BUFFER_DESC vbDesc;
            gpuVertexBuffer->GetDesc(&vbDesc);

            D3D11_BUFFER_DESC stagingDesc = vbDesc;
            stagingDesc.Usage = D3D11_USAGE_STAGING;
            stagingDesc.BindFlags = 0;
            stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
            stagingDesc.MiscFlags = 0;
            Microsoft::WRL::ComPtr<ID3D11Buffer> stagingBuffer;
            device->CreateBuffer(&stagingDesc, nullptr, stagingBuffer.GetAddressOf());

            context->CopyResource(stagingBuffer.Get(), gpuVertexBuffer);

            D3D11_MAPPED_SUBRESOURCE mappedResource;
            context->Map(stagingBuffer.Get(), 0, D3D11_MAP_READ, 0, &mappedResource);
            const uint8_t* pVertexData = reinterpret_cast<const uint8_t*>(mappedResource.pData);

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
            D3D11_MAPPED_SUBRESOURCE mappedIndexResource;
            context->Map(stagingIndexBuffer.Get(), 0, D3D11_MAP_READ, 0, &mappedIndexResource);
            const uint16_t* pIndexData = reinterpret_cast<const uint16_t*>(mappedIndexResource.pData);

            for (size_t i = 0; i < part->indexCount; ++i)
            {
                uint16_t index = pIndexData[part->startIndex + i];
                const uint8_t* pVertex = pVertexData + (part->vertexOffset + index) * part->vertexStride;
                auto vertex = reinterpret_cast<const DirectX::VertexPositionNormalTexture*>(pVertex);
                m_vertexNormals.push_back({ vertex->position, vertex->normal });
            }

            context->Unmap(stagingBuffer.Get(), 0);
            context->Unmap(stagingIndexBuffer.Get(), 0);
        }
    }

    // m_boneHierarchyが空の場合にのみボーン情報を設定（スケルトンは最初のモデルに依存）
    if (m_boneHierarchy.empty() && !tempModel->bones.empty())
    {
        for (size_t i = 0; i < tempModel->bones.size(); ++i)
        {
            const auto& boneData = tempModel->bones[i];

            BoneInfo info;
            info.name = boneData.name.c_str();
            info.parentIndex = boneData.parentIndex;

            info.localTransform = DirectX::SimpleMath::Matrix(tempModel->boneMatrices[i]);
            info.inverseBindPose = DirectX::SimpleMath::Matrix(tempModel->invBindPoseMatrices[i]);

            m_boneHierarchy.push_back(info);
        }
    }

    m_modelVec.push_back(std::move(tempModel));
    m_localMatrices.push_back(Matrix::Identity);

    return static_cast<int>(m_modelVec.size() - 1);
}

const WIS::BoneInfo& WIS::ModelComponent::GetBoneInfo(size_t boneIndex) const
{
    if (boneIndex >= m_boneHierarchy.size())
    {
        throw std::out_of_range("Bone index out of range in GetBoneInfo");
    }
    return m_boneHierarchy[boneIndex];
}

DirectX::ModelBone WIS::ModelComponent::GetBoneByIndex(int modelIndex, size_t boneIndex)
{
    if (modelIndex < 0 || modelIndex >= m_modelVec.size())
    {
        throw std::out_of_range("Model index out of range");
    }
    const auto& model = m_modelVec[modelIndex];
    if (boneIndex >= model->bones.size())
    {
        throw std::out_of_range("Bone index out of range");
    }
    return model->bones[boneIndex];
}


bool WIS::ModelComponent::GetBoneWorldPosition(size_t boneIndex, DirectX::SimpleMath::Vector3& outPosition)
{
    DirectX::SimpleMath::Matrix outMatrix;
    if (!GetBoneWorldMatrix(boneIndex, outMatrix))
    {
        return false;
    }
    outPosition = outMatrix.Translation();
    return true;
}

bool WIS::ModelComponent::GetBoneWorldMatrix(size_t boneIndex, DirectX::SimpleMath::Matrix& outMatrix)
{
    if (m_boneHierarchy.empty() || boneIndex >= m_boneHierarchy.size())
    {
        return false;
    }
    auto transformComp = GetOwner()->GetComponent<WIS::TransformComponent>();
    if (!transformComp)
    {
        return false;
    }

    Matrix rootTransform = m_localMatrices[0] * transformComp->GetWorldMatrix();
    std::vector<Matrix> boneWorldTransforms;
    CalculateStaticBoneWorldTransforms(boneWorldTransforms, rootTransform);

    outMatrix = boneWorldTransforms[boneIndex];

    return true;
}


bool WIS::ModelComponent::GetAnimatedBoneWorldMatrix(size_t boneIndex, DirectX::SimpleMath::Matrix& outMatrix)
{
    auto animComp = GetOwner()->GetComponent<WIS::AnimationComponent>();
    auto transformComp = GetOwner()->GetComponent<WIS::TransformComponent>();

    if (!animComp || !transformComp)
    {
        return false;
    }

    const DirectX::XMMATRIX* skinningMatrices = animComp->GetBoneTransforms();
    const size_t boneCount = animComp->GetBoneCount();

    if (!skinningMatrices || boneIndex >= boneCount || boneIndex >= m_boneHierarchy.size())
    {
        return false;
    }

    Matrix modelRootWorld = m_localMatrices[0] * transformComp->GetWorldMatrix();
    Vector3 ownerScale, ownerPos;
    Quaternion ownerRot;
    if (!modelRootWorld.Decompose(ownerScale, ownerRot, ownerPos))
    {
        // Decomposeが失敗した場合
        Matrix bindPose = m_boneHierarchy[boneIndex].inverseBindPose.Invert();
        Matrix skinningMatrix = Matrix(skinningMatrices[boneIndex]);
        outMatrix = bindPose * skinningMatrix * modelRootWorld;
        return true;
    }

    Matrix bindPose = m_boneHierarchy[boneIndex].inverseBindPose.Invert();
    Matrix skinningMatrix = Matrix(skinningMatrices[boneIndex]);
    Matrix boneModelMatrix = bindPose * skinningMatrix;

    Vector3 finalPos = Vector3::Transform(boneModelMatrix.Translation(), Matrix::CreateScale(ownerScale) * Matrix::CreateFromQuaternion(ownerRot)) + ownerPos;

    Quaternion finalRot = Quaternion::CreateFromRotationMatrix(boneModelMatrix) * ownerRot;

    Vector3 boneScale, dummyPos;
    Quaternion dummyRot;
    boneModelMatrix.Decompose(boneScale, dummyRot, dummyPos);
    Vector3 finalScale = boneScale * ownerScale;

    outMatrix = Matrix::CreateScale(finalScale) * Matrix::CreateFromQuaternion(finalRot) * Matrix::CreateTranslation(finalPos);

    return true;
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

    // --- Animation Data Retrieval ---
    auto animComp = GetOwner()->GetComponent<WIS::AnimationComponent>();
    size_t boneCount = 0;
    const XMMATRIX* boneTransforms = nullptr;
    if (animComp)
    {
        boneCount = animComp->GetBoneCount();
        boneTransforms = animComp->GetBoneTransforms();
    }


    // --- Main Model Rendering ---
    for (size_t i = 0; i < m_modelVec.size(); i++)
    {
        if (m_showWireframe) context->RSSetState(m_wireframeRasterizerState.Get());

        Matrix worldMatrix = m_localMatrices[i] * world;

        if (boneTransforms && boneCount > 0)
        {
            m_modelVec[i]->DrawSkinned(context, *state, boneCount, boneTransforms, worldMatrix, view, proj);
        }
        else
        {
            m_modelVec[i]->Draw(context, *state, worldMatrix, view, proj, false, [&]() {
                context->OMSetDepthStencilState(m_stencilState, 0);
                });
        }

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
    m_boneHierarchy.clear();

}

void WIS::ModelComponent::CalculateStaticBoneWorldTransforms(
    std::vector<DirectX::SimpleMath::Matrix>& outWorldTransforms,
    const DirectX::SimpleMath::Matrix& rootTransform)
{
    size_t boneCount = m_boneHierarchy.size();
    if (boneCount == 0) return;

    outWorldTransforms.resize(boneCount);

    for (size_t i = 0; i < boneCount; ++i)
    {
        const BoneInfo& bone = m_boneHierarchy[i];
        const Matrix& localTransform = bone.localTransform;

        if (bone.parentIndex != -1)
        {
            outWorldTransforms[i] = localTransform * outWorldTransforms[bone.parentIndex];
        }
        else
        {
            outWorldTransforms[i] = localTransform * rootTransform;
        }
    }
}

