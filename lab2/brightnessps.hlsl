Texture2D HDRTexture : register(t0);
SamplerState Sampler : register(s0);

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

float Luminance(float3 color)
{
    return dot(color, float3(0.2126, 0.7152, 0.0722));
}

float main(PS_INPUT input) : SV_TARGET
{
    float3 color = HDRTexture.Sample(Sampler, input.Tex).rgb;
    return log(Luminance(color) + 1.0f);
}