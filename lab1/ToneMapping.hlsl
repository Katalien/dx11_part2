Texture2D hdrTexture : register(t0);
SamplerState samplerState : register(s0);

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0; // UV-координаты
};

float3 Uncharted2Tonemap(float3 x)
{
    float A = 0.15;
    float B = 0.50;
    float C = 0.10;
    float D = 0.20;
    float E = 0.02;
    float F = 0.30;
    return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F) - E / F);
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 hdrColor = hdrTexture.Sample(samplerState, input.uv).rgb;
    float3 result = Uncharted2Tonemap(hdrColor * 2.0);
    result = result / Uncharted2Tonemap(11.2);
    return float4(pow(result, 1.0 / 2.2), 1.0); // Гамма-коррекция
}