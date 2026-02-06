#include "stdafx.h"
#include "SceneMgr.h"
#include "TestScene.h"
#include "CollisionCheckScene.h"

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
	//arrScene[(UINT)SCENE_TYPE::STAGE_O1] = new Scene_Start;
	//arrScene[(UINT)SCENE_TYPE::STAGE_O2] = new Scene_Start;

	arrScene[(UINT)SCENE_TYPE::COLLISION] = new CollisionCheckScene;
	arrScene[(UINT)SCENE_TYPE::COLLISION]->SetName(L"Collision Check Scene");

	//현재 씬 지정
	currScene = arrScene[(UINT)SCENE_TYPE::COLLISION];
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
