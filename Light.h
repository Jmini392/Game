#pragma once
#include "Shader.h"

class Light
{
private:
	XMFLOAT4 m_xmf4Color;
	XMFLOAT4 m_xmf4Direction;
	XMFLOAT4 m_xmf4CameraPosition;
	XMFLOAT4 m_xmf4Params;

public:
	Light();
	virtual ~Light();

	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);

	void SetLightColor(XMFLOAT4 color) { m_xmf4Color = color; }
	void SetLightDirection(XMFLOAT4 direction) { m_xmf4Direction = direction; }
	void SetCameraPosition(XMFLOAT4 position) { m_xmf4CameraPosition = position; }
	void SetLightParams(XMFLOAT4 params) { m_xmf4Params = params; }

	XMFLOAT4 GetLightColor() { return m_xmf4Color; }
	XMFLOAT4 GetLightDirection() { return m_xmf4Direction; }
};