#pragma once
#include "Entity.h"

class sphere : public Entity
{
public:
	sphere(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) : Entity(pd3dDevice,pd3dCommandList){};
protected:
	float					m_fRotationSpeed = 90.0f; // degrees per second
	float					m_fRadius = 15.0f; // 구 반지름
	float					m_fMoveSpeed = -20.0f; // x축 속도
};
class MovingSphere : public sphere
{
private:
public:
	MovingSphere(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float speed, int line);
	~MovingSphere();
	void Animate(float fTimeElapsed) override;
	void RotateSphere(float fTimeElapsed);
	void MoveSphere(float fTimeElapsed);
};
class BounsSphere : public sphere
{
private:
	float					m_fddownSpeed = -30.0f; // y축 가속도
	float					m_fDownSpeed = 0.0f; // y축 속도

	float					m_fGroundY = 0.0f; // 바닥 y좌표
	float					m_fBounceFactor = 0.8f; // 반발 계수 (0~1, 1이면 완전 탄성)
public:
	BounsSphere(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	~BounsSphere();

	void Animate(float fTimeElapsed) override;
	void RotateSphere(float fTimeElapsed);
	void MoveSphere(float fTimeElapsed);

	void SetGroundY(float fGroundY) { m_fGroundY = fGroundY; }
	void SetBounceFactor(float fFactor) { m_fBounceFactor = fFactor; }
	void SetRadius(float fRadius) { m_fRadius = fRadius; }
};

