// Light pixel shader
// Calculate diffuse lighting for a single directional light(also texturing)

Texture2D meshTexture : register(t0);
SamplerState SampleType : register(s0);


cbuffer LightBuffer : register(b0)
{
    float4 ambientColor;
    float4 diffuseColor;
    float3 lightPosition;
    float specularPower;
    float4 specularColor;
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
    float3 viewDirection : TEXCOORD3;
};


float4 main(InputType input) : SV_TARGET
{
    float4	textureColor;
    float4	biome1TextureColor;
    float4	biome2TextureColor;
    float3	lightDir;
    float	lightIntensity;
    float4	color;
    float3 reflection;
    float4 specular;
    float specularPower = 32;
    // Invert the light direction for calculations.
    textureColor = meshTexture.Sample(SampleType, input.tex);
    lightDir = normalize(input.position3D - lightPosition);
 
    // Calculate the amount of light on this pixel.
    lightIntensity = saturate(dot(input.normal, -lightDir));

    // Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
    color = ambientColor + (diffuseColor * lightIntensity); //adding ambient
    color = saturate(color);

    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = meshTexture.Sample(SampleType, input.tex);
    reflection = normalize(2 * lightIntensity * input.normal - lightDir);

        // Determine the amount of specular light based on the reflection vector, viewing direction, and specular power.
    specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);
    color = color * textureColor;
    color = saturate(color+(specular/3));
    return color;


}

