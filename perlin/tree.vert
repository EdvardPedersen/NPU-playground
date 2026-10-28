#version 450
#extension GL_GOOGLE_include_directive : require
// One instance per visible tree (from tree.comp): a square trunk and three stacked
// cones, built from the vertex index (TREE_VERTS in fly.cpp).
#include "terrain_pc.glsl"
#include "terrain_hmap.glsl"
#include "terrain_trees.glsl"

layout(std430, binding = 1) readonly buffer Trees { vec4 trees[]; };

layout(location = 0) flat out vec3 albedo;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec3 world;
layout(location = 3) out float ao;

const int SIDES = 8;
const float TAU = 6.2831853;

void main() {
    vec4 tree = trees[gl_InstanceIndex];
    float h = tree.w, spin = u01(hash(floatBitsToUint(tree.xz))) * TAU;
    int v = gl_VertexIndex;
    vec3 p, n;
    if (v < 24) { // trunk: 4 quads
        int k = v % 6;
        float a = (float(v / 6) + float(k == 1 || k == 2 || k == 4)) * TAU / 4.0 + spin;
        float an = (float(v / 6) + 0.5) * TAU / 4.0 + spin;
        p = vec3(0.06 * h * cos(a), float(k == 2 || k == 4 || k == 5) * 0.35 * h, 0.06 * h * sin(a));
        n = vec3(cos(an), 0.0, sin(an));
        albedo = vec3(0.28, 0.18, 0.10);
    } else { // canopy: 3 cones of SIDES triangles
        int u = v - 24, tier = u / (3 * SIDES), tri = (u / 3) % SIDES, corner = u % 3;
        float y0 = h * (TREE_Y0 + 0.2 * float(tier)), y1 = h * (0.65 + 0.175 * float(tier));
        float r = TREE_R * h * (1.0 - 0.25 * float(tier));
        float a = (float(tri) + (corner == 2 ? 0.5 : float(corner))) * TAU / float(SIDES) + spin;
        p = corner == 2 ? vec3(0.0, y1, 0.0) : vec3(r * cos(a), y0, r * sin(a));
        n = normalize(vec3(cos(a) * (y1 - y0), r, sin(a) * (y1 - y0)));
        albedo = tree_color(tree) * (0.85 + 0.15 * float(tier));
    }
    normal = n;
    ao = 0.4 + 0.6 * p.y / h;
    world = tree.xyz + p;
    gl_Position = pc.viewProj * vec4(world, 1.0);
}
