#include "stdafx.h"
#include "Obstacle.h"
#include "Mesh.h"
#include "Shader.h"

Obstacle::Obstacle()
    : m_fWidth(4.0f)
    , m_fLength(10.0f)
    , m_fHeight(0.1f)
    , m_xmf3Position(0.0f, 0.0f, 0.0f)
    , m_xmf4Color(1.0f, 0.0f, 0.0f, 1.0f)
{
    XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
}

Obstacle::Obstacle(float fWidth, float fLength, float fHeight)
    : m_fWidth(fWidth)
    , m_fLength(fLength)
    , m_fHeight(fHeight)
    , m_xmf3Position(0.0f, 0.0f, 0.0f)
    , m_xmf4Color(1.0f, 0.0f, 0.0f, 1.0f)
{
    XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
}

Obstacle::~Obstacle()
{
}

void Obstacle::CreateObstacle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
    CreateObstacleMesh(pd3dDevice, pd3dCommandList);
    UpdateWorldMatrix();
}

void Obstacle::SetObstacleDimensions(float fWidth, float fLength, float fHeight)
{
    m_fWidth = fWidth;
    m_fLength = fLength;
    m_fHeight = fHeight;
}

void Obstacle::SetObstacleColor(XMFLOAT4 color)
{
    m_xmf4Color = color;
}

void Obstacle::CreateObstacleMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
    CubeMeshDiffused* pObstacleMesh = new CubeMeshDiffused(
        pd3dDevice,
        pd3dCommandList,
        m_fWidth,
        m_fHeight,
        m_fLength,
        m_xmf4Color.x,
        m_xmf4Color.y,
        m_xmf4Color.z
    );

    SetMesh(pObstacleMesh);
}

void Obstacle::SetPosition(float x, float y, float z)
{
    m_xmf3Position = XMFLOAT3(x, y, z);
    UpdateWorldMatrix();
}

void Obstacle::SetPosition(XMFLOAT3 position)
{
    m_xmf3Position = position;
    UpdateWorldMatrix();
}

void Obstacle::UpdateWorldMatrix()
{
    XMMATRIX mtxTranslation = XMMatrixTranslation(
        m_xmf3Position.x,
        m_xmf3Position.y,
        m_xmf3Position.z
    );
    XMStoreFloat4x4(&m_xmf4x4World, mtxTranslation);
}

void Obstacle::Update()
{
    Object::Update();
}

void Obstacle::Animate(float fTimeElapsed)
{
    Object::Animate(fTimeElapsed);
}

// 🎨 렌더링 준비
void Obstacle::OnPrepareRender()
{
    Object::OnPrepareRender();
}