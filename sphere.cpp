#include "stdafx.h"
#include "sphere.h"
#include "ResourceMgr.h"

MovingSphere::MovingSphere(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float speed, int line)
	: sphere(pd3dDevice, pd3dCommandList)
{
	Mesh* m_psphereMesh = ResourceMgr::Instance()->LoadMeshObj(pd3dDevice, pd3dCommandList, L"sphere", "Res/Meshes/sphere.obj");
	
	float zOffset = line * 50.0f - 100;
	XMFLOAT3 scale = XMFLOAT3(m_fRadius, m_fRadius, m_fRadius);
	Scale(&scale); // 구 크기 조절
	SetMesh(m_psphereMesh);
	SetPosition(XMFLOAT3(zOffset, m_fRadius, 100.0f)); // 초기 위치 설정
	m_fMoveSpeed = speed; // 이동 속도 설정
}

MovingSphere::~MovingSphere()
{
}

void MovingSphere::Animate(float fTimeElapsed)
{
	RotateSphere(fTimeElapsed);
	//MoveSphere(fTimeElapsed);
}

void MovingSphere::RotateSphere(float fTimeElapsed)
{
	XMFLOAT3 ro = XMFLOAT3(1.0f, 0.0f, 0.0f);
	Rotate(&ro, -m_fRotationSpeed * fTimeElapsed);
}

void MovingSphere::MoveSphere(float fTimeElapsed)
{
	XMFLOAT3 shift = XMFLOAT3(0.0f, 0.0f, m_fMoveSpeed * fTimeElapsed);
	Move(&shift);
}

BounsSphere::BounsSphere(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
	: sphere(pd3dDevice, pd3dCommandList)
{
	Mesh* m_psphereMesh = ResourceMgr::Instance()->LoadMeshObj(pd3dDevice, pd3dCommandList, L"sphere", "Res/Meshes/sphere.obj");
	
	XMFLOAT3 scale = XMFLOAT3(m_fRadius, m_fRadius, m_fRadius);
	Scale(&scale); // 구 크기 조절
	SetMesh(m_psphereMesh);
	SetPosition(XMFLOAT3(0.0f, 50.0f, 100.0f)); // 초기 위치 설정
}
BounsSphere::~BounsSphere()
{
}

void BounsSphere::Animate(float fTimeElapsed)
{
	RotateSphere(fTimeElapsed);
}

void BounsSphere::RotateSphere(float fTimeElapsed)
{
	XMFLOAT3 ro = XMFLOAT3(1.0f, 0.0f, 0.0f);
	Rotate(&ro, -m_fRotationSpeed * fTimeElapsed);
	MoveSphere(fTimeElapsed);
}

void BounsSphere::MoveSphere(float fTimeElapsed)
{
	// 중력 적용: 속도 = 속도 + 가속도 * 시간
	m_fDownSpeed += m_fddownSpeed * fTimeElapsed;

	// 위치 이동
	XMFLOAT3 shift = XMFLOAT3(0.0f, m_fDownSpeed * fTimeElapsed, m_fMoveSpeed * fTimeElapsed);
	//Move(&shift);

	// 바닥 충돌 검사 및 튕기기
	XMFLOAT3 pos = GetPosition();
	float fBottomY = pos.y - m_fRadius;

	if (fBottomY <= m_fGroundY && m_fDownSpeed < 0.0f)
	{
		// 바닥 위로 위치 보정
		pos.y = m_fGroundY + m_fRadius;
		SetPosition(pos);

		// 속도 반전 (반발 계수 적용)
		m_fDownSpeed = -m_fDownSpeed * m_fBounceFactor;
	}
}