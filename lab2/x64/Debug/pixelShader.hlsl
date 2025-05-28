struct PointLight
{
    float4 Position;
    float4 Color;
    float Intensity;
    float3 Padding;
};

cbuffer LightBuffer : register(b2)
{
    PointLight Lights[3];
};

struct PS_INPUT
{
    float4 sv_position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 world_position : TEXCOORD1;
    float3 normal : NORMAL;
};

float4 CalculateLighting(PS_INPUT input, PointLight light)
{
    float3 lightDir = normalize(light.Position.xyz - input.world_position);
    float distance = length(light.Position.xyz - input.world_position);
    float attenuation = 1.0 / (1.0 + 10 * distance + 100 * distance * distance);
    float3 lightColor = light.Color.rgb * light.Intensity * attenuation * 0.5;


    float NdotL = max(dot(normalize(input.normal), lightDir), 0.0);
    float3 diffuse = lightColor * NdotL;

    return float4(diffuse * input.color.rgb, input.color.a);
}

float4 main(PS_INPUT input) : SV_TARGET
{

    float3 AmbientColor = float3(0.2, 0.0, 0.0);
    float AmbientIntensity = 0.1;

    float4 finalColor = float4(AmbientColor * AmbientIntensity * input.color.rgb, input.color.a);
    for (int i = 0; i < 3; i++)
    {
        finalColor += CalculateLighting(input, Lights[i]);
    }
    return finalColor;
}
