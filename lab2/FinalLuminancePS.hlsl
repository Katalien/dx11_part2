Texture2D ColorTexture : register(t0);
SamplerState Sampler : register(s0);

float4 main(float4 position : SV_POSITION, float2 texCoord : TEXCOORD) : SV_TARGET
{
    float3 color = ColorTexture.Sample(Sampler, texCoord).rgb;
    float lum = dot(color, float3(0.2126, 0.7152, 0.0722));
    return float4(lum, lum, lum, 1.0);
}