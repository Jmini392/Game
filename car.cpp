#include "stdafx.h"
#include "car.h"
#include "ResourceMgr.h"
Mesh*	Car::m_pCarMesh = NULL;

Car::Car(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT3 pos, XMFLOAT3 scale)
{
	m_ppStaticMeshes = &m_pCarMesh;
	if (m_pCarMesh == NULL)
	{
		m_pCarMesh = ResourceMgr::Instance()->LoadMesh(pd3dDevice, pd3dCommandList, L"milk", "Res/Meshes/milk.fbx");
	}
	SetMesh(m_pCarMesh);
	Move(&pos);
	Scale(&scale);
	XMFLOAT3 ro = XMFLOAT3(0.0f, 1.0f, 0.0f);
	Rotate(&ro, 180.0f);
}

Car::~Car()
{
}

void Car::Animate(float fTimeElapsed)
{
	rotateCar(fTimeElapsed);
	// MoveCar(fTimeElapsed);
}

void Car::rotateCar(float fTimeElapsed)
{
	XMFLOAT3 ro = XMFLOAT3(0.0f, 1.0f, 0.0f);
	Rotate(&ro, m_fRotationSpeed * fTimeElapsed);
}

void Car::MoveCar(float fTimeElapsed)
{
	XMFLOAT3 shift = XMFLOAT3(0.0f, 0.0f, m_fMoveSpeed * fTimeElapsed);
	Move(&shift);

}