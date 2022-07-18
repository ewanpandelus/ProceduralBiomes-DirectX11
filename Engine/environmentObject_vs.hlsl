// Simple throughput vertex shader. Work being done by the geometry shader.

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 instancePosition : TEXCOORD1;

};

InputType main(InputType input)
{
    input.position.x += input.instancePosition.y;
    input.position.y += input.instancePosition.y;
    input.position.z += input.instancePosition.z;
    return input;
}