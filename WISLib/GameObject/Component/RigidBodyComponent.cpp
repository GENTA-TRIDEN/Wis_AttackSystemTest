
#include "pch.h"
#include "RigidBodyComponent.h"

#include "TransformComponent.h"
#include "FloorComponent.h"
#include "ModelComponent.h"
#include "WISLib/GameObject/GameObject.h"
#include "UserResources.h"



using namespace WIS;
using namespace DirectX;
using namespace SimpleMath;


WIS::RigidBodyComponent::RigidBodyComponent()
	: m_velocity(DirectX::SimpleMath::Vector3::Zero)  // 速度を0に
	, m_force(DirectX::SimpleMath::Vector3::Zero)     // 力を0に
	, m_useGravity(true)                              // 重力を有効にするか（デフォルトtrue）
{

}

RigidBodyComponent::~RigidBodyComponent() {}

void RigidBodyComponent::OnStart(UserResources* userResources)
{
	auto device = userResources->GetDeviceResources()->GetD3DDevice();

	//BasicEffect の作成
	m_basicEffect = std::make_unique<DirectX::BasicEffect>(device);

	//DrawGrid は VertexPositionColor を使うので、頂点カラーを有効にする
	m_basicEffect->SetVertexColorEnabled(true);

	//Input Layout の作成
	// エフェクトからシェーダーの情報を取得して、レイアウトを作成します
	void const* shaderByteCode;
	size_t byteCodeLength;
	m_basicEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	// VertexPositionColor 用のレイアウトを作成して保持
	device->CreateInputLayout(
		DirectX::VertexPositionColor::InputElements,
		DirectX::VertexPositionColor::InputElementCount,
		shaderByteCode,
		byteCodeLength,
		m_inputLayout.ReleaseAndGetAddressOf()
	);
}

void WIS::RigidBodyComponent::OnUpdate(const float elapsedTime, UserResources* userResources)
{
	//TransformComponentのnullCheck
	auto transform = GetOwner()->GetComponent<WIS::TransformComponent>();
	if (!transform) return;



	// F = mg (力 = 質量 * 重力加速度)
	//if (m_useGravity)
	if (m_useGravity)
	{


		// Y軸下向きに力を加える
		DirectX::SimpleMath::Vector3 gravityForce(0.0f, -m_value.gravity * m_value.mass, 0.0f);
		AddForce(gravityForce);
	}

	//加速度を計算する
	DirectX::SimpleMath::Vector3 acceleration = DirectX::SimpleMath::Vector3::Zero;

	//0じゃないかチェックする
	if (m_value.mass > 0.0001f)
	{
		//力を計算する
		acceleration = m_force / m_value.mass;
	}



	// 前のフレームの速度に、加速度を足す
	m_velocity += acceleration * elapsedTime;



	//抵抗を適用させる
	if (m_value.drag > 0.0f)
	{
		// 係数がマイナスにならないようにClampする
		float dragFactor = 1.0f - (m_value.drag * elapsedTime);
		if (dragFactor < 0.0f) dragFactor = 0.0f;

		m_velocity *= dragFactor;
	}

	//位置を更新する
	transform->Translate(m_velocity * elapsedTime);

	//力のリセット
	m_force = DirectX::SimpleMath::Vector3::Zero;
}

