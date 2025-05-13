Texture2D HDRTexture : register(t0);
SamplerState Sampler : register(s0);

float4 main(float4 position : SV_POSITION, float2 texCoord : TEXCOORD) : SV_TARGET
{
    uint width, height;
    HDRTexture.GetDimensions(width, height);
    float2 texelSize = 1.0 / float2(width, height);
    
    
    float3 color1 = HDRTexture.Sample(Sampler, texCoord + float2(-0.5, -0.5) * texelSize).rgb;
    float3 color2 = HDRTexture.Sample(Sampler, texCoord + float2(-0.5, 0.5) * texelSize).rgb;
    float3 color3 = HDRTexture.Sample(Sampler, texCoord + float2(0.5, -0.5) * texelSize).rgb;
    float3 color4 = HDRTexture.Sample(Sampler, texCoord + float2(0.5, 0.5) * texelSize).rgb;
    
    
    float lum1 = dot(color1, float3(0.2126, 0.7152, 0.0722));
    float lum2 = dot(color2, float3(0.2126, 0.7152, 0.0722));
    float lum3 = dot(color3, float3(0.2126, 0.7152, 0.0722));
    float lum4 = dot(color4, float3(0.2126, 0.7152, 0.0722));
    
    
    float avgLum = (lum1 + lum2 + lum3 + lum4) * 0.25;
    
    return float4(avgLum, avgLum, avgLum, 1);
}