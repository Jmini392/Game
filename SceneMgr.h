#pragma once

#include "define.h"

class Scene;

class SceneMgr
{
    SINGLE(SceneMgr);
private:
    Scene* arrScene[(UINT)SCENE_TYPE::END];
    Scene* currScene;
    
    // 🔄 씬 전환 관련 변수 추가
    bool m_bSceneChanging;              // 씬 전환 중인지 여부
    SCENE_TYPE m_eNextSceneType;        // 전환될 다음 씬
    float m_fTransitionTimer;           // 전환 시간 (필요시)
    bool m_bTransitionEffect;           // 전환 효과 사용 여부

public:
    void Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
    void Update();
    void Render(ID3D12GraphicsCommandList* pd3dCommandList);
	void Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void Update(float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList);

    Scene* GetCurScene() { return currScene; }
    
    // 🔄 씬 전환 관련 함수들 추가
    void ChangeScene(SCENE_TYPE nextScene);            // 즉시 씬 전환
    void RequestSceneChange(SCENE_TYPE nextScene);     // 씬 전환 요청 (다음 프레임에 적용)
    void SetTransitionEffect(bool enable) { m_bTransitionEffect = enable; }
    
    // 📝 씬 상태 확인
    SCENE_TYPE GetCurrentSceneType() const;
    bool IsSceneChanging() const { return m_bSceneChanging; }
    
    // 🎯 트리거 관련 함수들
    void CheckSceneTransitionTriggers();               // 씬 전환 트리거 확인
    
private:
    // 🔧 내부 헬퍼 함수들
    void ProcessSceneChange();                         // 실제 씬 전환 처리
    SCENE_TYPE CheckKeyboardTriggers();               // 키보드 트리거 확인
    SCENE_TYPE CheckGameLogicTriggers();               // 게임 로직 트리거 확인
};

