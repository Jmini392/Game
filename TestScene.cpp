#include "stdafx.h"
#include "TestScene.h"
#include "Road.h"
#include "Obstacle.h"
#include <random>
#include <vector>
#include <set>

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
	
	DiffusedShader* pShader = new DiffusedShader();
	pShader->CreateShader(pd3dDevice);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pRoad->SetShader(pShader);
	AddObject(pRoad, GROUP_TYPE::TEST);

	// 장애물 랜덤 배치 로직
	CreateRandomObstacles(pd3dDevice, pd3dCommandList, pShader, 40);
}

void TestScene::CreateRandomObstacles(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
									  DiffusedShader* pShader, int obstacleCount)
{
	const float ROAD_WIDTH = 10.0f;
	const float OBSTACLE_WIDTH = 2.0f;
	const float ROAD_LENGTH = 100.0f;
	const int MAX_OBSTACLES_PER_ROW = 4; // 최대 4개까지만 배치
	const int POSSIBLE_POSITIONS_PER_ROW = 5; // 가로 10에서 장애물 크기 2로 나누면 5개 위치 가능
	
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> zDist(0.0f, ROAD_LENGTH); // Z축 위치 (길의 길이 범위)
	std::uniform_int_distribution<int> xPosDist(0, POSSIBLE_POSITIONS_PER_ROW - 1); // X축 위치 (0~4)
	std::uniform_int_distribution<int> obstaclesPerRowDist(1, MAX_OBSTACLES_PER_ROW); // 한 행당 장애물 개수

	std::vector<std::pair<float, float>> obstaclePositions; // (x, z) 위치 저장
	
	for (int i = 0; i < obstacleCount; ++i) {
		bool positionFound = false;
		int attempts = 0;
		const int MAX_ATTEMPTS = 100;
		
		while (!positionFound && attempts < MAX_ATTEMPTS) {
			float z = zDist(gen);
			int xIndex = xPosDist(gen);
			
			// X축 위치 계산: 도로 중앙을 기준으로 좌우 대칭 배치
			float x = (xIndex - 2.0f) * OBSTACLE_WIDTH; // -4, -2, 0, 2, 4 위치
			
			// 같은 Z축 라인에 있는 장애물 개수 확인
			int obstaclesInSameRow = 0;
			const float Z_TOLERANCE = 2.0f; // Z축 같은 라인으로 간주할 범위
			
			for (const auto& pos : obstaclePositions) {
				if (abs(pos.second - z) < Z_TOLERANCE) {
					obstaclesInSameRow++;
				}
			}
			
			// 중복 위치 확인
			bool positionOccupied = false;
			for (const auto& pos : obstaclePositions) {
				if (abs(pos.first - x) < OBSTACLE_WIDTH && abs(pos.second - z) < 2.0f) {
					positionOccupied = true;
					break;
				}
			}
			
			// 한 행에 최대 4개까지만 배치하고 위치가 중복되지 않는 경우
			if (obstaclesInSameRow < MAX_OBSTACLES_PER_ROW && !positionOccupied) {
				obstaclePositions.push_back({x, z});
				
				// 장애물 생성
				Obstacle* pObstacle = new Obstacle(OBSTACLE_WIDTH, 2.0f, 2.0f);
				pObstacle->CreateObstacle(pd3dDevice, pd3dCommandList);
				pObstacle->SetPosition(x, 1.0f, z);
				pObstacle->SetShader(pShader);
				AddObject(pObstacle, GROUP_TYPE::TEST);
				
				positionFound = true;
			}
			
			attempts++;
		}
	}
}

void TestScene::Exit()
{
}