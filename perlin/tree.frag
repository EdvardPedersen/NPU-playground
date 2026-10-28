#version 450
#extension GL_GOOGLE_include_directive : require
// Trees, lit like the terrain: ray-traced soft shadows from the terrain and the
// other trees when pc.rt >= 1.
#include "terrain_pc.glsl"
#include "terrain_hmap.glsl"
#include "terrain_trees.glsl"
#include "terrain_rt.glsl"

layout(location = 0) flat in vec3 albedo;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 world;
layout(location = 3) in float ao;
layout(location = 0) out vec4 outColor;

void main() {
    vec3 n = normalize(normal);
    float sh = pc.rt >= 1 ? shadow(world + 0.15 * n) : 1.0;
    outColor = vec4(mix(light(albedo, n, sh, ao), SKY, fog(length(world - pc.eye.xyz))), 1.0);
}
