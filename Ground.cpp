#include "stdafx.h"
#include "Ground.h"

Mesh*	Ground::m_pGroundMesh = NULL;
int		Ground::m_nReferences = 0;

Ground::Ground(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fHeight)
{
	if (m_pGroundMesh == NULL)
	{
		m_pGroundMesh = new GroundMesh(pd3dDevice, pd3dCommandList, fWidth, fHeight);
	}
	m_nReferences++;
	SetMesh(m_pGroundMesh);
}

Ground::~Ground()
{
	if (--m_nReferences == 0)
	{
		if (m_pGroundMesh) delete m_pGroundMesh;
		m_pGroundMesh = NULL;
	}
}