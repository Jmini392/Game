#include "stdafx.h"
#include "Ground.h"
#include "ResourceMgr.h"
Mesh*	Ground::m_pGroundMesh = NULL;

Ground::Ground(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_ppStaticMeshes = &m_pGroundMesh;
	if (m_pGroundMesh == NULL)
	{
		m_pGroundMesh = ResourceMgr::Instance()->LoadMesh(pd3dDevice, pd3dCommandList, L"Ground", "Res/Meshes/Ground.fbx");
	}
	XMFLOAT3 pos = XMFLOAT3(0.0f, 20.0f, 0.0f), scale = XMFLOAT3(20.0f, 20.0f, 20.0f);
	float rotate_angle = 90.0f;
	XMFLOAT3 rotate_axis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	//Move(&pos);
	Scale(&scale);
	// Rotate(&rotate_axis, rotate_angle);
	SetMesh(m_pGroundMesh);
}

Ground::~Ground()
{
}