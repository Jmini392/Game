#pragma once

#include "Scene.h"

class DiffusedShader; // 전방 선언

class TestScene : public Scene
{
private:
	void CreateRandomObstacles(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
							   DiffusedShader* pShader, int obstacleCount);

public:
	TestScene();
	~TestScene();

	virtual void Enter(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Exit();
};

