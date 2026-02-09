#include "stdafx.h"
#include "PlayScene.h"

PlayScene::PlayScene()
{
}

PlayScene::~PlayScene()
{
}

void PlayScene::Enter(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// 카메라 설정 (부모 Scene에서 이미 생성됨)
	if (m_pCamera) {
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		m_pCamera->GenerateProjectionMatrix(1.0f, 500.0f, ASPECT_RATIO, 90.0f);
		m_pCamera->GenerateViewMatrix(
			XMFLOAT3(0.0f, 30.0f, -100.0f),
			XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT3(0.0f, 1.0f, 0.0f)
		);
	}
	// 쉐이더 설정
	DiffusedShader* pShader = new DiffusedShader();
	pShader->CreateShader(pd3dDevice);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	// 메쉬 설정
	CubeMeshDiffused* pCuboidMesh = new CubeMeshDiffused(pd3dDevice, pd3dCommandList, 100.0f, 1.0f, 240.0f);
	CubeMeshDiffused* pCubeMesh = new CubeMeshDiffused(pd3dDevice, pd3dCommandList, 10.0f, 10.0f, 10.0f);

	// 바닥 오브젝트 생성
	Object* FloorObj = new Object();
	FloorObj->SetMesh(pCuboidMesh);
	FloorObj->SetPos(0.0f, -5.0f, 0.0f);
	FloorObj->SetShader(pShader);
	AddObject(FloorObj, GROUP_TYPE::PLAY);

	// 장애물 오브젝트 생성
	for (int i = 0; i < 5; ++i) {
		Object* ObsObj = new Object();
		ObsObj->SetMesh(pCubeMesh);
		ObsObj->SetPos(-40.0f + i * 20.0f, 0.0f, 10.0f + i * 10.0f);
		ObsObj->SetShader(pShader);
		AddObject(ObsObj, GROUP_TYPE::PLAY);
	}	
}

void PlayScene::Exit()
{
}