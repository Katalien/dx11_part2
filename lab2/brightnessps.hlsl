Texture2D HDRTexture : register(t0);
SamplerState Sampler : register(s0);

struct PS_OUTPUT
{
    float4 avg : SV_Target0;
    float4 min : SV_Target1;
    float4 max : SV_Target2;
};

float3 RGBToLuminance(float3 rgb)
{
    return dot(rgb, float3(0.2126, 0.7152, 0.0722));
}

PS_OUTPUT main(float4 pos : SV_POSITION, float2 texCoord : TEXCOORD)
{
    PS_OUTPUT output;
    
    float3 color = HDRTexture.Sample(Sampler, texCoord).rgb;
    float luminance = RGBToLuminance(color);
    
    output.avg = float4(luminance, luminance, luminance, 1.0);
    output.min = float4(luminance, luminance, luminance, 1.0);
    output.max = float4(luminance, luminance, luminance, 1.0);
    
    return output;
}
