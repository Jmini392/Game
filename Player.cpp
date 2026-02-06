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

	SetBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(6.0f, 6.0f, 6.0f)); // 로컬 바운딩 박스 설정
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

	UpdateWorldBB(); // 월드 바운딩 박스 갱신
}

void Player::Render()
{
}