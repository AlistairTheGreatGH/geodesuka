#version 450 core
#extension GL_ARB_separate_shader_objects : require
#extension GL_KHR_vulkan_glsl : enable

#define MAX_BONE_COUNT 256

// Mesh Geometry Data
layout (location = 0) in vec3   VertexPosition;
layout (location = 1) in vec2   VertexNormal;
layout (location = 2) in vec4   VertexTangent;
layout (location = 3) in vec3   VertexBitangent;
// Mesh Texturing & Coloring Data
layout (location = 4) in vec3   VertexTextureCoordinate;
layout (location = 5) in vec3   VertexColor;
// Bone Data (Seperate Instance Buffer)
layout (location = 6) in uvec4  VertexBoneID;
layout (location = 7) in vec4   VertexBoneWeight;

// Transforms Vertices to Model Space.
layout (set = 0, binding = 0) uniform MeshUBO {
    mat4 Transform;
    // Static or Dynamic Bone Transforms For Animation
    mat4 BoneTransform[MAX_BONE_COUNT];
    // Transforms The Vertex From Local Space To Bone Space.
    mat4 OffsetTransform[MAX_BONE_COUNT];
} Mesh;

// Transforms Vertices to World Space.
layout (set = 0, binding = 1) uniform ObjectUBO {
    vec3 Position;
    mat4 Orientation;
} Object;


// Transforms Vertices to Camera Space.
layout (set = 0, binding = 2) uniform Camera3DUBO {
    mat4 Projection;
    mat4 Rotation;
    mat4 Translation;
} Camera3D;

// Mesh Per Vertex Data
layout (location = 0) out vec3 WorldPosition;
layout (location = 1) out vec3 WorldNormal;
layout (location = 2) out vec2 TextureCoordinate;

void main() {
    // Raw Vertex
    vec4 v = vec4(VertexPosition, 1.0);
    // Raw Normal
    vec4 n = vec4(VertexNormal, 1.0);

    mat4 mt = mat4(0.0f);
    if (VertexBoneID[0] < MAX_BONE_COUNT) {
        // Bone Transformations detected.
        for (int i = 0; i < 4; i++) {
            mt += Mesh.BoneTransform[VertexBoneID[i]] * Mesh.OffsetTransform[VertexBoneID[i]] * VertexBoneWeight[i];
        }
    } else {
        // No Bone Transformations detected.
        mt = Mesh.Transform;
    }

    // Transform to Model Space
    v = mt * v;
    n = mt * n;

    mat4 ct = mat4(
        1.0f, 0.0f, 0.0f, Object.Position.x,
        0.0f, 1.0f, 0.0f, Object.Position.y,
        0.0f, 0.0f, 1.0f, Object.Position.z,
        0.0f, 0.0f, 0.0f, 1.0f
    );
    // Transform to World Space
    v = ct * Object.Orientation * v;
    n = ct * Object.Orientation * n;

    // Carry data and interpolate to fragment shader. (Needed for Lighting & Shadows)
    WorldPosition   = v.xyz;
    WorldNormal     = n.xyz;

    // Interpolate Texture Coordinates.
    TextureCoordinate = VertexTextureCoordinate;

    // Transform to Camera Space for rasterization of primitive.
    gl_Position = Camera3D.Projection * Camera3D.Rotation * Camera3D.Translation * v;
}
