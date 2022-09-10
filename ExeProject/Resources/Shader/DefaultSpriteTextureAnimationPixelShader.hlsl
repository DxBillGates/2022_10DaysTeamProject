#include "DefaultShaderInfo.hlsli"

Texture2D<float4> tex : register(t0);

float4 main(DefaultSpriteVSOutput input) : SV_TARGET
{
	float4 texColor = tex.Sample(clampPointSampler,input.uv);
	return texColor * color;
}