#pragma once
#include "Entity.h"
class Player : public Entity
{
public:
	Player(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~Player();
	virtual void Animate(float fTimeElapsed) override;
};

