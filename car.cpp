#include "stdafx.h"
#include "car.h"
#include "ResourceMgr.h"

Car::Car(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT3 pos, XMFLOAT3 scale)
{
	
	Mesh* m_pCarMesh = ResourceMgr::Instance()->LoadMeshFbx(pd3dDevice, pd3dCommandList, L"milk", L"Res/Meshes/milk.fbx");
	
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
	UpDownCar(fTimeElapsed);
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

void Car::UpDownCar(float fTimeElapsed)
{
	XMFLOAT3 shift = XMFLOAT3(0.0f, m_fUpDownSpeed * fTimeElapsed, 0.0f);
	Move(&shift);
	XMFLOAT3 pos = GetPosition();
	if (pos.y > m_fUpDownRange) {
		pos.y = m_fUpDownRange;
		m_fUpDownSpeed = -m_fUpDownSpeed; // 방향 전환
	}
	else if (pos.y < -m_fUpDownRange) {
		pos.y = -m_fUpDownRange;
		m_fUpDownSpeed = -m_fUpDownSpeed; // 방향 전환
	}
	SetPosition(pos);
}