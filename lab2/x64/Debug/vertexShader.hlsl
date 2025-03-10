// Разделяем буферы на два отдельных с разными регистрами
cbuffer SceneMatrixBuffer : register(b0)
{
    float4x4 mViewProjectionMatrix;
};

cbuffer WorldMatrixBuffer : register(b1)
{
    float4x4 mWorldMatrix;
};

struct VS_INPUT
{
    float4 position : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 sv_position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 world_position : TEXCOORD1;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    // Умножаем позицию сначала на мировую матрицу, затем на view-projection
    float4 worldPos = mul(input.position, mWorldMatrix);
    output.sv_position = mul(worldPos, mViewProjectionMatrix);
    output.world_position = worldPos.xyz;
    output.color = input.color;
    output.uv = input.uv;
    return output;
}