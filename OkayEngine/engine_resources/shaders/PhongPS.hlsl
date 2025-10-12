#include "ShaderInclude.hlsli"

#define LIGHT_DIR (-dirLights[0].direction)
#define LIGHT_COLOUR (dirLights[0].colour * dirLights[0].intensity)
#define PI (3.1415926f)

#if 0
float3 Illumination(float3 pointPos, float3 normal)
{
    float3 vecToLight = normalize(LIGHT_DIR);
    float cosAlpha = max(0.0, dot(normal, vecToLight));

    float3 illumination = LIGHT_COLOUR * cosAlpha;

    return illumination;
}

float3 FresnelSchlick(float3 medianVec, float3 viewVec, float3 F0, float roughness)
{
    float cosAlpha = max(dot(medianVec, viewVec), 0.0);
    return F0 + (max(F0, 1.0 - roughness) - F0) * pow(1.0 - cosAlpha, 5);
}

float TrowbridgeReitzGGX(float3 normalVec, float3 medianVec, float roughness)
{
    float alpha = roughness * roughness;
    float cosBeta = dot(normalVec, medianVec);
    float NDF = alpha / (PI * (cosBeta * cosBeta * (alpha - 1.0) + 1.0) * (cosBeta * cosBeta * (alpha - 1.0) + 1.0));

    return NDF;
}

float SchlickGGX_Direct(float cosAlpha, float roughness)
{
    float k = pow((roughness + 1.0), 2) / 8;

    return cosAlpha / (cosAlpha * (1.0 - k) + k);
}

float SmithGGX_Direct(float3 normalVec, float3 viewVec, float3 lightVec, float roughness)
{
    float cosAlpha = max(dot(normalVec, viewVec), 0.0);
    float cosBeta = max(dot(normalVec, lightVec), 0.0);

    return SchlickGGX_Direct(cosAlpha, roughness) * SchlickGGX_Direct(cosBeta, roughness);
}

float4 main(TransformedVertex input) : SV_TARGET
{
    float2 uv = input.uv * uvTiling + uvOffset;
    float3 albedo = diffuseTexture.Sample(simp, uv).rgb;
    float roughness = specularTexture.Sample(simp, uv).r;
    float metal = 0.f;//metallicTexture.Sample(simp, uv).r;
    float F0 = 0.2f;
    
    float3 normal = normalize(input.normal);
    float3 viewVec = -normalize(camPos - input.worldPos);
    float3 lightVec = LIGHT_DIR;
    float3 lightColor = LIGHT_COLOUR;
    float3 medianVec = normalize(viewVec + lightVec);

    float3 L = Illumination(input.worldPos, normal);
    float3 F = FresnelSchlick(medianVec, viewVec, float3(F0, F0, F0), roughness);
    float D = TrowbridgeReitzGGX(normal, medianVec, roughness);
    float G = SmithGGX_Direct(normal, lightVec, viewVec, roughness);

    float3 Kd = (float3(1.f, 1.f, 1.f) - F) * (1.0 - metal);
    float3 colour = (Kd * albedo / PI + F * D * G / (4.f * max(dot(viewVec, normal), 0.f) * max(dot(lightVec, normal), 0.f) + 0.001f)) * L;
    
    float3 sky = skyBoxTexture.Sample(simp,normal).rgb;

    
    return float4(colour, 1.f);
}

#else

float chiGGX(float v)
{
    return v > 0.f ? 1.f : 0.f;
}

float GGX_Distribution(float3 normal, float3 halfwayVec, float roughness)
{
    float NoH = dot(normal, halfwayVec);
    float roughness2 = roughness * roughness;
    float NoH2 = NoH * NoH;
    float den = NoH2 * roughness2 + (1.f - NoH2);
    return (chiGGX(NoH) * roughness2) / (PI * den * den);
}

float3 SchlicksApproximation(float3 halfwayVec, float3 viewVec, float3 F0, float roughness)
{
    float cosAlpha = max(1.f - dot(halfwayVec, viewVec), 0.f);
    return F0 + (max(F0, 1.f - roughness) - F0) * pow(1.f - cosAlpha, 5.f);
    //return roughness + (1.f - roughness) * pow(1.f - cosAlpha, 5.f);
}

float GGX_PartialGeometryTerm(float3 viewVec, float3 normal, float3 halfwayVec, float roughness)
{
    float VoH2 = saturate(dot(viewVec, halfwayVec));
    float chi = chiGGX(VoH2 / saturate(dot(viewVec, normal)));
    VoH2 = VoH2 * VoH2;
    float tan2 = (1.f - VoH2) / VoH2;
    return (chi * 2.f) / (1.f + sqrt(1.f + roughness * roughness * tan2));
}

float GGX_PartialGeometryTerm2(float3 viewVec, float3 normal, float3 halfwayVec, float3 lightDir, float roughness)
{
    float NoV = saturate(dot(normal, viewVec));
    float NoL = saturate(dot(normal, lightDir));
    float VoH = saturate(dot(viewVec, halfwayVec));
    float chi = chiGGX(VoH / (NoV + 1e-5)) * chiGGX(VoH / (NoL + 1e-5));
    
    float VoH2 = VoH * VoH;
    float tan2 = (1 - VoH2) / (VoH2 * (1 + 1e-5));
    
    float G = 2 / (1 + sqrt(1 + roughness * roughness * tan2));
    return chi * G;
}

// Pixel shader
float4 main(TransformedVertex input) : SV_TARGET
{
    float2 uv = input.uv * uvTiling + uvOffset;
	float3 albedo = diffuseTexture.Sample(simp, uv).rgb;
	float roughness = specularTexture.Sample(simp, uv).r;
	float metallic = metallicTexture.Sample(simp, uv).r;
    float F0 = 0.2f;
    
    float3 normal = normalize(input.normal);
	float3 viewVec = normalize(camPos - input.worldPos);
    float3 lightDir = normalize(float3(5.f, 5.f, 0.f) - input.worldPos);//LIGHT_DIR;
    float3 lightColor = LIGHT_COLOUR;
    float3 halfwayVec = normalize(viewVec + lightDir);

    float D = GGX_Distribution(normal, halfwayVec, roughness);
    float3 F = SchlicksApproximation(halfwayVec, viewVec, float3(F0, F0, F0), roughness);
    float G = GGX_PartialGeometryTerm(viewVec, normal, halfwayVec, roughness);
    
    float VdotN = 1.f - dot(viewVec, normal);
    float NdotL = 1.f - dot(normal, lightDir);
    float ks = (D * F * G) / max((4.f * VdotN * NdotL), 0.0001f);
    ks *= dot(normal, lightDir) > 0.f;
    ks = clamp(ks, 0.f, 1.f);
    
    float kd = (1.f - ks) * (1.f - metallic);
    
    float3 sky = skyBoxTexture.Sample(simp, reflect(-viewVec, normal)).rgb;
    albedo *= sky;
    float3 finalColour = kd * albedo + ks * lightColor;
    
    return float4(finalColour, 1.0);
}
#endif