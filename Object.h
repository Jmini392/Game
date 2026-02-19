#pragma once

#include "Mesh.h"

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
	virtual void Update();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);
	// virtual RECT GetBB() {};

	void Scale(XMFLOAT3* pxmf3Scale);
	void Move(XMFLOAT3* pxmf3Shift);
	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);

	XMFLOAT3 GetPosition() const { return XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43); }
	void SetPosition(XMFLOAT3 xmf3Position);
};