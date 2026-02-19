#pragma once
#include "Camera.h"

// 조명 정보를 셰이더에게 넘겨주기 위한 구조체
struct CB_LIGHT_INFO
{
	XMFLOAT4 m_xmf4LightColor;
	XMFLOAT4 m_xmf4LightDirection;
	XMFLOAT4 m_xmf4CameraPosition;
	XMFLOAT4 m_xmf4LightParams;
};

// 재질 정보를 셰이더에 전달하기 위한 구조체
struct CB_MATERIAL_INFO
{
	XMFLOAT4 m_xmf4Ambient;
	XMFLOAT4 m_xmf4Diffuse;
	XMFLOAT4 m_xmf4Specular;
	XMFLOAT4 m_xmf4Emissive;
};

class Shader
{
public:
	Shader();
	virtual ~Shader();

private:
	int m_nReferences = 0;
	static ID3D12RootSignature* s_pd3dGraphicsRootSignature;
	static bool s_bRootSignatureCreated;

	// Light CBV
	static ID3D12Resource* s_pd3dLightCB;
	static CB_LIGHT_INFO* s_pMappedLight;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	static void CreateRootSignature(ID3D12Device* pd3dDevice);
	static void ReleaseRootSignature();
	static ID3D12RootSignature* GetRootSignature() { return s_pd3dGraphicsRootSignature; }

	static void CreateConstantBuffers(ID3D12Device* pd3dDevice);
	static void ReleaseConstantBuffers();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	D3D12_SHADER_BYTECODE CompileShaderFromFile(const WCHAR* pszFileName, LPCSTR pszShaderName,
		LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob);
	virtual void CreateShader(ID3D12Device* pd3dDevice);
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World);

	// CBV 업데이트 함수
	static void UpdateLightCBV(ID3D12GraphicsCommandList* pd3dCommandList, const CB_LIGHT_INFO& lightInfo);
	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);

protected:
	ID3D12PipelineState** m_ppd3dPipelineStates = NULL;
	int m_nPipelineStates = 0;
};

class DiffusedShader : public Shader
{
public:
	DiffusedShader();
	virtual ~DiffusedShader();
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual void CreateShader(ID3D12Device* pd3dDevice);
};