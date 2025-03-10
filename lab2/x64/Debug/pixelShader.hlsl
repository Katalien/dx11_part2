struct PointLight
{
    float4 Position; // В мировых координатах
    float4 Color;
    float Intensity;
};

cbuffer LightBuffer : register(b2)
{
    PointLight Lights[3];
};

struct PS_INPUT
{
    float4 sv_position : SV_POSITION;
    float4 color : COLOR; // Порядок должен совпадать с VS_OUTPUT!
    float2 uv : TEXCOORD0;
    float3 world_position : TEXCOORD1;
};


float4 CalculateLighting(PS_INPUT input, PointLight light)
{
    // Используем мировые координаты вершины и света
    float3 lightDir = normalize(light.Position.xyz - input.world_position);
    float distance = length(light.Position.xyz - input.world_position);
    float attenuation = 1.0 / (distance * distance);
    float3 lightColor = light.Color.rgb * light.Intensity * attenuation;
    return float4(lightColor * input.color.rgb, input.color.a);
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 finalColor = float4(0, 0, 0, input.color.a);
    for (int i = 0; i < 3; i++)
    {
        finalColor += CalculateLighting(input, Lights[i]);
    }
    return finalColor;
}