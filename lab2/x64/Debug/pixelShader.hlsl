struct PointLight
{
    float4 Position;
    float4 Color;
    float Intensity;
};

cbuffer LightBuffer : register(b2)
{
    PointLight Lights[3]; // Три источника света
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
};

float4 CalculateLighting(PS_INPUT input, PointLight light)
{
    float3 lightDir = normalize(light.Position.xyz - input.position.xyz);
    float distance = length(light.Position.xyz - input.position.xyz);
    float attenuation = 1.0 / (distance * distance);
    float3 lightColor = light.Color.rgb * light.Intensity * attenuation;
    return float4(lightColor * input.color.rgb, input.color.a);
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 finalColor = float4(0, 0, 0, 1);

    // Применение всех источников света
    for (int i = 0; i < 3; i++)
    {
        finalColor += CalculateLighting(input, Lights[i]);
    }

    return finalColor;
}
