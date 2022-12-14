#include "DefaultShaderInfo.hlsli"

Texture2D<float4> tex : register(t0);

float4 main(DefaultSpriteVSOutput input) : SV_TARGET
{
	float4 texColor = tex.Sample(wrapPointSampler,input.uv);

	// これしないと画面クリアの色が透明部分に映っちゃう
	texColor.a = 1;

	return texColor * color;
}