#include "shaders/shared.h"

cbuffer PerFrame : register(PER_FRAME_CB_SLOT)
{
	PerFrameData frameData;
}

cbuffer PerMesh : register(PER_MESH_CB_SLOT)
{
	PerMeshData meshData;
}

cbuffer PerMaterial : register(PER_MATERIAL_CB_SLOT)
{
	PerMaterialData materialData;
}

cbuffer Lights : register(LIGHTS_CB_SLOT) {
	LightsData lightsData;
}

Texture2D TextureColor : register(TEXTURE_COLOR_SRV_SLOT);
Texture2D TextureAmbient : register(TEXTURE_AMBIENT_SRV_SLOT);
Texture2D TextureSpecular : register(TEXTURE_SPECULAR_SRV_SLOT);


SamplerState LinearSampler : register(LINEAR_SAMPLER_SLOT);
SamplerState NearestSampler : register(NEAREST_SAMPLER_SLOT);

struct VSOut {
	float4 pos : SV_POSITION;
	float3 worldPos : WORLD_POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEX_COORD;
};

VSOut VSMain(VSDefaultIn input) {
	VSOut res;
	res.worldPos = mul(meshData.transform, float4(input.pos, 1)).xyz;
	res.pos = mul(frameData.viewProj, float4(res.worldPos, 1));
	res.normal = input.normal;
	res.texCoord = input.texCoord;
	return res;
}

float3 Phong(float3 l, float3 lc, float3 n, float3 pos, float3 v, float3 Ka, float3 Kd, float3 Ks) {
	float3 res = lc * Ka;
	float nl = max(0, dot(n, -l));
	if (nl > 0) {
		float3 r = reflect(l, n);
		float rv = max(0, dot(r, v));
		res += lc * Kd * nl;
		res += lc * Ks * pow(rv, materialData.ph);
	}

	return res;
}

float4 ApplyPhong(VSOut input, float3 v) {
	float3 Ka = materialData.ambient;
	if (materialData.usedTextures.y)
		Ka *= TextureAmbient.Sample(LinearSampler, input.texCoord).xyz;

	float3 Kd = materialData.diffuse;
	if (materialData.usedTextures.x)
		Kd *= TextureColor.Sample(LinearSampler, input.texCoord).xyz;

	float3 Ks = materialData.specular;
	if (materialData.usedTextures.z)
		Ks *= TextureSpecular.Sample(LinearSampler, input.texCoord).xyz;

	float4 result = float4(0, 0, 0, 1);
	for (int i = 0; i < lightsData.pointAmount; i++) {
		float3 l = normalize(input.worldPos - lightsData.pointLights[i].position);
		float distSq = dot(l, l);
		if (distSq < lightsData.pointLights[i].radius * lightsData.pointLights[i].radius) {
			float3 lNorm = l / sqrt(distSq);
			result.xyz += Phong(lNorm, lightsData.pointLights[i].color, input.normal, input.worldPos, v, Ka, Kd, Ks);
		}
	}

	for (int i = 0; i < lightsData.dirAmount; i++) {
		float3 l = lightsData.dirLights[i].direction;
		float3 lNorm = normalize(l);
		result.xyz += Phong(lNorm, lightsData.pointLights[i].color, input.normal, input.worldPos, v, Ka, Kd, Ks);
	}

	return result;

}

float CountNDF(VSOut input, float3 v, float3 l) {
	float3 h = normalize(v + l);
	float alphSqrt = materialData.roughness * materialData.roughness;
	return alphSqrt / (M_PI * pow(pow(max(dot(input.normal, h), 0.0), 2) * (alphSqrt - 1) + 1, 2));
}

float4 CountNDFColor(VSOut input, float3 v) {
	float4 result = float4(0, 0, 0, 1);
	for (int i = 0; i < lightsData.pointAmount; i++) {
		float3 l = normalize(lightsData.pointLights[i].position - input.worldPos);
		float NDF = CountNDF(input, v, l);
		result.xyz += float3(NDF, NDF, NDF);
	}
	return result;
}

float3 CountFresnelFunc(VSOut input, float3 v, float3 l) {
	float3 h = normalize(v + l);
	float3 f0 = float3(0.04f, 0.04f, 0.04f) * (1 - materialData.metalness) + materialData.reflection * materialData.metalness;
	return f0 + (1 - f0) * pow(1 - dot(h, v), 5);
}

float4 CountFFColor(VSOut input, float3 v) {
	float4 result = float4(0, 0, 0, 1);
	for (int i = 0; i < lightsData.pointAmount; i++) {
		float3 l = normalize(lightsData.pointLights[i].position - input.worldPos);
		result.xyz += dot(input.normal, l) < 0 ? float3(0, 0, 0) : CountFresnelFunc(input, v, l);
	}
	return result;
}

float CountSchlickGGX(float3 n, float3 v, float k) {
	float nv = max(dot(n, v), 0);
	return nv / (nv * (1 - k) + k);
}

float CountGeomFunc(VSOut input, float3 v, float3 l) {
	float k = pow(materialData.roughness + 1, 2) / 8;
	return CountSchlickGGX(input.normal, v, k) * CountSchlickGGX(input.normal, l, k);
}

float4 CountGFColor(VSOut input, float3 v) {
	float4 result = float4(0, 0, 0, 1);
	for (int i = 0; i < lightsData.pointAmount; i++) {
		float3 l = normalize(lightsData.pointLights[i].position - input.worldPos);
		float GF = CountGeomFunc(input, v, l);
		result.xyz += float3(GF, GF, GF);
	}
	return result;
}

float3 CountBRDF(VSOut input, float3 v, float3 li) {
	float3 F = CountFresnelFunc(input, v, li);
	float D = CountNDF(input, v, li);
	float G = CountGeomFunc(input, v, li);
	float3 albedo = TextureColor.Sample(LinearSampler, input.texCoord).xyz;

	float denom = max(4 * max(dot(li, input.normal), 0.0) * max(dot(v, input.normal), 0.0), 0.0001);

	return (float3(1.0, 1.0, 1.0) - F) * albedo / M_PI * (1 - materialData.metalness)
		+ D * F * G / denom;
}

float4 CountPBRColor(VSOut input, float3 v) {
	float4 result = float4(0, 0, 0, 1);
	for (int i = 0; i < lightsData.pointAmount; i++) {
		float3 l = normalize(lightsData.pointLights[i].position - input.worldPos);
		result.xyz += CountBRDF(input, v, l) * lightsData.pointLights[i].color * max(dot(l, input.normal), 0.0);
	}
	return result;
}

float4 PSMain(VSOut input) : SV_TARGET{
	input.normal = normalize(input.normal);
	float3 view = normalize(frameData.cameraPos - input.worldPos);
	
	float4 color = float4(0, 0, 0, 1);
	switch (materialData.shadeModel) {
		case SHADE_MODEL_COLOR:
		{
			color = materialData.color;
			if (materialData.usedTextures.x)
				color *= TextureColor.Sample(LinearSampler, input.texCoord);
			break;
		}
		case SHADE_MODEL_PHONG:
		{
			color = ApplyPhong(input, view);
			break;
		}
		case SHADE_MODEL_NDF:
		{
			color = CountNDFColor(input, view);
			break;
		}
		case SHADE_MODEL_GF:
		{
			color = CountGFColor(input, view);
			break;
		}
		case SHADE_MODEL_FF:
		{
			color = CountFFColor(input, view);
			break;
		}
		case SHADE_MODEL_PBR:
		{
			color = CountPBRColor(input, view);
			break;
		}
		default:
		{
			color = float4(1, 0, 1, 1);
			break;
		}
	}

	return color;
}