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
    
    output.position = float4(input.position, 1.0); // 위치 변환 (동차 좌표로 변환)
    output.color = input.color;                     // 색상 전달
    
    return output;
}

float4 PS(PixelInput input) : SV_TARGET // 픽셀 셰이더
{
    return input.color; // 입력된 색상을 그대로 출력
}