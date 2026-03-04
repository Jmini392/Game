#include "stdafx.h"
#include "Entity.h"
#include "ResourceMgr.h"

Entity::Entity(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pAxisMesh  = ResourceMgr::Instance()->LoadMesh(pd3dDevice, pd3dCommandList, L"Axis");
	if (m_pAxisMesh == nullptr) {
		m_pAxisMesh = new AxisMesh(pd3dDevice, pd3dCommandList, 3);
		ResourceMgr::Instance()->AddMesh(L"Axis", m_pAxisMesh);
	}
}

Entity::~Entity()
{
}

void Entity::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	Object::Render(pd3dCommandList, pCamera);
	if (m_pAxisMesh) {
		m_pAxisMesh->Render(pd3dCommandList);
	}
}