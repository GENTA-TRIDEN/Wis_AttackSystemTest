#pragma once
#include "CameraBase.h"
class FixedCamera :public WIS::CameraBase
{
	//関数群


private:


	DirectX::SimpleMath::Vector3 m_fixedTarget;

public:
	FixedCamera(DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Vector3 target);
	~FixedCamera();
	//派生オブジェクト特有の動き
	void UniqueInitialize()override;
	void UniqueUpdate(DirectX::SimpleMath::Vector3& position, DirectX::SimpleMath::Vector3& target, DirectX::SimpleMath::Vector3& up, const DirectX::Mouse::State& state)override;
	void UniqueRander()override;
	void Uniquefinalize()override;







};