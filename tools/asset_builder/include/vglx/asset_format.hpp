/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <cstdint>

#define VGLX_TEX_VER 1
#define VGLX_MSH_VER 2

enum TextureFormat : uint32_t {
    TextureFormat_RGBA8 = 0,
};

enum TextureColorSpace : uint32_t {
    TextureColorSpace_Linear = 0,
    TextureColorSpace_sRGB = 1,
};

enum VertexAttributeFlags : uint32_t {
    VertexAttr_None         = 0,
    VertexAttr_HasPosition  = 1 << 0,
    VertexAttr_HasNormal    = 1 << 1,
    VertexAttr_HasUV        = 1 << 2,
    VertexAttr_HasTangent   = 1 << 3,
    VertexAttr_HasColor     = 1 << 4,
};

enum MaterialTextureMapType : uint32_t {
    MaterialTextureMapType_Diffuse  = 0,
    MaterialTextureMapType_Alpha    = 1,
    MaterialTextureMapType_Normal   = 2,
    MaterialTextureMapType_Specular = 3,
};

#pragma pack(push, 1)
struct TextureHeader {
    char magic[4]; // "TEX0"
    uint32_t version;
    uint32_t header_size;
    uint32_t width;
    uint32_t height;
    uint32_t format; // TextureFormat
    uint32_t color_space; // TextureColorSpace
    uint32_t mip_levels;
    uint64_t pixel_data_size;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct MeshHeader {
    char magic[4] = {}; // "MSH0"
    uint32_t version;
    uint32_t header_size;
    uint32_t material_count;
    uint32_t mesh_count;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct MaterialTextureMapRecord {
    char filename[128] = {};
    uint32_t type; // MaterialTextureMapType
};
#pragma pack(pop)

#pragma pack(push, 1)
struct MaterialRecord {
    char name[64] = {};
    uint32_t texture_count;
    float ambient[3];
    float diffuse[3];
    float specular[3];
    float shininess;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct MeshRecord {
    char name[64] = {};
    uint32_t vertex_count;
    uint32_t index_count;
    uint32_t vertex_stride;
    uint32_t material_index;
    uint64_t vertex_data_size;
    uint64_t index_data_size;
    uint32_t vertex_flags; // VertexAttributeFlags
};
#pragma pack(pop)