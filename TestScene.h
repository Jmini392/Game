#pragma once

#include "Scene.h"

class TestScene : public Scene
{
public:
	TestScene();
	~TestScene();

	virtual void Enter(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Exit();
};

