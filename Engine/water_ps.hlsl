
Texture2D depthTexture : register(t0);
SamplerState SampleType : register(s0);


cbuffer LightBuffer : register(b0)
{
    float4 ambientColor;
    float4 diffuseColor;
    float3 lightPosition;
    float maxDepth;
};


struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 position3D : TEXCOORD2;
};

float GetDepthPercentage(float depth) {
    return (depth / maxDepth);
}
float4 main(InputType input) : SV_TARGET
{
    float4 textureColor;
    float3	lightDir;
    float	lightIntensity;
    float4	color;
    textureColor = depthTexture.Sample(SampleType, input.tex);
    // Invert the light direction for calculations.
    lightDir = float3(0.5, -0.5, 0.5);// lightDir = normalize(input.position3D - lightPosition);

    // Calculate the amount of light on this pixel.
    lightIntensity = saturate(dot(input.normal, -lightDir));

    // Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
    color = ambientColor + (diffuseColor * lightIntensity); //adding ambient
    color = saturate(color)*float4(0.1,0.1,1,1);
    return color;


}