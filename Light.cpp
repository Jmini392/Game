#include "stdafx.h"
#include "Light.h"

Light::Light()
{
	// 기본 흰색 광원
	m_xmf4Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_xmf4Direction = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
	m_xmf4CameraPosition = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	m_xmf4Params = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
}

Light::~Light()
{
}

void Light::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	// 루트 파라미터 인덱스 2번 (b2 레지스터)에 조명 정보 전달
	// XMFLOAT4 4개 = 16개의 float = 16개의 32bit 상수
	pd3dCommandList->SetGraphicsRoot32BitConstants(2, 4, &m_xmf4Color, 0);
	pd3dCommandList->SetGraphicsRoot32BitConstants(2, 4, &m_xmf4Direction, 4);
}