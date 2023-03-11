#include "ShaderInclude.hlsli"

float4 main(TransformedVertex input) : SV_TARGET
{
	const float2 uv = input.uv * uvTiling + uvOffset;
	const float4 baseColour = diffuseTexture.Sample(simp, uv);
	const float4 specular = specularTexture.Sample(simp, uv);
	const float4 ambient = ambientTexture.Sample(simp, uv);
	const float3 posToCam = normalize(camPos - input.worldPos);

	PointLight poLight;
	DirectionalLight dirLight;

	float distance;
	float3 posToLight;

	float4 diffShading = float4(0.f, 0.f, 0.f, 1.f);
	float4 specShading = float4(0.f, 0.f, 0.f, 1.f);

	uint i = 0u;

	// Point Lights 
	for (i = 0; i < numPoint; i++)
	{
		poLight = pointLights[i];
		
		posToLight = poLight.position - input.worldPos;
		distance = length(posToLight);
		posToLight /= distance;
		
		const float attu = (1.f / (1.f + poLight.attenuation.x * distance + poLight.attenuation.y * distance * distance));
		
		specShading += specular * pow(max(dot(reflect(-posToLight, input.normal), posToCam), 0.f), shinyness) * 
			float4(poLight.colour, 1.f) * poLight.intensity * attu;
		
		diffShading += baseColour * max(dot(input.normal, posToLight), 0.f) * float4(poLight.colour, 1.f) * poLight.intensity * attu;
	}

	// Directional Lights
	for (i = 0; i < numDir; i++)
	{
		dirLight = dirLights[i];

		specShading += specular * pow(max(dot(reflect(dirLight.direction, input.normal), posToCam), 0.f), shinyness) * 
			float4(dirLight.colour, 1.f) * dirLight.intensity;

		diffShading += max(dot(input.normal, -dirLight.direction), 0.f) * float4(dirLight.colour, 1.f) * dirLight.intensity;
	}
	

	return saturate(baseColour * (float4(ambientTint, 0.f) * ambientTintIntensity + diffShading + specShading));
}