#pragma once
#include "Object.h"
class Car : public Object
{
private:
	static int					m_nReferences; // 해당 오브젝트가 가지는 메시 데이터의 참조 횟수
	static Mesh*				m_pCarMesh; // 자동차 메시

	float m_fRotationSpeed = 90.0f; // degrees per second
	float m_fMoveSpeed = -20.0f; // units per second
public:
	Car(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,XMFLOAT3 pos, XMFLOAT3 scale);
	~Car();

	void Animate(float fTimeElapsed) override;

	void rotateCar(float fTimeElapsed);
	void MoveCar(float fTimeElapsed);
};


