#pragma once
#include "Object.h"
class Car : public Object
{
private:
	float m_fRotationSpeed = 90.0f; // degrees per second
	float m_fMoveSpeed = -20.0f; // units per second

	float m_fUpDownSpeed = 10.f; // y축 속도
	float m_fUpDownRange = 3.f; // 위아래 이동 범위
public:
	Car(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,XMFLOAT3 pos, XMFLOAT3 scale);
	~Car();

	void Animate(float fTimeElapsed) override;

	void rotateCar(float fTimeElapsed);
	void MoveCar(float fTimeElapsed);
	void UpDownCar(float fTimeElapsed);
};