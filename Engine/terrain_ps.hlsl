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
cbuffer ColourBuffer : register(b1)
{
    float4 desertColour;
    float4 forestColour;
    float3 snowColour;
    float excess;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    nointerpolation  float3 normal : NORMAL;
    float3 position3D : TEXCOORD2;
};


float4 main(InputType input) : SV_TARGET
{
    float4	textureColor;

    float4	desertTex1;
    float4	desertTex2;
    float4	forestTex;

    float4 noiseTex;


    float4	snowColor;

    float4 desertCol1 = float4(0.886, 0.733, 0.007, 1);
    float4 desertCol2 = float4(0.941, 0.965, 0.176,1);
    float4 desertCol;



    float4 forestCol1 = float4(0, 0.69, 0.35, 1);
    float4 forestCol2 = float4(0, 0.769, 0.024, 1);
    float4 forestCol;


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
    desertTex1 = desertTexture.Sample(SampleType, input.tex*5);
    forestTex = forestTexture.Sample(SampleType, input.tex*5);

    desertTex2 = desert2Texture.Sample(SampleType, input.tex);
    noiseTex = noiseTexture.Sample(SampleType, input.tex / 5);




    desertCol = lerp(desertCol1, desertCol2, noiseTex.r);
    forestCol = lerp(forestCol1, forestCol2, noiseTex.r);
    desertCol = lerp(desertCol, desertTex1, 0.2);
    forestCol = lerp(forestCol, forestTex, 0.2);

;
//forestColor  = lerp(desertColor, float4(0, 1, 0, 1), .4);

snowColor = snowTexture.Sample(SampleType, input.tex);



desertCol = textureColor.r * desertCol;
forestCol = textureColor.g * forestCol;
snowColor = textureColor.b * snowColor;
return color * saturate(desertCol + forestCol + snowColor);


}