#include "DefaultShaderInfo.hlsli"

Texture2D<float4> tex : register(t0);

float4 main(DefaultSpriteVSOutput input) : SV_TARGET
{
	float4 texColor = tex.Sample(wrapPointSampler,input.uv);

	// ‚±‚ê‚µ‚È‚¢‚Æ‰æ–ÊƒNƒŠƒA‚ÌF‚ª“§–¾•”•ª‚É‰f‚Á‚¿‚á‚¤
	texColor.a = 1;

	return texColor * color;
}