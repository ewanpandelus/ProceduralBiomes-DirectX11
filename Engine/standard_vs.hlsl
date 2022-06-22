// Light vertex shader
// Standard issue vertex shader, apply matrices, pass info to pixel shader

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    float time;
};

cbuffer CameraBuffer
{
    float3 cameraPosition;
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
    float3 position3D : TEXCOORD2;
    float3 viewDirection : TEXCOORD3;
};
OutputType main(InputType input)
{
    float4 worldPosition;
    float4 _wind_dir = float4(0.5, 0.05, 0.5, 0);
    float _wind_size = 45;
    float4 textureColor;
    float _tree_sway_stutter_influence = 0.2;
    float _tree_sway_stutter = 1.5;
    float _tree_sway_speed = 1;
    float _tree_sway_disp = 0.3;

    float _branches_disp = 0.3;

    float  _leaves_wiggle_disp = 0.07;
    float _leaves_wiggle_speed = 0.01;

    float _r_influence = 1;
    float  _b_influence = 1;


    OutputType output;

    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    worldPosition = mul(input.position, worldMatrix);

    float4 _Time = float4(time / 20, time, time * 2, time * 3);
    float4 worldPos = input.position;
    float4 vertex = input.position;
    vertex.x += 5 * (cos(_Time.z * _tree_sway_speed + (worldPos.x / _wind_size) + (sin(_Time.z * _tree_sway_stutter * _tree_sway_speed + (worldPos.x / _wind_size)) * _tree_sway_stutter_influence)) + 1) / 2 * _tree_sway_disp * _wind_dir.x * (vertex.y / 10) +
        cos(_Time.w * vertex.x * _leaves_wiggle_speed + (worldPos.x / _wind_size)) * _leaves_wiggle_disp * _wind_dir.x;

    vertex.z += 5 * (cos(_Time.z * _tree_sway_speed + (worldPos.z / _wind_size) + (sin(_Time.z * _tree_sway_stutter * _tree_sway_speed + (worldPos.z / _wind_size)) * _tree_sway_stutter_influence)) + 1) / 2 * _tree_sway_disp * _wind_dir.z * (vertex.y / 10) +
        cos(_Time.w * vertex.z * _leaves_wiggle_speed + (worldPos.x / _wind_size)) * _leaves_wiggle_disp * _wind_dir.z;


    output.position += vertex;
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

    // Calculate the normal vector against the world matrix only.
    output.normal = mul(input.normal, (float3x3)worldMatrix);

    // Normalize the normal vector.
    output.normal = normalize(output.normal);
    output.viewDirection = cameraPosition.xyz - worldPosition.xyz;

    // Normalize the viewing direction vector.
    output.viewDirection = normalize(output.viewDirection);
    // world position of vertex (for point light)
    output.position3D = (float3)mul(input.position, worldMatrix);

    return output;
}