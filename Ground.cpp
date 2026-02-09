#include "stdafx.h"
#include "Ground.h"

Mesh*	Ground::m_pGroundMesh = NULL;

Ground::Ground(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fHeight)
{
	m_ppStaticMeshes = &m_pGroundMesh;
	if (m_pGroundMesh == NULL)
	{
		m_pGroundMesh = new GroundMesh(pd3dDevice, pd3dCommandList, fWidth, fHeight);
	}
	SetMesh(m_pGroundMesh);
}

Ground::~Ground()
{
}