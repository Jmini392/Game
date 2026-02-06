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

	m_BBMesh = new BoundingBoxMesh(pd3dDevice, pd3dCommandList);
	m_BBShader = new Dbg::DebugShader();
	m_BBShader->CreateShader(pd3dDevice);
}

Player::~Player()
{
	if (m_BBMesh) m_BBMesh->Release();
	if (m_BBShader) m_BBShader->Release();
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

void Player::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	Object::Render(pd3dCommandList, pCamera); // 플레이어 큐브 본체 렌더링

	if (renderBB) // 바운딩 박스 렌더링
	{
		// 현재 월드 바운딩 박스 정보 가져오기
		XMFLOAT3 center = m_worldBB.Center;
		XMFLOAT3 extents = m_worldBB.Extents;

		// 바운딩 박스를 그리기 위한 월드 행렬 계산
		// 단위 큐브(1x1x1)를 Extents * 2 만큼 확대(Scale)하고, Center 위치로 이동(Translation)
		XMFLOAT4X4 xmf4x4BoxWorld;
		XMMATRIX mtxScale = XMMatrixScaling(extents.x * 2.0f, extents.y * 2.0f, extents.z * 2.0f);
		XMMATRIX mtxTrans = XMMatrixTranslation(center.x, center.y, center.z);
		XMStoreFloat4x4(&xmf4x4BoxWorld, mtxScale * mtxTrans);

		// 디버그 셰이더에 행렬 전달 및 렌더링
		m_BBShader->UpdateShaderVariable(pd3dCommandList, &xmf4x4BoxWorld);
		m_BBShader->OnPrepareRender(pd3dCommandList);

		// 셰이더의 Render를 호출하거나 메쉬의 Render를 호출
		// (Shader::Render는 OnPrepareRender만 수행하므로 Mesh::Render를 직접 호출)
		m_BBMesh->Render(pd3dCommandList);
	}
}