#include "stdafx.h"
#include "TestScene.h"
#include "Road.h"
#include "Obstacle.h"

TestScene::TestScene()
{
}

TestScene::~TestScene()
{
}

void TestScene::Enter(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
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

	Road* pRoad = new Road(10.0f, 100.0f, 0.1f);
	pRoad->CreateRoad(pd3dDevice, pd3dCommandList);
	pRoad->SetPosition(0.0f, 0.0f, 30.0f);
	Obstacle* pObstacle = new Obstacle(2.0f, 2.0f, 2.0f);
	pObstacle->CreateObstacle(pd3dDevice, pd3dCommandList);
	pObstacle->SetPosition(0.0f, 1.0f, 10.0f);
	DiffusedShader* pShader = new DiffusedShader();
	pShader->CreateShader(pd3dDevice);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pRoad->SetShader(pShader);
	AddObject(pRoad, GROUP_TYPE::TEST);
	pObstacle->SetShader(pShader);
	AddObject(pObstacle, GROUP_TYPE::TEST);
}

void TestScene::Exit()
{
}