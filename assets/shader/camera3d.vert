#version 450 core
#extension GL_ARB_separate_shader_objects : require
#extension GL_KHR_vulkan_glsl : enable

#define MAX_BONE_COUNT 256

// Mesh Geometry Data
layout (location = 0) in vec3   VertexPosition;
layout (location = 1) in vec2   VertexNormal;
layout (location = 2) in vec4   VertexTangent;
layout (location = 3) in vec3   VertexBitangent;
layout (location = 4) in uvec4  VertexBoneID;
layout (location = 5) in vec4   VertexBoneWeight;

// Mesh Texturing & Coloring Data
layout (location = 6) in vec3   VertexTextureCoordinate[8];
layout (location = 7) in vec3   VertexColor[8];

// Object Data
layout (set = 0, binding = 0) uniform ObjectUBO {
    vec3 Position;
    mat4 Orientation;
} Object;

// Per node 
layout (set = 0, binding = 1) uniform NodeUBO {
    // Base Transformation
    mat4 Transform;
    // Used For Node Animations
    vec3 Translation;
    mat4 Rotation;
    vec3 Scaling;
} Node;

// Per mesh data, including vertex transformations.
layout (set = 0, binding = 2) uniform MeshUBO {
    mat4 BoneTransform[MAX_BONE_COUNT];
} Mesh;

// Camera Data
layout (set = 0, binding = 3) uniform Camera3DUBO {
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
    //
    bool MeshDeformationDetected = false;

    // Zero Matrix
    mat4 AggregateVertexTransform = mat4(0.0f);

    // Check for Mesh Deformations
    for (int i = 0; i < 4; i++) {
        if (VertexBoneID[i] >= 0) {
            MeshDeformationDetected = true;
            break;
        }
    }

    if (MeshDeformationDetected){
        // Apply Mesh Deformations to vertices.
        for (int i = 0; i < 4; i++) {
            if ((VertexBoneID[i] >= 0) && (VertexBoneID[i] < MAX_BONE_COUNT)) {
                AggregateVertexTransform += Mesh.BoneTransform[VertexBoneID[i]] * VertexBoneWeight[i];
            }
        }
    }
    else {
        // No mesh deformation detected, identity matrix.
        AggregateVertexTransform = mat4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    // Bone Transformations.
    n = AggregateVertexTransform * n;
    // Node Mesh Instance Transform
    n = Node.Transform * n;
    // Transforms to world space.
    n = Object.Translation * Object.Rotation * n;
    // Gather interpolatd normal vectors over geometric primitive
    WorldNormal = n.xyz;

    // Bone Transformations.
    v = AggregateVertexTransform * v;
    // Mesh Transformations to object space.
    v = Node.Transform * v;
    // Transform To World Space.
    v = Object.Translation * Object.Rotation * v;
    // Gather interpolated surface position for lighting and shadows.
    WorldPosition = v.xyz;

    // Interpolate Texture Coordinates.
    TextureCoordinate = VertexTextureCoordinate;

    // Transform to Camera View.
    gl_Position = Camera3D.Projection * Camera3D.Rotation * Camera3D.Translation * v;
}
