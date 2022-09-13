#include "DefaultShaderInfo.hlsli"

// gameObjectなどのレイヤー
Texture2D<float4> defaultLayerTexture : register(t0);
// パーティクルなどのエフェクト用のレイヤー
Texture2D<float4> effectLayerTexture : register(t1);

// ブルーム用ブラーレイヤー
Texture2D<float4> blurLayerTexture1 : register(t2);
Texture2D<float4> blurLayerTexture2 : register(t3);
Texture2D<float4> blurLayerTexture3: register(t4);


float4 main(DefaultSpriteVSOutput input) : SV_TARGET
{
	float4 resultColor = float4(0,0,0,1);

	resultColor.rgb += defaultLayerTexture.Sample(clampLinearSampler, input.uv);
	resultColor.rgb += effectLayerTexture.Sample(clampLinearSampler, input.uv)/2;
	resultColor.rgb += blurLayerTexture1.Sample(clampLinearSampler, input.uv) / 2;
	resultColor.rgb += blurLayerTexture2.Sample(clampLinearSampler, input.uv) / 2;
	resultColor.rgb += blurLayerTexture3.Sample(clampLinearSampler, input.uv) / 2;

	return resultColor;
}