#include "pch.h"
#include "EnemyComponent.h"
#include "WISLib/GameObject/Component/TransformComponent.h"
#include "WISLib/GameObject/Component/CharacterComponent.h"
#include "WISLib/GameObject/Component/RigidBodyComponent.h" // RigidBodyを使うために必須
#include "WISLib/GameObject/GameObject.h"
#include "WISLib/Camera/CameraManager.h"
#include "WISLib/Camera/TPSCamera.h"
#include "UserResources.h"
#include "GameComponent/PlayerComponent.h"

using namespace WIS;
using namespace DirectX;
using namespace SimpleMath;

WIS::EnemyComponent::EnemyComponent(DirectX::SimpleMath::Vector3 point1, DirectX::SimpleMath::Vector3 point2)
{
	m_patrolPoints.push_back(point1);
	m_patrolPoints.push_back(point2);
}

WIS::EnemyComponent::~EnemyComponent()
{
}

void WIS::EnemyComponent::OnStart(UserResources* userResources)
{
	// 初期化処理
	// RigidBodyを使う場合、速度は「1秒間に進む距離」で指定します
	m_moveSpeed = 2.0f;

	auto device = userResources->GetDeviceResources()->GetD3DDevice();

	// 1. BasicEffect の作成
	m_basicEffect = std::make_unique<DirectX::BasicEffect>(device);

	// DrawGrid は VertexPositionColor を使うので、頂点カラーを有効にする
	m_basicEffect->SetVertexColorEnabled(true);

	void const* shaderByteCode;
	size_t byteCodeLength;
	m_basicEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
	m_currentState = state::Search;

	// VertexPositionColor 用のレイアウトを作成して保持
	device->CreateInputLayout(
		DirectX::VertexPositionColor::InputElements,
		DirectX::VertexPositionColor::InputElementCount,
		shaderByteCode,
		byteCodeLength,
		m_inputLayout.ReleaseAndGetAddressOf()
	);

	// 最初の移動先を設定
	SelectNextIndex();
}

void WIS::EnemyComponent::OnPreUpdate(const float elapsedTime, UserResources* userResources)
{
}

void WIS::EnemyComponent::OnUpdate(const float elapsedTime, UserResources* userResources)
{
	// コンポーネント取得
	auto transform = GetOwner()->GetComponent<TransformComponent>();
	auto rigidBody = GetOwner()->GetComponent<RigidBodyComponent>();

	// 必要なコンポーネントがない場合は処理しない
	if (!transform || !rigidBody) return;

	DirectX::SimpleMath::Vector3 NowPos = transform->GetPosition();

	// 現在の速度を取得（Y成分＝重力落下速度 を維持するため）
	// ※RigidBodyComponentに GetVelocity() / SetVelocity() が必要です
	Vector3 currentVelocity = rigidBody->GetVelocity();
	Vector3 nextVelocity = Vector3::Zero;

	switch (m_currentState)
	{
	case WIS::EnemyComponent::state::Search:
	{
		// --- 巡回移動 ---

		// 方向(m_moveDirection) に スピードを掛ける
		nextVelocity = m_moveDirection * m_moveSpeed;

		// Y軸（重力）は元のままにする
		nextVelocity.y = currentVelocity.y;

		// 速度を適用
		rigidBody->ChangeVelocity(nextVelocity);

		// 目標地点への距離チェック (XZ平面距離で見るのが理想ですが、簡易的に距離で)
		// 到着判定を少し緩く(0.5f)しておくと、通り過ぎて振動するのを防げます
		if (Vector3::Distance(NowPos, m_patrolPoints[m_currentPatrolPointIndex]) <= 0.5f) {
			SelectNextIndex();
			m_isCanAttack = true;
		}

		// プレイヤー発見チェック
		if (Vector3::Distance(NowPos, m_PlayerPos) <= m_PlayerToFindLength && m_isCanAttack) {
			m_currentState = state::Chase;
		}
	}
	break;

	case WIS::EnemyComponent::state::Chase:
	{
		// --- 追跡移動 ---
		DirectX::SimpleMath::Vector3 TargetPos = m_PlayerPos;

		// ターゲットへのベクトル算出
		DirectX::SimpleMath::Vector3 MoveVec = TargetPos - NowPos;

		// 空を飛ばないようにY成分を無視して正規化する（水平移動のみ）
		MoveVec.y = 0;
		if (MoveVec.LengthSquared() > 0.0001f) {
			MoveVec.Normalize();
		}

		// 速度計算
		nextVelocity = MoveVec * m_moveSpeed;
		nextVelocity.y = currentVelocity.y; // 重力維持
		rigidBody->ChangeVelocity(nextVelocity);

		// 回転 (Transformを直接回転させてOK)
		// LookAtLHはビュー行列用なので、モデルの向きと逆になる場合があります。
		// もし敵が後ろ向きに進むなら、LookAtの引数順序を変えるか、LookRotation関数を自作して使います。
		// ここでは元のロジックを維持します。
		DirectX::XMMATRIX rot = DirectX::XMMatrixLookAtLH(NowPos, TargetPos, DirectX::SimpleMath::Vector3::Up);
		DirectX::SimpleMath::Quaternion Qrot = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(rot);
		transform->SetRotation(Qrot);

		// 攻撃範囲チェック
		if (Vector3::Distance(NowPos, m_PlayerPos) <= m_PlayerAttackLength) {
			m_currentState = state::Attack;
			// 攻撃時は停止（慣性を残さないならXZを0に）
			rigidBody->ChangeVelocity(Vector3(0, currentVelocity.y, 0));
		}

		// 見失うチェック（距離が離れたら）
		if (Vector3::Distance(NowPos, m_PlayerPos) >= m_PlayerToFindLength) {
			m_currentState = state::Search;
			SelectNextIndex(); // 最寄りのパトロール地点へ向かう等の処理にするのがベター
		}
	}
	break;

	case WIS::EnemyComponent::state::Attack:
	{
		// 攻撃中は移動停止（重力のみ適用）
		rigidBody->ChangeVelocity(Vector3(0, currentVelocity.y, 0));
	}
	break;

	default:
		break;
	}
}

