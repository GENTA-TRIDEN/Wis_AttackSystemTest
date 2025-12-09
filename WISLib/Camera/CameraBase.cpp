#include "pch.h"
#include "CameraBase.h"

WIS::CameraBase::CameraBase(DirectX::SimpleMath::Vector3 position, WIS::CameraMode type)
	: m_position(position),
	m_target(DirectX::SimpleMath::Vector3::Forward),
	m_view(DirectX::SimpleMath::Matrix::Identity),
	m_type(type)
{
}

WIS::CameraBase::~CameraBase()
{
}

void WIS::CameraBase::Initialize()
{
	UniqueInitialize();
	DirectX::SimpleMath::Vector3 up(0.0f, 1.0f, 0.0f);
	m_view = DirectX::SimpleMath::Matrix::CreateLookAt(m_position, m_target, up);
}

void WIS::CameraBase::Update(const DirectX::Mouse::State& state)
{
	DirectX::SimpleMath::Vector3 up(0.0f, 1.0f, 0.0f);

	UniqueUpdate(m_position, m_target, up, state);

	m_view = DirectX::SimpleMath::Matrix::CreateLookAt(m_position, m_target, up);
}

void WIS::CameraBase::Rander()
{
	UniqueRander();
}

void WIS::CameraBase::Finalize()
{
	Uniquefinalize();
}

void WIS::CameraBase::RotateTarget(const DirectX::SimpleMath::Matrix& rot)
{
	DirectX::SimpleMath::Vector3::Transform(m_target, rot);
}