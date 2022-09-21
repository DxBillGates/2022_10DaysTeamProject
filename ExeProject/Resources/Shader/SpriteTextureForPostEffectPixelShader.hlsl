#include "DefaultShaderInfo.hlsli"

Texture2D<float4> tex : register(t0);

float4 main(DefaultSpriteVSOutput input) : SV_TARGET
{
	float4 texColor = tex.Sample(wrapPointSampler,input.uv);

	// ���ꂵ�Ȃ��Ɖ�ʃN���A�̐F�����������ɉf�����Ⴄ
	texColor.a = 1;

	return texColor * color;
}