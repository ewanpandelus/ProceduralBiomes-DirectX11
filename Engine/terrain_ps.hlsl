// Light pixel shader
// Calculate diffuse lighting for a single directional light(also texturing)

Texture2D biomesTexture : register(t0);
Texture2D desertTexture : register(t1);
Texture2D forestTexture : register(t2);
Texture2D snowTexture : register(t3);
Texture2D desert2Texture : register(t4);
Texture2D noiseTexture : register(t5);

SamplerState SampleType : register(s0);


cbuffer LightBuffer : register(b0)
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
    float3 position3D : TEXCOORD2;
};
int FastFloor(double x)
{
    return x > 0 ? (int)x : (int)x - 1;
}
double Dot(int g[3], double x, double y, double z)
{
    return g[0] * x + g[1] * y + g[2] * z;
}
double Mix(double a, double b, double t)
{
    return (1 - t) * a + t * b;
}
double Fade(double t)
{
    return t * t * t * (t * (t * 6 - 15) + 10);
}
double Dot(int g[3], double x, double y) {
    return g[0] * x + g[1] * y;
}

float4 main(InputType input) : SV_TARGET
{
    float4	textureColor;
    float4	desertColor;
    float4	desertTex1;
    float4	desertTex2;
    float4 noiseTex;

    float4	forestColor;
    float4	snowColor;



    float desertValue;
    float forestValue;
    float3	lightDir;
    float	lightIntensity;
    float4	color;

    // Invert the light direction for calculations.
    lightDir = normalize(input.position3D - lightPosition);

    // Calculate the amount of light on this pixel.
    lightIntensity = saturate(dot(input.normal, -lightDir));

    // Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
    color = ambientColor + (diffuseColor * lightIntensity); //adding ambient
    color = saturate(color);

    // Sample the pixel color from the texture using the sampler at this texture coordinate location.

    textureColor = biomesTexture.Sample(SampleType, input.tex / 5);
    desertTex1 = desertTexture.Sample(SampleType, input.tex*2);
    desertTex2 = desert2Texture.Sample(SampleType, input.tex * 2);
    noiseTex = noiseTexture.Sample(SampleType, input.tex / 5);


     

 //   desertColor = lerp(desertColor, float4(1, 0.8, 0.5, 1), .4);



    forestColor = forestTexture.Sample(SampleType, input.tex);
  //forestColor  = lerp(desertColor, float4(0, 1, 0, 1), .4);

    snowColor = snowTexture.Sample(SampleType, input.tex);
   // snowColor = lerp(snowColor, float4(1, 1, 1, 1), .4);
    desertColor = lerp(desertTex1, snowColor, noiseTex.r);

    desertColor = textureColor.r * desertColor;
    forestColor = textureColor.g * forestColor;
    snowColor = textureColor.b * snowColor;







    return saturate(desertColor + forestColor+snowColor);


}

