#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <DirectXMath.h>

using namespace Microsoft::WRL;
using namespace DirectX;

struct Vertex
{
	XMFLOAT3 position; // x, y, z 좌표
	XMFLOAT4 color;    // RGBA 색상
	XMFLOAT2 uv;       // 텍스처 좌표
};

struct ObjectConstants
{
	XMFLOAT4X4 worldViewProj; // 월드-뷰-투영 행렬
	float padding[48]; // 256바이트 정렬을 위한 패딩
};

class GameFramework
{
public:
	GameFramework();
	~GameFramework();

	bool Initialize(HWND hwnd, int width, int height);

	void Update();
	void Render();

	void Release();

private:
	bool BuildObjects();
	bool BuildTexture();

	ComPtr<IDXGIFactory4> mdxgiFactory;
	ComPtr<ID3D12Device> md3dDevice;
	ComPtr<ID3D12Fence> md3dFence;
	UINT64 mCurrentFence = 0;

	ComPtr<ID3D12CommandQueue> md3dCommandQueue;
	ComPtr<ID3D12CommandAllocator> md3dCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> md3dCommandList;

	ComPtr<IDXGISwapChain4> mSwapChain;
	int mClientWidth = 800;
	int mClientHeight = 600;

	UINT mRtvDescriptorSize = 0; // 렌더 타겟 뷰 디스크립터 크기
	UINT mDsvDescriptorSize = 0; // 깊이-스텐실 뷰 디스크립터 크기
	UINT mCbvSrvUavDescriptorSize = 0; // 셰이더 리소스 뷰 디스크립터 크기

	HWND mhMainWnd = nullptr;

	ComPtr<ID3D12DescriptorHeap> mRtvHeap; // 렌더 타겟 뷰 디스크립터 힙
	ComPtr<ID3D12DescriptorHeap> mDsvHeap; // 깊이-스텐실 뷰 디스크립터 힙

	static const int SwapChainBufferCount = 2;
	ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];
	ComPtr<ID3D12Resource> mDepthStencilBuffer;

	int mCurrBackBuffer = 0;

	// 뷰포트와 시저 사각형
	D3D12_VIEWPORT mScreenViewport;
	D3D12_RECT mScissorRect;

	ComPtr<ID3D12RootSignature> mRootSignature;
	ComPtr<ID3D12PipelineState> mPipelineState;

	ComPtr<ID3D12Resource> mVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW mVertexBufferView;

	ComPtr<ID3D12Resource> mIndexBuffer;
	D3D12_INDEX_BUFFER_VIEW mIndexBufferView;

	UINT mIndexCount = 0;

	ComPtr<ID3D12Resource> mConstantBuffer;
	ComPtr<ID3D12DescriptorHeap> mCbvHeap; // 상수 버퍼 뷰 디스크립터 힙
	UINT8* mMappedData = nullptr; // CPU가 데이터를 사용할 메모리 포인터

	float mTheta = 0.0f; // 회전 각도

	XMFLOAT4X4 mWorldMatrix = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f }; // 월드 행렬
	XMFLOAT4X4 mViewMatrix = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f }; // 뷰 행렬
	XMFLOAT4X4 mProjMatrix = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f }; // 투영 행렬

	ComPtr<ID3D12Resource> mTexture;
	ComPtr<ID3D12Resource> mTextureUpload;
	ComPtr<ID3D12DescriptorHeap> mSrvHeap; // 셰이더 리소스 뷰 디스크립터 힙
};

