#pragma once
#include "ComponentBase.h"
#include "WISLib/WISLibTags.h"
#include <vector>


namespace WIS {
	class FloorComponent :public WIS::ComponentBase
	{
	private:

		std::vector<DirectX::VertexPositionColor> m_VertexList;
		std::vector<uint16_t> m_IndexList;


		bool m_HaveModel = false;

	public:
		FloorComponent();
		~FloorComponent();
		void OnStart(UserResources* userResources) override;

		void OnUpdate(const float elapsedTime, UserResources* userResources) override;

		void OnCollision(std::shared_ptr<WIS::GameObject> otherObject) override;

		// モデルロード時に、ここにもデータを入れておく
		void SetMeshData(const std::vector<DirectX::VertexPositionColor>& verts, const std::vector<uint16_t>& indices) {
			m_VertexList = verts;
			m_IndexList = indices;
		}

		void SetModel();


		std::vector<DirectX::VertexPositionColor>& GetVertices() { return m_VertexList; }
		//IndexList
		std::vector<uint16_t>& GetIndices() { return m_IndexList; }

		float GetHeight(DirectX::SimpleMath::Vector3);
	};
}

