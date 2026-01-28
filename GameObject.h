#pragma once
#include "Mesh.h"
#include <DirectXMath.h>

using namespace DirectX;

class GameObject
{
public:
	GameObject();
	~GameObject() {};

	void SetMesh(Mesh* mesh) { mMesh = mesh; }
	void SetPosition(float x, float y, float z) { mPosition = XMFLOAT3(x, y, z); }
	void SetScale(float s) { mScale = XMFLOAT3(s, s, s); }

	void Update(float dt);

	XMMATRIX GetWorldMatrix() const;

	void Render(ID3D12GraphicsCommandList* cmdList);
private:
	Mesh* mMesh = nullptr;

	XMFLOAT3 mPosition = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 mRotation = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 mScale = { 1.0f, 1.0f, 1.0f };

	XMFLOAT4X4 mWorld; // 객체의 월드 행렬
};

