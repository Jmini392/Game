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
			XMFLOAT3(0.0f, 10.0f, -20.0f),
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

	// UI: 좌측 상단 쿼드
	//QuadMeshDiffused (가로길이, 세로길이, x, y 좌표)
	QuadMeshDiffused* pQuadLeft = new QuadMeshDiffused(pd3dDevice, pd3dCommandList, 200.0f, 50.0f, 10.0f, 10.0f);
	Object* pUILeft = new Object();
	pUILeft->SetMesh(pQuadLeft);
	UIScreenShader* pUIShaderLeft = new UIScreenShader();
	pUIShaderLeft->CreateShader(pd3dDevice);
	pUIShaderLeft->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	// 직교 설정: left=0, right=FRAME_BUFFER_WIDTH, top=0, bottom=FRAME_BUFFER_HEIGHT (왼쪽-상단 원점)
	pUIShaderLeft->SetOrtho(0.0f, (float)FRAME_BUFFER_WIDTH, 0.0f, (float)FRAME_BUFFER_HEIGHT);
	pUILeft->SetShader(pUIShaderLeft);
	AddObject(pUILeft, GROUP_TYPE::UI);

	// UI: 우측 상단 쿼드
	float uiWidth = 200.0f, uiHeight = 50.0f, margin = 10.0f;
	//QuadMeshDiffused* pQuadRight = new QuadMeshDiffused(pd3dDevice, pd3dCommandList,
	//	uiWidth, uiHeight, (float)FRAME_BUFFER_WIDTH - uiWidth - margin, 10.0f);
	QuadMeshDiffused* pQuadRight = new QuadMeshDiffused(pd3dDevice, pd3dCommandList, 200.0f, 50.0f, 600.0f, 10.0f);
	Object* pUIRight = new Object();
	pUIRight->SetMesh(pQuadRight);
	UIScreenShader* pUIShaderRight = new UIScreenShader();
	pUIShaderRight->CreateShader(pd3dDevice);
	pUIShaderRight->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pUIShaderRight->SetOrtho(0.0f, (float)FRAME_BUFFER_WIDTH, 0.0f, (float)FRAME_BUFFER_HEIGHT);
	pUIRight->SetShader(pUIShaderRight);
	AddObject(pUIRight, GROUP_TYPE::UI);
	
}

void TestScene::Exit()
{
}