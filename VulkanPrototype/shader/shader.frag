#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D textureSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTextureCoordinate;

layout(location = 0) out vec4 outColor;

void main()
{
    // outColor = texture(textureSampler, fragTextureCoordinate) * vec4(fragColor, 1.0);
    outColor = vec4(fragColor, 1.0);
}   