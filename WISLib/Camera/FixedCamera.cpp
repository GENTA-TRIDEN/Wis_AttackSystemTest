#include "pch.h"
#include "FixedCamera.h"

FixedCamera::FixedCamera(DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Vector3 target)
	: CameraBase(position, WIS::CameraMode::MODE_FIXED),
	m_fixedTarget(target)
{
}

FixedCamera::~FixedCamera()
{
}

void FixedCamera::UniqueInitialize()
{
	m_target = m_fixedTarget;
}

void FixedCamera::UniqueUpdate(DirectX::SimpleMath::Vector3& position, DirectX::SimpleMath::Vector3& target, DirectX::SimpleMath::Vector3& up, const DirectX::Mouse::State& state)
{
	target = m_fixedTarget;
	//UPをPositionとターゲットから計算
	target = { 0.0f, 0.0f, 0.0f };
	up = DirectX::SimpleMath::Vector3::Up;
	position = { 0.0f,0.0f,8.0f };

}

void FixedCamera::UniqueRander()
{
}

void FixedCamera::Uniquefinalize()
{
	DirectX::Mouse::Get().SetMode(DirectX::Mouse::MODE_RELATIVE);

}