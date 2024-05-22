#version 330 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 Texcoord;
layout (location = 2) in vec3 Normal;

// Transformation matrix.
uniform mat4 worldMatrix;
uniform mat4 normalMatrix;
uniform mat4 MVP;

out vec3 position;
out vec2 texcoord;
out vec3 normal;

void main()
{
    gl_Position = MVP * vec4(Position, 1.0);

    // Pass vertex attributes.
    vec4 positionTmp = worldMatrix * vec4(Position, 1.0);
    position = positionTmp.xyz / positionTmp.w;
    texcoord = Texcoord;
    normal = (normalMatrix * vec4(Normal, 0.0)).xyz;
}