#version 450 core
#extension GL_ARB_separate_shader_objects : require
#extension GL_KHR_vulkan_glsl : enable

// Object Data
layout (set = 0, binding = 0) uniform UBOBaseObject {
    vec3    Position;
    mat4    Orientation;
} Object;

// Render Target Data
layout (set = 1, binding = 0) uniform UBOWindow {
    vec2    Size; // Meters
    uvec2   Resolution; // Pixels
} Window;

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec2 VertexTextureCoordinate;
layout (location = 2) in vec4 VertexColor;

layout (location = 0) out vec2 InterpolatedTextureCoordinate;
layout (location = 1) out vec4 InterpolatedColor;

void main() {
    vec4 v = Object.Orientation * vec4(VertexPosition, 1.0f);
    vec4 r = v + vec4(Object.Position, 1.0f);
    mat4 a = mat4(
        1.0f / Window.Size.x, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f / Window.Size.y, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    gl_Position = a * r;
    InterpolatedTextureCoordinate = VertexTextureCoordinate;
    InterpolatedColor = VertexColor;
}
