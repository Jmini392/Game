#pragma once
#include "Object.h"
class Player : public Object
{
public:
	Player(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	~Player();

	void Update();
	virtual void Render();
};

