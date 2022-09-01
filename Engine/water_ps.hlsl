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

float2 ClipSpaceToTextureCoords(float4 clipSpace) {
    float2 ndc = (clipSpace.xy / clipSpace.w);
    float2 texCoords = ndc / 2.0 + 0.5;
    return texCoords;
}
struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    nointerpolation float3 normal : NORMAL;
};


float4 main(InputType input) : SV_TARGET
{
    float4 textureColor;
    float2 texCoordsReal = ClipSpaceToTextureCoords(input.position);
    float2 refractionCoords = texCoordsReal;
    textureColor = texture0.Sample(SampleType, 1- (texCoordsReal/5));
    
    float3	lightDir;
    float	lightIntensity;
    float4	color;
    float3 reflection;
    float4 specular;
    float specularPower = 32;
    // Invert the light direction for calculations.

    lightDir = float3(0.5, -0.5, 0.5);
    lightIntensity = saturate(dot(input.normal, -lightDir));
    reflection = normalize(2 * lightIntensity * input.normal - lightDir);


    color = ambientColor + (diffuseColor * lightIntensity); //adding ambient
   // specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);

    //color = color * textureColor;
    color = saturate(color);


     color*=float4(0.2,0.4,1,0.5);
     ///color.a = 0.01;
     //return color;

    return color;
}