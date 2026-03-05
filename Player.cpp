#include "stdafx.h"
#include "Player.h"
#include "ResourceMgr.h"

Player::Player(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
	: Entity(pd3dDevice, pd3dCommandList)
{
	Mesh* m_pPlayerMesh = ResourceMgr::Instance()->LoadMeshFbx(pd3dDevice, pd3dCommandList, L"player", L"Res/Meshes/player.fbx");
	SetMesh(m_pPlayerMesh);
	XMFLOAT3 pos = XMFLOAT3(0.0f, 20.0f, 50.0f);
	XMFLOAT3 scale = XMFLOAT3(15.0f, 15.0f, 15.0f);
	XMFLOAT3 rot = XMFLOAT3(1.0f, 0.0f, 0.0f);
	Rotate(&rot, -30.0f);
 	Move(&pos);
	Scale(&scale);
}

Player::~Player()
{
}

void Player::Animate(float fTimeElapsed)
{
	XMFLOAT4X4 xmf4x4Rotation = Matrix4x4::RotationY_Multiply(3.0f * fTimeElapsed);
	m_xmf4x4World._42 += -20.0f; m_xmf4x4World._43 += -50.0f;
	m_xmf4x4World = Matrix4x4::Multiply(m_xmf4x4World, xmf4x4Rotation);
	m_xmf4x4World._42 += 20.0f; m_xmf4x4World._43 += 50.0f;
}