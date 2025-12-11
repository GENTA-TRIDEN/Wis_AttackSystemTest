#include "pch.h"
#include "EnemyComponent.h"
#include "WISLib/GameObject/Component/TransformComponent.h"
#include "WISLib/GameObject/Component/CharacterComponent.h"
#include "WISLib/GameObject/GameObject.h"
#include "WISLib/Camera/CameraManager.h"
#include "WISLib/Camera/TPSCamera.h"

#include "UserResources.h"

#include "GameComponent/PlayerComponent.h"




WIS::EnemyComponent::EnemyComponent(DirectX::SimpleMath::Vector3 point1, DirectX::SimpleMath::Vector3 point2)
{
	m_patrolPoints.push_back(point1);
	m_patrolPoints.push_back(point2);
	m_patrolPoints.push_back({ 5,0,0 });

}

WIS::EnemyComponent::~EnemyComponent()
{
}

void WIS::EnemyComponent::OnStart(UserResources* userResources)
{
	// 初期化処理
	m_moveSpeed = 2.2f;

	auto device = userResources->GetDeviceResources()->GetD3DDevice();

	// 1. BasicEffect の作成
	m_basicEffect = std::make_unique<DirectX::BasicEffect>(device);

	// ★重要: DrawGrid は VertexPositionColor を使うので、頂点カラーを有効にする
	m_basicEffect->SetVertexColorEnabled(true);

	// 2. Input Layout の作成
	// エフェクトからシェーダーの情報を取得して、レイアウトを作成します
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

	SelectNextIndex();

}

void WIS::EnemyComponent::OnPreUpdate(const float elapsedTime, UserResources* userResources)
{
}

void WIS::EnemyComponent::OnUpdate(const float elapsedTime, UserResources* userResources)
{
	m_moveSpeed = 2.0f * elapsedTime;

	auto keyboard = DirectX::Keyboard::Get().GetState();


	auto transform = GetOwner()->GetComponent< TransformComponent>();

	DirectX::SimpleMath::Vector3 NowPos = transform->GetPosition();

	switch (m_currentState)
	{
	case WIS::EnemyComponent::state::Search:

		NowPos+=m_moveDirection;
		transform->SetPosition(NowPos);


		if (NowPos.Distance(m_patrolPoints[m_currentPatrolPointIndex], NowPos)<=0.2f) {
			SelectNextIndex();
			m_isCanAttack = true;
		}

		if (NowPos.Distance(m_PlayerPos, NowPos) <= m_PlayerToFindLength&& m_isCanAttack) {
			m_currentState = state::Chase;



		}





		break;
	case WIS::EnemyComponent::state::Chase:
	{
		auto transform = GetOwner()->GetComponent< TransformComponent>();
		DirectX::SimpleMath::Vector3 NowPos = transform->GetPosition();
		DirectX::SimpleMath::Vector3 TargetPos = m_PlayerPos;
		DirectX::SimpleMath::Vector3 MoveVec = TargetPos - NowPos;
		MoveVec.Normalize();
		m_moveDirection = MoveVec * m_moveSpeed;
		DirectX::XMMATRIX rot = DirectX::XMMatrixLookAtLH(NowPos, TargetPos, DirectX::SimpleMath::Vector3::Up);
		DirectX::SimpleMath::Quaternion Qrot = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(rot);
		transform->SetRotation(Qrot);
	}
		//現在の位置からm_PlayerPosへの向きを求める


		NowPos += m_moveDirection;
		transform->SetPosition(NowPos);

		if (NowPos.Distance(m_PlayerPos, NowPos) <= m_PlayerAttackLength) {
			m_currentState = state::Attack;
		}

		if (NowPos.Distance(m_PlayerPos, NowPos) >= m_PlayerToFindLength) {
			m_currentState = state::Search;
			SelectNextIndex();
		}


		break;
	case WIS::EnemyComponent::state::Attack:


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
	
	//m_patrolPointsの描画
	for (DirectX::SimpleMath::Vector3 pos : m_patrolPoints) {

		DirectX::BoundingSphere boundingSphere = DirectX::BoundingSphere({pos}, 0.2f);

		DX::Draw(
			primitiveBatch,
			boundingSphere,
			DirectX::Colors::Black); // 色は適宜見やすいもの (Red, Blueなど) に変更してください



	}


	auto ThisTranc = GetOwner()->GetComponent<WIS::TransformComponent>();

	DirectX::BoundingSphere boundingSphere = DirectX::BoundingSphere({ ThisTranc->GetPosition()}, m_PlayerToFindLength);



	DX::Draw(
		primitiveBatch,
		boundingSphere,
		DirectX::Colors::DarkBlue); // 色は適宜見やすいもの (Red, Blueなど) に変更してください






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

		if (m_isCanAttack) {
			playerCharacter->DecreaseHP(m_AttackLength);
			m_isCanAttack = false;
			m_currentState = state::Search;
			SelectNextIndex();
		}
	}
}

void WIS::EnemyComponent::SelectNextIndex()
{

	//m_patrolPointsがあることが前提
	if (m_patrolPoints.size() > 0) {


		//Indexを加算する
		m_currentPatrolPointIndex++;


		//Sizeを超えた場合は0に戻す
		m_currentPatrolPointIndex = (m_currentPatrolPointIndex > m_patrolPoints.size() - 1) ? 0 : m_currentPatrolPointIndex;


		//現在の位置から次のぽいんとまでの向きを求める
		auto transform = GetOwner()->GetComponent< TransformComponent>();
		DirectX::SimpleMath::Vector3 NowPos = transform->GetPosition();
		DirectX::SimpleMath::Vector3 TargetPos = m_patrolPoints[m_currentPatrolPointIndex];
		DirectX::SimpleMath::Vector3 MoveVec = TargetPos - NowPos;
		MoveVec.Normalize();

		m_moveDirection = MoveVec * m_moveSpeed;


		DirectX::XMMATRIX rot = DirectX::XMMatrixLookAtLH(NowPos, TargetPos, DirectX::SimpleMath::Vector3::Up);
		DirectX::SimpleMath::Quaternion Qrot = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(rot);
		transform->SetRotation(Qrot);

	}

}
