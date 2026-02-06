#pragma once
#include "Object.h"
#include "Camera.h"

//게임 객체의 정보를 셰이더에게 넘겨주기 위한 구조체(상수 버퍼)이다. 
struct CB_GAMEOBJECT_INFO
{
	XMFLOAT4X4 m_xmf4x4World;
};

class Shader
{
public:
	Shader();
	virtual ~Shader();

private:
	int m_nReferences = 0;
	static ID3D12RootSignature* s_pd3dGraphicsRootSignature;  // ← static으로 모든 Shader 공유
	static bool s_bRootSignatureCreated;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	static void CreateRootSignature(ID3D12Device* pd3dDevice);  // ← static 생성 함수
	static void ReleaseRootSignature();  // ← static 해제 함수
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

class UIScreenShader : public DiffusedShader
{
public:
	UIScreenShader();
	virtual ~UIScreenShader() {}
	// Render 오버라이드에서 직접 view/proj(직교)를 루트 상수에 씁니다.
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera) override;

	// 화면 직교 설정: left,right,top,bottom 은 픽셀 좌표(왼쪽 상단 원점 사용 시 top=0, bottom=FRAME_BUFFER_HEIGHT)
	void SetOrtho(float left, float right, float top, float bottom, float znear = 0.0f, float zfar = 1.0f);
private:
	XMFLOAT4X4 m_xmf4x4View;
	XMFLOAT4X4 m_xmf4x4Projection;
};