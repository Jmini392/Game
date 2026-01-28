#include "GameObject.h"

GameObject::GameObject()
{
	XMStoreFloat4x4(&mWorld, XMMatrixIdentity());
}

void GameObject::Update(float dt)
{
	mRotation.y += dt;
	mRotation.x += dt * 0.5f;

	XMMATRIX S = XMMatrixScaling(mScale.x, mScale.y, mScale.z);
	XMMATRIX R = XMMatrixRotationRollPitchYaw(mRotation.x, mRotation.y, mRotation.z);
	XMMATRIX T = XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z);

	XMMATRIX world = S * R * T;
	XMStoreFloat4x4(&mWorld, world);
}

XMMATRIX GameObject::GetWorldMatrix() const
{
	return XMLoadFloat4x4(&mWorld);
}

void GameObject::Render(ID3D12GraphicsCommandList* cmdList)
{
	if (mMesh) mMesh->Render(cmdList);
}