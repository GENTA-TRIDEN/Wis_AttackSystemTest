#pragma once
#include "ComponentBase.h"
#include "WISLib/WISLibTags.h"
#include <vector>


namespace WIS {
	class FloorComponent :public WIS::ComponentBase
	{
	private:

		std::vector<DirectX::VertexPositionColor> VertexList;
		std::vector<uint16_t> IndexList;


	public:
		FloorComponent();
		~FloorComponent();
		void OnStart(UserResources* userResources) override;

		void OnUpdate(const float elapsedTime, UserResources* userResources) override;

		void OnCollision(std::shared_ptr<WIS::GameObject> otherObject) override;

		// モデルロード時に、ここにもデータを入れておく
		void SetMeshData(const std::vector<DirectX::VertexPositionColor>& verts, const std::vector<uint16_t>& indices) {
			VertexList = verts;
			IndexList = indices;
		}


		std::vector<DirectX::VertexPositionColor>& GetVertices() { return VertexList; }
		std::vector<uint16_t>& GetIndices() { return IndexList; }


	};
}

