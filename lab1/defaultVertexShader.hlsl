struct VS_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0; // Добавьте UV-координаты
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0; // Передайте UV-координаты
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    output.position = float4(input.position, 1.0f);
    output.color = input.color;
    output.uv = input.uv; // Передайте UV-координаты
    return output;
}

