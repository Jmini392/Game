#pragma once
#include "Object.h"
class Ground : public Object
{
private:

public:
	Ground(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, POINT chunk);
	~Ground();
};

// 오르막길 오브젝트
class Hill : public Ground
{
private:
	float m_fSlopeAngle = 30.0f; // 기울기 (degrees)
public:
	Hill(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,POINT chunk, float angle);
	~Hill();
};