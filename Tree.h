#pragma once
#pragma once
#include "Object.h"
class Tree : public Object
{
private:
public:
	Tree(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT3 pos);
	~Tree();

	void Animate(float fTimeElapsed) {};
};