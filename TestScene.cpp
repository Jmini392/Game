#include "stdafx.h"
#include "TestScene.h"

TestScene::TestScene()
{
}

TestScene::~TestScene()
{
}

void TestScene::Enter(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// 카메라 설정 (부모 Scene에서 이미 생성됨)
	if (m_pCamera) {
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		m_pCamera->GenerateProjectionMatrix(1.0f, 500.0f, ASPECT_RATIO, 90.0f);
		m_pCamera->GenerateViewMatrix(
			XMFLOAT3(5.0f, 10.0f, -20.0f),
			XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT3(0.0f, 1.0f, 0.0f)
		);
	}

	CubeMeshDiffused* pCubeMesh = new CubeMeshDiffused(pd3dDevice, pd3dCommandList, 12.0f, 12.0f, 12.0f);
	Object* TestObj = new Object();
	TestObj->SetMesh(pCubeMesh);
	DiffusedShader* pShader = new DiffusedShader();
	pShader->CreateShader(pd3dDevice);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	TestObj->SetShader(pShader);
	AddObject(TestObj, GROUP_TYPE::TEST);
}

void TestScene::Exit()
{
}