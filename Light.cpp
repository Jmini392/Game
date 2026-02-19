#include "stdafx.h"
#include "Light.h"

Light::Light()
{
	// ±âº» Èò»ö ±¤¿ø
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
	CB_LIGHT_INFO lightInfo;
	lightInfo.m_xmf4LightColor = m_xmf4Color;
	lightInfo.m_xmf4LightDirection = m_xmf4Direction;
	lightInfo.m_xmf4CameraPosition = m_xmf4CameraPosition;
	lightInfo.m_xmf4LightParams = m_xmf4Params;

	Shader::UpdateLightCBV(pd3dCommandList, lightInfo);
}