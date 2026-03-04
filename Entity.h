#pragma once
#include "object.h"
class Entity : public Object
{
private:
	Mesh* m_pAxisMesh = nullptr; // 축 메쉬 (선택적으로 사용)
public:
	Entity(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	~Entity();

	void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera) override;
};

