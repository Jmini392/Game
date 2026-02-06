#include "stdafx.h"
#include "Player.h"

#include "KeyMgr.h"
#include "TimeMgr.h"

Player::Player(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CubeMeshDiffused* pCubeMesh = new CubeMeshDiffused(pd3dDevice, pd3dCommandList, 12.0f, 12.0f, 12.0f);
	SetMesh(pCubeMesh);
	DiffusedShader* pShader = new DiffusedShader();
	pShader->CreateShader(pd3dDevice);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	SetShader(pShader);
}

Player::~Player()
{
}

void Player::Update()
{
	if (KEY_HOLD(KEY::W)) {
		m_xmf4x4World._43 += 30.f * DT;
	}
	if (KEY_HOLD(KEY::S)) {
		m_xmf4x4World._43 -= 30.f * DT;
	}
	if (KEY_HOLD(KEY::A)) {
		m_xmf4x4World._41 -= 30.f * DT;
	}
	if (KEY_HOLD(KEY::D)) {
		m_xmf4x4World._41 += 30.f * DT;
	}
}

void Player::Render()
{
}