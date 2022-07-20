cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    float time;
};
struct OutputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};
float3 GerstnerWave( float4 wave, float3 p, inout float3 tangent, inout float3 binormal, float amp)
 {
    float steepness = wave.z;
    float wavelength = wave.w;
    float k = 2 * 3.14 / wavelength;
    float c = sqrt(9.8 / k);
    float2 d = normalize(wave.xy);
    float f = k * (dot(d, p.xz) - c * time/2);
    float a = steepness / k;

    //p.x += d.x * (a * cos(f));
    //p.y = a * sin(f);
    //p.z += d.y * (a * cos(f));

    tangent += float3( -d.x * d.x * (steepness * sin(f)),d.x * (steepness * cos(f)), -d.x * d.y * (steepness * sin(f)));
    binormal += float3(-d.x * d.y * (steepness * sin(f)),d.y * (steepness * cos(f)),-d.y * d.y * (steepness * sin(f)));
    return float3(d.x * (a * cos(f)),a * sin(f),d.y * (a * cos(f)))*amp;
}
OutputType main(InputType input)
{

    OutputType output;

    input.position.w = 1.0f;
    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    float amplitude = 0.5f;
    float wavelength = 4.f;
    float2 direction = float2(1, 1);
    float2 d = normalize(direction);
    float steepness = 0.5;


  /*  float3 p = output.position.xyz;
    float k = 3.14/wavelength;
    float c = sqrt(9.8 / k);
    float f = k * (dot(d, p.xz) - c * time);
    float a = steepness / k;
    p.x += d.x * (a * cos(f));
    p.y = a * sin(f)*amplitude;
    p.z += d.y * (a * cos(f));
    float3 tangent = float3( 1 - d.x * d.x * (steepness * sin(f)),d.x * (steepness * cos(f)),-d.x * d.y * (steepness * sin(f)) );
    float3 binormal = float3(-d.x * d.y * (steepness * sin(f)), d.y * (steepness * cos(f)), 1 - d.y * d.y * (steepness * sin(f)));
    float3 normal = normalize(cross(binormal, tangent));*/
    float4 waveA = float4(1, 0, 0.5, 10);
    float4 waveB = float4(0, 1, 0.25, 20);
    float4 waveC = float4(1, 1, 0.15, 10);




    float3 gridPoint = output.position.xyz;
    float3 tangent = float3(1, 0, 0);
    float3 binormal = float3(0, 0, 1);
    float3 p = gridPoint;
    p += GerstnerWave(waveA, gridPoint, tangent, binormal, amplitude);
    p += GerstnerWave(waveB, gridPoint, tangent, binormal, amplitude);
    p += GerstnerWave(waveC, gridPoint, tangent, binormal, amplitude);

    float3 normal = normalize(cross(binormal, tangent));
 



    output.position = float4(p,1);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

    // Calculate the normal vector against the world matrix only.
    output.normal = normal;// mul(input.normal, (float3x3)worldMatrix);

    // Normalize the normal vector.
   // output.normal = normalize(output.normal);

    // world position of vertex (for point light)


    return output;


}