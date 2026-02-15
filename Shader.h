#pragma once
#include "Object.h"
#include "Camera.h"

// 게임 객체의 정보를 셰이더에게 넘겨주기 위한 구조체(상수 버퍼)이다. 
struct CB_GAMEOBJECT_INFO
{
	XMFLOAT4X4 m_xmf4x4World;
};

// 조명 정보를 셰이더에게 넘겨주기 위한 구조체
struct CB_LIGHT_INFO
{
	XMFLOAT4 m_xmf4LightColor;        // 광원 색상 (rgb + intensity)
	XMFLOAT4 m_xmf4LightDirection;    // 광원 방향 (방향광) 또는 위치 (점광원)
	XMFLOAT4 m_xmf4CameraPosition;    // 카메라 위치 (Specular 계산용)
	XMFLOAT4 m_xmf4LightParams;       // x: 광원 타입 (0=방향광, 1=점광원), y: 감쇠 계수, z,w: 예약
};

// 재질 정보를 셰이더에 전달하기 위한 구조체
struct CB_MATERIAL_INFO
{
	XMFLOAT4 m_xmf4Ambient;
	XMFLOAT4 m_xmf4Diffuse;
	XMFLOAT4 m_xmf4Specular;  // w = Shininess
	XMFLOAT4 m_xmf4Emissive;
};

class Shader
{
public:
	Shader();
	virtual ~Shader();

private:
	int m_nReferences = 0;
	static ID3D12RootSignature* s_pd3dGraphicsRootSignature;  // 루트 시그니처는 static으로 모든 Shader 공유
	static bool s_bRootSignatureCreated;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	static void CreateRootSignature(ID3D12Device* pd3dDevice);  // 루트 시그니처 생성
	static void ReleaseRootSignature();  // 루트 시그니처 해제
	static ID3D12RootSignature* GetRootSignature() { return s_pd3dGraphicsRootSignature; }

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
	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList,
		XMFLOAT4X4* pxmf4x4World);
	virtual void UpdateLightVariable(ID3D12GraphicsCommandList* pd3dCommandList,
		const CB_LIGHT_INFO& lightInfo);
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