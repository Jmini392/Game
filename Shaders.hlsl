cbuffer cbPerObject : register(b0) // 상수 버퍼 선언
{
    float4x4 worldViewProj; // 월드-뷰 투영 행렬
}

struct VertexInput // 정점 입력 구조체
{
    float3 position : POSITION; // 위치 (x,y,z)
    float4 color : COLOR;      // 색상 (r,g,b,a)
};

struct PixelInput // 픽셀 입력 구조체
{
    float4 position : SV_POSITION; // 화면 위치 (x,y,z,w), w는 동차 좌표
    float4 color : COLOR;          // 색상 (r,g,b,a)
};

PixelInput VS(VertexInput input) // 정점 셰이더
{
    PixelInput output;
    
    float4 pos = float4(input.position, 1.0f); // 위치를 동차 좌표로 변환, w가 1.0f여야 위치 계산에 올바르게 사용됨
    
    output.position = mul(pos, worldViewProj); // 변환된 위치 계산, 행렬 곱셈
    output.color = input.color;                     // 색상 전달
    
    return output;
}

float4 PS(PixelInput input) : SV_TARGET // 픽셀 셰이더
{
    return input.color; // 입력된 색상을 그대로 출력
}