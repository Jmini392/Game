#include "stdafx.h"
#include "Ground.h"
#include "ResourceMgr.h"

Ground::Ground(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, POINT chunk)
{

	Mesh* m_pGroundMesh = ResourceMgr::Instance()->LoadMesh(pd3dDevice, pd3dCommandList, L"ground");
	if (m_pGroundMesh == nullptr) {
		m_pGroundMesh = new GroundMesh(pd3dDevice, pd3dCommandList, 50, 50);
		ResourceMgr::Instance()->AddMesh(L"ground", m_pGroundMesh);
	}
	SetMesh(m_pGroundMesh);

	XMFLOAT3 init = XMFLOAT3(chunk.x, 0.0f, chunk.y * 50.0f);
	Move(&init);
}

Ground::~Ground()
{
}

Hill::Hill(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,POINT chunk, float angle)
	: Ground(pd3dDevice, pd3dCommandList, chunk)
{
	m_fSlopeAngle = angle;
	XMFLOAT3 ro = XMFLOAT3(1.0f, 0.0f, 0.0f);
	Rotate(&ro, m_fSlopeAngle);
}

Hill::~Hill()
{
}