#pragma once

class Scene;

class SceneMgr
{
	SINGLE(SceneMgr);
private:
	Scene* arrScene[(UINT)SCENE_TYPE::END];
	Scene* currScene;

public:
	void Init(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void Update(float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList);

	Scene* GetCurScene() { return currScene; }
};

