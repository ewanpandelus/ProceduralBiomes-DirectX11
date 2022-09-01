// triangle_gs
// Geometry shader that generates a triangle for every vertex.

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    float padding;
};

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
  
};

struct OutputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};
float3 CalcTriangleNormal(InputType input[3]) {
    float3 tangent1 = input[1].position.xyz - input[0].position.xyz;
    float3 tangent2 = input[2].position.xyz - input[0].position.xyz;
    float3 normal = cross(tangent1, tangent2);
    return normalize(normal);
}
[maxvertexcount(3)]
void main(triangle InputType input[3], inout TriangleStream<OutputType> triStream)
{
    OutputType output;

    float3 triNormal = CalcTriangleNormal(input);
    for (int i = 0; i < 3; i++) {
        output.position = input[i].position;
        output.tex = input[i].tex;
        output.normal = input[i].normal;
        triStream.Append(output);
    }
    triStream.RestartStrip();
}