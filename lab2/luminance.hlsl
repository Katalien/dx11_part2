Texture2D<float4> HDRTexture : register(t0);
RWTexture2D<float4> Result : register(u0);

[numthreads(16, 16, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    float3 color = HDRTexture[id.xy].rgb;
    float luminance = dot(color, float3(0.2126, 0.7152, 0.0722));
    Result[id.xy] = float4(luminance, 0, 0, 0);
}