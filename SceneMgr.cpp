#include "stdafx.h"
#include "SceneMgr.h"
#include "TestScene.h"
#include "PlayScene.h"

SceneMgr::SceneMgr()
	:arrScene()
	, currScene(nullptr)
{

}

SceneMgr::~SceneMgr()
{

}

void SceneMgr::Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	//Scene 생성
	arrScene[(UINT)SCENE_TYPE::TEST] = new TestScene;
	arrScene[(UINT)SCENE_TYPE::TEST]->SetName(L"Test Scene");
	arrScene[(UINT)SCENE_TYPE::PLAY] = new PlayScene;
	arrScene[(UINT)SCENE_TYPE::PLAY]->SetName(L"Play Scene");
	//arrScene[(UINT)SCENE_TYPE::STAGE_O1] = new Scene_Start;
	//arrScene[(UINT)SCENE_TYPE::STAGE_O2] = new Scene_Start;

	//현재 씬 지정
	currScene = arrScene[(UINT)SCENE_TYPE::PLAY];
	currScene->Enter(pd3dDevice, pd3dCommandList);
}

void SceneMgr::Update()
{
	currScene->Update();
}

void SceneMgr::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	currScene->Render(pd3dCommandList);
}
