#pragma once

#include "Mesh.h"
#include "Shader.h"
#include <DirectXCollision.h>

using namespace DirectX;

class Shader;
class Camera;

class Object
{
private:
	int m_nReference = 0;
protected:
	XMFLOAT4X4 m_xmf4x4World;
	Mesh* m_pMesh = NULL;
	Shader* m_pShader = NULL;

	BoundingBox m_localBB; // 로컬 공간의 바운딩 박스(AABB 형태), 회전 및 크기 변환이 적용되지 않은 초기 상태
	BoundingBox m_worldBB; // 월드 공간의 바운딩 박스(AABB 형태), 실제 충돌 체크에 사용

public:
	Object();
	virtual ~Object();

	void AddRef() { m_nReference++; }
	void Release() { if (--m_nReference <= 0) delete this; }
public:
	void ReleaseUploadBuffers();
	virtual void SetMesh(Mesh* pMesh);
	virtual void SetShader(Shader* pShader);
	virtual void Animate(float fTimeElapsed);
	virtual void OnPrepareRender();
	virtual void Update() = 0;
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);

	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);

	void SetBB(const XMFLOAT3& center, const XMFLOAT3& extents); // 로컬 바운딩 박스 설정 / 중심점, 반너비(반지름?)
	void UpdateWorldBB(); // 월드 변환 행렬을 적용하여 월드 바운딩 박스 갱신
	const BoundingBox& GetBB() const { return m_worldBB; } // 월드 바운딩 박스 반환, 충돌 체크에 사용
};