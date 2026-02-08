#include "stdafx.h"
#include "Object.h"
#include "Shader.h"
#include "Camera.h"

Object::Object()
{
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
	// 단위행렬 생성후 저장
}

Object::~Object()
{
	if (m_pMesh) m_pMesh->Release();
	if (m_pShader) {
		m_pShader->ReleaseShaderVariables();
		m_pShader->Release();
	}
}

void Object::SetShader(Shader* pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}

void Object::SetMesh(Mesh* pMesh)
{
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh) m_pMesh->AddRef();
}

void Object::ReleaseUploadBuffers()
{
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();
}

void Object::Animate(float fTimeElapsed)
{
}

void Object::OnPrepareRender()
{
}

void Object::Update()
{
}

void Object::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	OnPrepareRender();
	if (m_pShader)
	{
		//게임 객체의 월드 변환 행렬을 셰이더의 상수 버퍼로 전달(복사)한다. 
		m_pShader->UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);
		m_pShader->Render(pd3dCommandList, pCamera);
	}
	if (m_pMesh) m_pMesh->Render(pd3dCommandList);
}

void Object::Scale(XMFLOAT3* pxmf3Scale)
{
	XMMATRIX mtxScale = XMMatrixScalingFromVector(XMLoadFloat3(pxmf3Scale));
	m_xmf4x4World = Matrix4x4::Multiply(mtxScale, m_xmf4x4World);
}

void Object::Move(XMFLOAT3* pxmf3Shift)
{
	XMMATRIX mtxTranslate = XMMatrixTranslationFromVector(XMLoadFloat3(pxmf3Shift));
	m_xmf4x4World = Matrix4x4::Multiply(mtxTranslate, m_xmf4x4World);
}

void Object::Rotate(XMFLOAT3* pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis),
		XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}