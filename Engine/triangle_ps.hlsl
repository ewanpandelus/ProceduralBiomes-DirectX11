// Simple colour pixel shader

Texture2D texture0 : register(t0);
SamplerState SampleType : register(s0);

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};


float4 main(InputType input) : SV_TARGET
{
	float4 textureColor;
	textureColor = texture0.Sample(SampleType, input.tex);
	return textureColor;
}