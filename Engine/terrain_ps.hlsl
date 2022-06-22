// Light pixel shader
// Calculate diffuse lighting for a single directional light(also texturing)

Texture2D noiseTexture : register(t0);
Texture2D biome1Texture : register(t1);
Texture2D biome2Texture : register(t2);
SamplerState SampleType : register(s0);


cbuffer LightBuffer : register(b0)
{
	float4 ambientColor;
    float4 diffuseColor;
    float3 lightPosition;
    float padding;
};
cbuffer NoiseMapBuffer : register (b1)
{
    float frequency;
    float amplitude;
    float offset;
    float excess1;
}
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
    float4	biome1TextureColor;
    float4	biome2TextureColor;

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
	textureColor = noiseTexture.Sample(SampleType, input.tex/5);

    float depthValue = textureColor.z / textureColor.w;
    biome1TextureColor = lerp(float4(0.2, 0.8, 0.2,1), biome1Texture.Sample(SampleType, input.tex),0.2);
    biome2TextureColor = lerp(float4(1, 1, 0.2, 1), biome2Texture.Sample(SampleType, input.tex), 0.2);
    float2 pos = float2((input.position3D.z * frequency) + offset, (input.position3D.x * frequency) + offset);
    //float noise = snoise(pos)*amplitude;
    float noise = textureColor;
    float4 noiseMap = float4(lerp(float3(0, 0, 1), float3(1, 0, 0), noise),1);
    float4 tempMap = lerp(biome1TextureColor, biome2TextureColor, noise);
    return color * tempMap;

   
}

