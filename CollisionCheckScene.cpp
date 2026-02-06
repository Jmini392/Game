#include "stdafx.h"
#include "CollisionCheckScene.h"
#include "Player.h"
#include "Mesh.h"
#include "Core.h"

CollisionCheckScene::CollisionCheckScene()
{
}

CollisionCheckScene::~CollisionCheckScene()
{
	if (m_BBMesh) m_BBMesh->Release();
	if (m_BBShader) m_BBShader->Release();
}

void CollisionCheckScene::Enter(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
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

	// 2. 플레이어 생성
	CubeMeshDiffused* pCubeMesh = new CubeMeshDiffused(pd3dDevice, pd3dCommandList, 2.0f, 2.0f, 2.0f);
	Player* pPlayer = new Player(pd3dDevice, pd3dCommandList);

	pPlayer->SetMesh(pCubeMesh);

	pPlayer->SetBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));

	DiffusedShader* pShader = new DiffusedShader();
	pShader->CreateShader(pd3dDevice);

	AddObject(pPlayer, GROUP_TYPE::TEST);

	// 3. 테스트용 장애물 박스 생성 (오른쪽 라인: X = 5.0)
	// Extents(1,1,1)은 플레이어와 동일한 크기
	m_testBB.Center = XMFLOAT3(5.0f, 0.0f, 0.0f);
	m_testBB.Extents = XMFLOAT3(1.0f, 1.0f, 1.0f);

	// 4. 장애물 그리기용 디버그 리소스 생성
	m_BBMesh = new BoundingBoxMesh(pd3dDevice, pd3dCommandList);
	m_BBShader = new Dbg::DebugShader();
	m_BBShader->CreateShader(pd3dDevice);
}

void CollisionCheckScene::Exit()
{
}

void CollisionCheckScene::Update()
{
	Scene::Update(); // 플레이어 이동 (Object::Update 호출됨)

	// --- 충돌 체크 로직 ---
	// 0번 오브젝트(플레이어) 가져오기
	// (Scene 클래스에서 arrObj가 protected여야 접근 가능. private이면 Getter 필요)
	if (arrObj[(UINT)GROUP_TYPE::TEST].empty()) return;

	Player* pPlayer = (Player*)arrObj[(UINT)GROUP_TYPE::TEST][0];

	// 플레이어의 현재 월드 박스 가져오기
	const BoundingBox& playerBB = pPlayer->GetBB();

	// Intersects 함수로 충돌 확인!
	HWND hWnd = Core::Instance()->GetMainHandle();

	if (playerBB.Intersects(m_testBB))
	{
		SetWindowText(hWnd, L"충돌 발생");
	}
	else
	{
		SetWindowText(hWnd, L"이동 중... (충돌 없음)");
	}
}

void CollisionCheckScene::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	Scene::Render(pd3dCommandList); // 플레이어 및 기본 렌더링

	// 장애물 박스 위치 그리기 (빨간 선)
	if (m_BBShader && m_BBMesh)
	{
		XMFLOAT4X4 xmf4x4BoxWorld;

		// 박스 크기(Extents)만큼 확대하고, 위치(Center)로 이동하는 행렬 만들기
		XMMATRIX mtxScale = XMMatrixScaling(m_testBB.Extents.x * 2, m_testBB.Extents.y * 2, m_testBB.Extents.z * 2);
		XMMATRIX mtxTrans = XMMatrixTranslation(m_testBB.Center.x, m_testBB.Center.y, m_testBB.Center.z);
		XMStoreFloat4x4(&xmf4x4BoxWorld, mtxScale * mtxTrans);

		m_BBShader->UpdateShaderVariable(pd3dCommandList, &xmf4x4BoxWorld);
		m_BBShader->OnPrepareRender(pd3dCommandList);
		m_BBMesh->Render(pd3dCommandList);
	}
}