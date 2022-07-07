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
//float3 CalcTriangleNormal(InputType input[3]) {
//    float3 tangent1 = input[1].position.xyz - input[0].position.xyz;
//    float3 tangent2 = input[2].position.xyz - input[0].position.xyz;
//    float3 normal = cross(tangent1, tangent2);
//    return normalize(normal);
//}
//[maxvertexcount(3)]
//void main(triangle InputType input[3], inout TriangleStream<OutputType> triStream)
//{
//    OutputType output;
//
//   // float3 triNormal = CalcTriangleNormal(input);
//    for (int i = 0; i < 3; i++) {
//        output.position = input[i].position;
//        output.position = mul(output.position, worldMatrix);
//        output.position = mul(output.position, viewMatrix);
//        output.position = mul(output.position, projectionMatrix);
//        output.tex = input[i].tex;
//        output.normal = float3(0,0,0);
//        triStream.Append(output);
//    }
//    triStream.RestartStrip();
//}
[maxvertexcount(3)]
void main(triangle InputType input[3], inout TriangleStream<OutputType> triStream)
{
    OutputType output;

    output.position = input[0].position;
    // output.instancePosition = input[0].instancePosition;
    output.position = mul(output.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    output.tex = input[0].tex;
    output.normal = mul(input[0].normal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);
    triStream.Append(output);


    output.position = input[1].position;
    //output.instancePosition = input[1].instancePosition;

    output.position = mul(output.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    output.tex = input[1].tex;
    output.normal = mul(input[1].normal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);
    triStream.Append(output);





    // Move the vertex away from the point position
    output.position = input[2].position;
    // output.instancePosition = input[2].instancePosition;

    output.position = mul(output.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    output.tex = input[2].tex;
    output.normal = mul(input[2].normal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);
    triStream.Append(output);

    triStream.RestartStrip();

    // Move the vertex away from the point position




    // Move the vertex away from the point position





}