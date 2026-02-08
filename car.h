#pragma once
#include "Object.h"
class Car : public Object
{
private:
	static int					m_nReferences; // 해당 오브젝트가 가지는 메시 데이터의 참조 횟수
	static Mesh*				m_pCarMesh; // 자동차 메시
public:
	Car(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,XMFLOAT3 pos, XMFLOAT3 scale);
	~Car();
};


