#pragma once
#include "define.h"
#include "Object.h"

class Obstacle : public Object
{
private:
    float m_fWidth;
    float m_fLength;
    float m_fHeight;

    XMFLOAT3 m_xmf3Position;    // 위치
    XMFLOAT4 m_xmf4Color;       // 색상
public:
    Obstacle();
    Obstacle(float fWidth, float fLength, float fHeight);
    virtual ~Obstacle();

    void CreateObstacle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
    void SetObstacleDimensions(float fWidth, float fLength, float fHeight);
    void SetObstacleColor(XMFLOAT4 color);

    void SetPosition(float x, float y, float z);
    void SetPosition(XMFLOAT3 position);
    XMFLOAT3 GetPosition() const { return m_xmf3Position; }

    float GetWidth() const { return m_fWidth; }
    float GetLength() const { return m_fLength; }
    float GetHeight() const { return m_fHeight; }

    virtual void Update() override;
    virtual void Animate(float fTimeElapsed) override;
    virtual void OnPrepareRender() override;

protected:
    void UpdateWorldMatrix();
    void CreateObstacleMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
};

