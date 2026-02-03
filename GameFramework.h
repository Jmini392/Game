#pragma once
#include "Timer.h"
#include "Scene.h"
#include "Camera.h"

class CGameFramework
{
private:
	HINSTANCE m_hInstance;
	HWND      m_hWnd;

	int m_nWndClientWidth;
	int m_nWndClientHeight;

	IDXGIFactory4* m_pdxgiFactory;
	//DXGI 팩토리 인터페이스 포인터
	IDXGISwapChain3* m_pdxgiSwapChain;
	//DXGI 스왑체인 인터페이스 포인터 / 주로 디스플레이를 제어하기 위해 필요
	ID3D12Device* m_pd3dDevice;
	//디바이스 인터페이스 포인터 / 주로 리소스를 생성하기 위해 필요

	bool m_bMsaa4xEnable = false;
	UINT m_nMsaa4xQualityLevels = 0;
	// MSAA 다중 샘플링 활성화, 다중 샘플링 레벨 설정.

	static const UINT m_nSwapChainBuffers = 2;
	// 스왑 체인 후면 버퍼의 개수.
	UINT m_nSwapChainBufferIndex;
	// 현재 스왑 체인 후면 버퍼 인덱스.

	ID3D12Resource* m_ppd3dRenderTargetBuffers[m_nSwapChainBuffers];
	ID3D12DescriptorHeap* m_pd3dRtvDescriptorHeap;
	UINT m_nRtvDescriptorIncrementSize;
	// 렌더 타켓 버퍼, 서술자 힙 인터페이스 포인터, 렌더 타겟 서술자 원소의 크기.

	ID3D12Resource* m_pd3dDepthStencilBuffer;
	ID3D12DescriptorHeap* m_pd3dDsvDescriptorHeap;
	UINT m_nDsvDescriptorIncrementSize;
	// 깊이-스텐실 버퍼, 서술자 힙 인터페이스 포인터, 깊이-스텐실 서술자 원소의 크기.

	ID3D12CommandQueue* m_pd3dCommandQueue;
	ID3D12CommandAllocator* m_pd3dCommandAllocator;
	ID3D12GraphicsCommandList* m_pd3dCommandList;
	// 명령 큐, 명령 할당자, 명령 리스트 인터페이스 포인터.

	ID3D12PipelineState* m_pd3dPipelineState;
	// 그래픽스 파이프라인 상태 객체 인터페이스 포인터.

	ID3D12Fence* m_pd3dFence;
	UINT64 m_nFenceValues[m_nSwapChainBuffers];
	HANDLE m_hFenceEvent;
	// 펜스 인터페이스 포인터, 펜스 값, 이벤트 핸들.

	CGameTimer m_GameTimer;
	// 게임 프레임워크에서 사용할 타이머

	_TCHAR m_pszFrameRate[50];
	// 프레임 레이트를 주 윈도우의 캡션에 출력하기 위한 문자열

	CScene* m_pScene;
public:
	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	// 프레임워크 초기화 함수(주 윈도우 생성 시 호출)

	void OnDestroy();

	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateDirect3DDevice();
	void CreateCommandQueueAndList();
	// 스왑 체인, 디바이스, 서술자 힙, 명령 큐/할당자/리스트 생성 함수

	void CreateRenderTargetViews();
	void CreateDepthStencilView();
	// 렌더 타겟 뷰와 깊이-스텐실 뷰 생성 함수

	void BuildObjects();
	void ReleaseObjects();
	// 렌더링할 메쉬와 게임 객체를 생성하고 소멸하는 함수

	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();
	// 프레임워크의 핵심(사용자 입력, 애니메이션, 렌더링)을 구성하는 함수

	void WaitForGpuComplete();
	// CPU와 GPU를 동기화하는 함수

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	// 윈도우의 메시지(키보드, 마우스 입력)를 처리하는 함수

	void ChangeSwapChainState();

	void MoveToNextFrame();

	CCamera* m_pCamera = NULL;
};

