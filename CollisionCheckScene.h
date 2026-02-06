#pragma once
#include "Scene.h"
#include <DirectXCollision.h>

using namespace DirectX;

class Mesh; // 전방 선언
class Shader;

class CollisionCheckScene : public Scene
{
public:
	CollisionCheckScene();
	virtual	~CollisionCheckScene();

	virtual void Enter(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Exit() override;
	virtual void Update();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);

private:
	BoundingBox m_testBB;

	Mesh* m_BBMesh = nullptr;
	Shader* m_BBShader = nullptr;
};

