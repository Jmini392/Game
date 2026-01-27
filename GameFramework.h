#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <DirectXMath.h>

using namespace Microsoft::WRL;
using namespace DirectX;

struct Vertex
{
	XMFLOAT3 position; // x, y, z ÁÂÇ¥
	XMFLOAT4 color;    // RGBA »ö»ó
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

	UINT mRtvDescriptorSize = 0; // ·»´õ Å¸°Ù ºä µð½ºÅ©¸³ÅÍ Å©±â
	UINT mDsvDescriptorSize = 0; // ±íÀÌ-½ºÅÙ½Ç ºä µð½ºÅ©¸³ÅÍ Å©±â
	UINT mCbvSrvUavDescriptorSize = 0; // ¼ÎÀÌ´õ ¸®¼Ò½º ºä µð½ºÅ©¸³ÅÍ Å©±â

	HWND mhMainWnd = nullptr;

	ComPtr<ID3D12DescriptorHeap> mRtvHeap; // ·»´õ Å¸°Ù ºä µð½ºÅ©¸³ÅÍ Èü
	ComPtr<ID3D12DescriptorHeap> mDsvHeap; // ±íÀÌ-½ºÅÙ½Ç ºä µð½ºÅ©¸³ÅÍ Èü

	static const int SwapChainBufferCount = 2;
	ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];
	ComPtr<ID3D12Resource> mDepthStencilBuffer;

	int mCurrBackBuffer = 0;

	// ºäÆ÷Æ®¿Í ½ÃÀú »ç°¢Çü
	D3D12_VIEWPORT mScreenViewport;
	D3D12_RECT mScissorRect;

	ComPtr<ID3D12RootSignature> mRootSignature;
	ComPtr<ID3D12PipelineState> mPipelineState;

	ComPtr<ID3D12Resource> mVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW mVertexBufferView;

	ComPtr<ID3D12Resource> mIndexBuffer;
	D3D12_INDEX_BUFFER_VIEW mIndexBufferView;

	UINT mIndexCount = 0;
};

