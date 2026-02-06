#pragma once
#include "Object.h"
class Player : public Object
{
private:
	Mesh* m_BBMesh = nullptr;
	Shader* m_BBShader = nullptr;

	bool renderBB = true; // 바운딩 박스 렌더링 여부
public:
	Player(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	~Player();

	virtual void Update() override;
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera) override;
};

