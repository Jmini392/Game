#include "stdafx.h"
#include "SceneMgr.h"
#include "Scene.h"
#include "TestScene.h"
#include "KeyMgr.h"
#include "PlayScene.h"

SceneMgr::SceneMgr()
    : currScene(nullptr)
    , m_bSceneChanging(false)
    , m_eNextSceneType(SCENE_TYPE::END)
    , m_fTransitionTimer(0.0f)
    , m_bTransitionEffect(false)
{
    // 씬 배열 초기화
    for (int i = 0; i < (UINT)SCENE_TYPE::END; i++) {
        arrScene[i] = nullptr;
    }
}

SceneMgr::~SceneMgr()
{
    // 모든 씬 해제
    for (int i = 0; i < (UINT)SCENE_TYPE::END; i++) {
        if (arrScene[i]) {
            delete arrScene[i];
            arrScene[i] = nullptr;
        }
    }
}

void SceneMgr::Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
<<<<<<< HEAD
    // 씬들 생성
    arrScene[(UINT)SCENE_TYPE::TEST] = new TestScene();
    // 추가 씬들 생성...
    // arrScene[(UINT)SCENE_TYPE::MENU] = new MenuScene();
    // arrScene[(UINT)SCENE_TYPE::GAME] = new GameScene();
    
    // 시작 씬 설정
    currScene = arrScene[(UINT)SCENE_TYPE::TEST];
    if (currScene) {
        currScene->Enter(pd3dDevice, pd3dCommandList);
    }
=======
	//Scene ����
	arrScene[(UINT)SCENE_TYPE::TEST] = new PlayScene;
	arrScene[(UINT)SCENE_TYPE::TEST]->SetName(L"Play Scene");
	//arrScene[(UINT)SCENE_TYPE::STAGE_O1] = new Scene_Start;
	//arrScene[(UINT)SCENE_TYPE::STAGE_O2] = new Scene_Start;

	//���� �� ����
	currScene = arrScene[(UINT)SCENE_TYPE::TEST];
	currScene->Enter(pd3dDevice, pd3dCommandList);
>>>>>>> JI_MIN
}

void SceneMgr::Update(float fTimeElapesd)
{
<<<<<<< HEAD
    // 🔄 씬 전환 처리
    if (m_bSceneChanging) {
        ProcessSceneChange();
        return;
    }
    
    // 🎯 씬 전환 트리거 확인
    CheckSceneTransitionTriggers();
    
    // 현재 씬 업데이트
    if (currScene) {
        currScene->Update();
    }
}

void SceneMgr::CheckSceneTransitionTriggers()
{
    // 1. 키보드 트리거 확인
    SCENE_TYPE keyboardTrigger = CheckKeyboardTriggers();
    if (keyboardTrigger != SCENE_TYPE::END) {
        RequestSceneChange(keyboardTrigger);
        return;
    }
    
    // 2. 게임 로직 트리거 확인
    SCENE_TYPE gameLogicTrigger = CheckGameLogicTriggers();
    if (gameLogicTrigger != SCENE_TYPE::END) {
        RequestSceneChange(gameLogicTrigger);
        return;
    }
}

SCENE_TYPE SceneMgr::CheckKeyboardTriggers()
{    
    // 숫자 키로 씬 전환 (예시)
    if (KEY_TAP(KEY::T)) {
        return SCENE_TYPE::TEST;
    }
    
    return SCENE_TYPE::END;  // 트리거 없음
}

SCENE_TYPE SceneMgr::CheckGameLogicTriggers()
{
    if (!currScene) return SCENE_TYPE::END;
    
    // 현재 씬의 상태에 따른 전환 로직
    // 예: 플레이어가 특정 위치에 도달했을 때
    // 예: 게임 오버 상태일 때
    // 예: 레벨 클리어했을 때
    
    // TestScene에서의 특정 조건 확인 예시
    if (GetCurrentSceneType() == SCENE_TYPE::TEST) {
        // 예: ESC키를 누르면 메뉴로 이동
        if (KEY_TAP(KEY::ESC)) {
            // return SCENE_TYPE::MENU;  // 메뉴 씬이 있다면
        }
        
        // 예: 게임 클리어 조건
        // if (IsGameCleared()) {
        //     return SCENE_TYPE::RESULT;
        // }
    }
    
    return SCENE_TYPE::END;  // 트리거 없음
}

void SceneMgr::RequestSceneChange(SCENE_TYPE nextScene)
{
    if (nextScene == GetCurrentSceneType() || nextScene >= SCENE_TYPE::END) {
        return;  // 같은 씬이거나 잘못된 씬
    }
    
    if (!arrScene[(UINT)nextScene]) {
        return;  // 존재하지 않는 씬
    }
    
    m_eNextSceneType = nextScene;
    m_bSceneChanging = true;
    
    #ifdef _DEBUG
    OutputDebugStringA("Scene change requested\n");
    #endif
}

void SceneMgr::ChangeScene(SCENE_TYPE nextScene)
{
    if (nextScene >= SCENE_TYPE::END || !arrScene[(UINT)nextScene]) {
        return;
    }
    
    // 현재 씬 종료
    if (currScene) {
        currScene->Exit();
    }
    
    // 새 씬으로 전환
    currScene = arrScene[(UINT)nextScene];
    if (currScene) {
        // 씬 초기화는 이미 Init에서 했으므로 Enter만 호출
        // currScene->Enter(device, commandList);  // 필요시 디바이스 정보 전달
    }
    
    #ifdef _DEBUG
    char debugMsg[100];
    sprintf_s(debugMsg, "Scene changed to: %d\n", (int)nextScene);
    OutputDebugStringA(debugMsg);
    #endif
}

// 🔄 실제 씬 전환 처리 (내부 함수)
void SceneMgr::ProcessSceneChange()
{
    if (!m_bSceneChanging || m_eNextSceneType >= SCENE_TYPE::END) {
        return;
    }
    
    // 전환 효과가 있다면 시간 체크
    if (m_bTransitionEffect) {
        m_fTransitionTimer += 0.016f;  // 대략적인 델타타임
        if (m_fTransitionTimer < 0.5f) {  // 0.5초 전환 시간
            return;  // 아직 전환 시간이 안됨
        }
        m_fTransitionTimer = 0.0f;
    }
    
    // 실제 씬 전환 수행
    ChangeScene(m_eNextSceneType);
    
    // 전환 완료
    m_bSceneChanging = false;
    m_eNextSceneType = SCENE_TYPE::END;
}

// 📝 현재 씬 타입 반환
SCENE_TYPE SceneMgr::GetCurrentSceneType() const
{
    for (int i = 0; i < (UINT)SCENE_TYPE::END; i++) {
        if (arrScene[i] == currScene) {
            return (SCENE_TYPE)i;
        }
    }
    return SCENE_TYPE::END;
=======
	currScene->Update(fTimeElapesd);
>>>>>>> JI_MIN
}

void SceneMgr::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
    if (currScene) {
        currScene->Render(pd3dCommandList);
    }
    
    // 전환 효과 렌더링 (필요시)
    if (m_bSceneChanging && m_bTransitionEffect) {
        // 페이드 아웃/인 효과 등 구현
        // RenderTransitionEffect(pd3dCommandList);
    }
}
