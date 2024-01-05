#version 450 core
#extension GL_ARB_separate_shader_objects : require
#extension GL_KHR_vulkan_glsl : enable

// Mesh Per Vertex Data
layout (location = 0) in vec3 WorldPosition;
layout (location = 1) in vec3 WorldNormal;
layout (location = 2) in vec2 UV;

layout (set = 1, binding = 0) uniform sampler2D SurfaceColor;
layout (set = 1, binding = 1) uniform sampler2D SurfaceNormal;
layout (set = 1, binding = 2) uniform sampler2D SurfaceHeightMap;
layout (set = 1, binding = 3) uniform sampler2D SurfaceMetallic;
layout (set = 1, binding = 4) uniform sampler2D SurfaceRoughness;
layout (set = 1, binding = 5) uniform sampler2D SurfaceAmbientOcclusion;

// Geometry buffer's output
layout (location = 0) out vec4 PixelColor;
layout (location = 1) out vec4 PixelPosition;
layout (location = 2) out vec4 PixelNormal;
layout (location = 3) out vec4 PixelSpecular;

void main() {
    // Copy color over, no need for modification.
    PixelColor      = texture(TextureColor, UV);

    // Positions and normals will be modified by Normal maps and Height maps.
    PixelPosition   = vec4(WorldPosition, 1.0f);
    PixelNormal     = vec4(WorldNormal + texture(TextureNormal, UV).xyz, 1.0f);
}
