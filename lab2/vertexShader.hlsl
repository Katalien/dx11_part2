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
    float3 position : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL; // Добавляем нормаль
};

struct VS_OUTPUT
{
    float4 sv_position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 world_position : TEXCOORD1;
    float3 normal : NORMAL; // Передаем нормаль в пиксельный шейдер
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    float4 worldPos = mul(mWorldMatrix, float4(input.position, 1.0f));
    output.sv_position = mul(mViewProjectionMatrix, worldPos);
    output.world_position = worldPos.xyz;
    output.color = input.color;
    output.uv = input.uv;
    output.normal = mul((float3x3) mWorldMatrix, input.normal); // Трансформируем нормаль
    return output;
}