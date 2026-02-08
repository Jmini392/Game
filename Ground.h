#pragma once
#include "Object.h"
class Ground : public Object
{
private:
	static int					m_nReferences; // 해당 오브젝트가 가지는 메시 데이터의 참조 횟수
	static Mesh*				m_pGroundMesh; // 지면 메시 데이터 (정적 멤버 변수)
public:
	Ground(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fHeight);
	~Ground();
};