void WIS::RigidBodyComponent::OnCollision(std::shared_ptr<WIS::GameObject> otherObject)
{
    // --- 1. 必要な情報の取得 ---
    auto myTrans = GetOwner()->GetComponent<TransformComponent>();
    auto otherModelComp = otherObject->GetComponent<ModelComponent>();
    auto otherTrans = otherObject->GetComponent<TransformComponent>();

    // ModelComponentなどがない場合は処理しない
    if (!myTrans || !otherModelComp || !otherTrans || otherModelComp->VertexList.empty()) return;

    // --- 2. レイ（Ray）の設定 ---
    Vector3 currentPos = myTrans->GetPosition();

    // A. 床判定用レイ（真下）
    Vector3 downRayOrigin = currentPos + Vector3(0.0f, 0.5f, 0.0f); // 腰あたりから
    Vector3 downRayDir = Vector3(0.0f, -1.0f, 0.0f);                // 真下

    // B. 壁判定用レイ（進行方向）
    Vector3 velocityDir = m_velocity;
    velocityDir.y = 0; // 壁判定は水平方向のみ見る
    bool isMoving = velocityDir.LengthSquared() > 0.001f; // 動いているか？

    if (isMoving) {
        velocityDir.Normalize();
    }

    Vector3 forwardRayOrigin = currentPos + Vector3(0.0f, 0.5f, 0.0f); // 腰あたりから
    Vector3 forwardRayDir = velocityDir; // 進行方向

    // 判定結果を保存する変数
    float nearestGroundDist = FLT_MAX; // 床までの距離
    bool hitGround = false;            // 床に当たったか

    float nearestWallDist = FLT_MAX;   // 壁までの距離
    bool hitWall = false;              // 壁に当たったか
    Vector3 wallNormal = Vector3::Zero; // 壁の法線

    // 行列計算
    Matrix objWorld = otherTrans->GetWorldMatrix();
    Matrix modelLocal = otherModelComp->GetLocalMatrix(0);
    Matrix finalWorld = modelLocal * objWorld;

    const auto& indices = otherModelComp->IndexList;
    const auto& vertices = otherModelComp->VertexList;

    // --- 3. 全ポリゴンに対してレイ判定ループ ---
    for (size_t i = 0; i < indices.size(); i += 3)
    {
        if (i + 2 >= indices.size()) break;

        // 三角形の頂点をワールド座標に変換
        Vector3 v0 = Vector3::Transform(vertices[indices[i]].position, finalWorld);
        Vector3 v1 = Vector3::Transform(vertices[indices[i + 1]].position, finalWorld);
        Vector3 v2 = Vector3::Transform(vertices[indices[i + 2]].position, finalWorld);

        // --- A. 床（Down）判定 ---
        float dist = 0.0f;
        if (DirectX::TriangleTests::Intersects(downRayOrigin, downRayDir, v0, v1, v2, dist))
        {
            if (dist < nearestGroundDist && dist < 1.0f) // 足元1.0f以内
            {
                nearestGroundDist = dist;
                hitGround = true;

                // ここで法線チェックして「急な坂（壁）」を弾く処理を入れても良いですが、
                // 今回は「壁判定」を別に行うので、単純に一番近い床を探します。
            }
        }

        // --- B. 壁（Forward）判定 ---
        // 動いている時だけチェック
        if (isMoving)
        {
            if (DirectX::TriangleTests::Intersects(forwardRayOrigin, forwardRayDir, v0, v1, v2, dist))
            {
                // 体の半径分（例: 0.5f）より近くに壁があるか？
                if (dist < nearestWallDist && dist < 0.5f)
                {
                    nearestWallDist = dist;
                    hitWall = true;

                    // 壁の法線を計算
                    Vector3 edge1 = v1 - v0;
                    Vector3 edge2 = v2 - v0;
                    Vector3 normal = edge1.Cross(edge2);
                    normal.Normalize();
                    wallNormal = normal;
                }
            }
        }
    }

    // --- 4. 衝突後の処理（レスポンス） ---

    // A. 壁処理（反射・スライド）
// A. 壁処理（水平方向のみのスライド）
	if (hitWall)
	{
		// 壁の法線と、進行方向の内積をとる（向かい合っているか？）
		if (wallNormal.Dot(forwardRayDir) < 0.0f)
		{

			// Y成分を0にして、強制的に垂直な壁として扱う
			Vector3 flatNormal = wallNormal;
			flatNormal.y = 0.0f;

			// 正規化（Yを消すと長さが変わるため、再度1.0に戻す）
			if (flatNormal.LengthSquared() > 0.0001f)
			{
				flatNormal.Normalize();


				Vector3 pushBack = flatNormal * m_velocity.Dot(flatNormal);
				m_velocity -= pushBack;

				float penetrationDepth = 0.5f - nearestWallDist; // 0.5fは体の半径
				if (penetrationDepth > 0.0f)
				{
					// 押し出し方向も flatNormal を使う
					Vector3 pushVec = flatNormal * penetrationDepth;
					myTrans->SetPosition(currentPos + pushVec);

					// 位置を更新したのでcurrentPosも更新
					currentPos = myTrans->GetPosition();
				}
			}
		}
	}

    // B. 床処理（接地）
	if (hitGround)
	{
		// 地面の高さ（レイの衝突地点のY座標）
		float groundY = downRayOrigin.y - nearestGroundDist;

		//調整用
		float heightOffset = m_heightOffset;
		float finalCharacterY = groundY + heightOffset;

		// 接地判定（Y速度が下向き、かつ地面に近い）
		if (m_velocity.y <= 0)
		{
			Vector3 newPos = currentPos;
			newPos.y = finalCharacterY; // 補正した高さを代入
			myTrans->SetPosition(newPos);

			m_velocity.y = 0;
			m_isGrounded = true;
		}
	}
    else
    {
        m_isGrounded = false;
    }
}
void WIS::RigidBodyComponent::OnRender(UserResources* userResources, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{

#ifdef _DEBUG

	auto font = userResources->GetDebugFont();
	auto context = userResources->GetDeviceResources()->GetD3DDeviceContext();
	auto device = userResources->GetDeviceResources()->GetD3DDevice();

	// プリミティブバッチを取得
	DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* primitiveBatch = userResources->GetPrimitiveBatch();

	auto Trans = GetOwner()->GetComponent<TransformComponent>();
	if (!m_basicEffect) {
		return;
	}

	m_basicEffect->SetWorld(DirectX::SimpleMath::Matrix::Identity);

	m_basicEffect->SetView(view);
	m_basicEffect->SetProjection(proj);
	m_basicEffect->Apply(context);

	context->IASetInputLayout(m_inputLayout.Get());

	primitiveBatch->Begin();



	DirectX::SimpleMath::Vector3 worldPos = Trans->GetPosition();

	DX::DrawRay(
		primitiveBatch,
		worldPos,
		m_velocity,
		false,
		DirectX::Colors::LightGreen);


	DirectX::SimpleMath::Vector3 spherePos = worldPos + m_velocity;
	BoundingSphere boundingSphere = BoundingSphere(spherePos, 0.2f);

	DX::Draw(
		primitiveBatch,
		boundingSphere,
		DirectX::Colors::LightGreen);

	primitiveBatch->End();



#endif
}





