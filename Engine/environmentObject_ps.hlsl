// Simple colour pixel shader

Texture2D texture0 : register(t0);
SamplerState SampleType : register(s0);

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	float time;
};
cbuffer LightBuffer : register(b1)
{
    float4 ambientColor;
    float4 diffuseColor;
    float3 lightPosition;
    float padding;
};


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

    float3	lightDir;
    float	lightIntensity;
    float4	color;


    float3 position3D = (float3)mul(input.position, worldMatrix);
    // Invert the light direction for calculations.

    lightDir = float3(0.5, -0.5, 0.5);// normalize(position3D - lightPosition);

    // Calculate the amount of light on this pixel.
    lightIntensity = saturate(dot(input.normal, -lightDir));

    // Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
    color = ambientColor + (diffuseColor * lightIntensity); //adding ambient
    color = saturate(color);



    color = color * textureColor;
 

    return color;
}