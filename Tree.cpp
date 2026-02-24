#include "stdafx.h"
#include "Tree.h"
#include "ResourceMgr.h"

Tree::Tree(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT3 pos)
{
	Mesh* m_pCarMesh = ResourceMgr::Instance()->LoadMeshFbx(pd3dDevice, pd3dCommandList, L"tree", L"Res/Meshes/birch_tree.fbx");

	SetMesh(m_pCarMesh);
	XMFLOAT3 scale = XMFLOAT3(70.0f, 70.0f, 70.0f);
	Scale(&scale);
	Move(&pos);
}

Tree::~Tree()
{
}