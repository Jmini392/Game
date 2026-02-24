#include "stdafx.h"
#include "PlayScene.h"
#include "Ground.h"
#include "Car.h"
#include "sphere.h"
#include "SoundMgr.h"
#include "Tree.h"

PlayScene::PlayScene()
{
}

PlayScene::~PlayScene()
{
}

void PlayScene::Enter(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	SoundMgr::Instance()->Play("bgm", "Res/Sounds/bgm.mp3", 0.5f);
	// 카메라 설정 (부모 Scene에서 이미 생성됨)
	if (m_pCamera) {
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		m_pCamera->GenerateProjectionMatrix(1.0f, 500.0f, ASPECT_RATIO, 90.0f);
		m_pCamera->GenerateViewMatrix(
			XMFLOAT3(0.0f, 90.0f, -50.0f),   // Eye: 차량 뒤쪽 위
			XMFLOAT3(0.0f, 0.0f, 30.0f),     // LookAt: 차량 전방
			XMFLOAT3(0.0f, 1.0f, 0.0f)       // Up
		);
	}

	// 조명 초기화
	if (m_pLight) {
		m_pLight->SetLightColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)); // 흰색 광원
		m_pLight->SetLightDirection(XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f)); // 위→아래 방향광
	}

	Ground* grounds[10];
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 5; j++) {
			POINT chunk = { (j - 2) * 50, i };
			grounds[i] = new Ground(pd3dDevice, pd3dCommandList,chunk );
			AddObject(grounds[i], GROUP_TYPE::TEST);
		}
	}

	Tree* tree = new Tree(pd3dDevice, pd3dCommandList, XMFLOAT3(0.0f, 0.0f, 100.0f));
	AddObject(tree, GROUP_TYPE::TEST);

	Car* car = new Car(pd3dDevice, pd3dCommandList, XMFLOAT3(0.0f, 0.0f, 10.0f), XMFLOAT3(10,10,10));
	AddObject(car, GROUP_TYPE::TEST);

	for (int i = 0; i < 5; i++) {
		MovingSphere* sphere = new MovingSphere(pd3dDevice, pd3dCommandList, -20.0f - i * 10.0f, i);
		AddObject(sphere, GROUP_TYPE::TEST);
	}

	BounsSphere* ball = new BounsSphere(pd3dDevice, pd3dCommandList);
	AddObject(ball, GROUP_TYPE::TEST);

	DiffusedShader* pShader = new DiffusedShader();
	pShader->CreateShader(pd3dDevice);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	SetShaderToAllObjects(pShader);

	
}

void PlayScene::Exit()
{
	SoundMgr::Instance()->Stop("bgm");
}

void PlayScene::RoadMeshes(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{

}