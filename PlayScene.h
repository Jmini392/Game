#pragma once
#include "Scene.h"

class PlayScene : public Scene
{
public:
	PlayScene();
	~PlayScene();

	virtual void Enter(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Exit();
};