void WIS::EnemyComponent::OnPostUpdate(const float elapsedTime, UserResources* userResources)
{
}

void WIS::EnemyComponent::OnRender(UserResources* userResources, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{
	auto font = userResources->GetDebugFont();
	auto context = userResources->GetDeviceResources()->GetD3DDeviceContext();
	auto device = userResources->GetDeviceResources()->GetD3DDevice();

	// プリミティブバッチを取得
	DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* primitiveBatch = userResources->GetPrimitiveBatch();

	if (!m_basicEffect) {
		return;
	}

	font->AddString((L"state:" + std::to_wstring(static_cast<int>(m_currentState))).c_str(), { 800.0f, 50.0f });

	DirectX::FXMMATRIX value = DirectX::SimpleMath::Matrix::Identity;
	m_basicEffect->SetWorld(value);

	m_basicEffect->SetView(view);
	m_basicEffect->SetProjection(proj);
	m_basicEffect->Apply(context);

	context->IASetInputLayout(m_inputLayout.Get());

	primitiveBatch->Begin();

	// m_patrolPointsの描画
	for (DirectX::SimpleMath::Vector3 pos : m_patrolPoints) {
		DirectX::BoundingSphere boundingSphere = DirectX::BoundingSphere({ pos }, 0.2f);
		DX::Draw(
			primitiveBatch,
			boundingSphere,
			DirectX::Colors::Black);
	}

	auto ThisTranc = GetOwner()->GetComponent<WIS::TransformComponent>();
	DirectX::BoundingSphere boundingSphere = DirectX::BoundingSphere({ ThisTranc->GetPosition() }, m_PlayerToFindLength);

	DX::Draw(
		primitiveBatch,
		boundingSphere,
		DirectX::Colors::DarkBlue);

	primitiveBatch->End();
}

void WIS::EnemyComponent::OnRelease()
{
}

void WIS::EnemyComponent::OnCollision(std::shared_ptr<WIS::GameObject> otherObject)
{
	auto player = otherObject->GetComponent<WIS::PlayerComponent>();
	if (player) {
		auto playerCharacter = otherObject->GetComponent<WIS::CharacterComponent>();

		// 攻撃状態でなくても触れたらダメージを与えるロジックになっています。
		// 状態依存にするなら if (m_currentState == state::Attack) などを追加してください。
		if (m_isCanAttack && playerCharacter) {
			playerCharacter->DecreaseHP(m_AttackLength);
			m_isCanAttack = false;
			m_currentState = state::Search;
			SelectNextIndex();
		}
	}
}

void WIS::EnemyComponent::SelectNextIndex()
{
	// m_patrolPointsがあることが前提
	if (m_patrolPoints.size() > 0) {
		// Indexを加算する
		m_currentPatrolPointIndex++;

		// Sizeを超えた場合は0に戻す
		if (m_currentPatrolPointIndex >= m_patrolPoints.size()) {
			m_currentPatrolPointIndex = 0;
		}

		// 現在の位置から次のポイントまでの向きを求める
		auto transform = GetOwner()->GetComponent<TransformComponent>();
		if (!transform) return;

		DirectX::SimpleMath::Vector3 NowPos = transform->GetPosition();
		DirectX::SimpleMath::Vector3 TargetPos = m_patrolPoints[m_currentPatrolPointIndex];

		DirectX::SimpleMath::Vector3 MoveVec = TargetPos - NowPos;

		// 水平移動のみにする場合
		MoveVec.y = 0;

		if (MoveVec.LengthSquared() > 0.0001f) {
			MoveVec.Normalize();
		}

		// 方向ベクトルを保存（UpdateでこれにSpeedを掛けてRigidBodyに渡す）
		m_moveDirection = MoveVec;

		// 向きを変更
		DirectX::XMMATRIX rot = DirectX::XMMatrixLookAtLH(NowPos, TargetPos, DirectX::SimpleMath::Vector3::Up);
		DirectX::SimpleMath::Quaternion Qrot = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(rot);
		transform->SetRotation(Qrot);
	}
}