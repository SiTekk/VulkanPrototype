#version 460

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

struct GameObjectData {
    vec3 position;
};

layout(std140, binding = 2) readonly buffer GameObjectBuffer {
    GameObjectData gameObjectData[];
} gameObjectBuffer;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTextureCoordinate;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTextureCoordinate;

void main()
{
    vec3 globalPosition = inPosition + gameObjectBuffer.gameObjectData[gl_BaseInstance].position;
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(globalPosition, 1.0);
    
    // gl_Position = vec4(inPosition, 1.0);
    fragColor = inColor;
    fragTextureCoordinate = inTextureCoordinate;
}