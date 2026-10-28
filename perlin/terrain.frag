#version 450
#extension GL_GOOGLE_include_directive : require
// Per-pixel lighting with ray tracing against the heightmap and the trees (pc.rt):
// 1 = soft sun shadows, 2 = also water reflections, which are themselves lit and
// shadowed, 3 = also ambient occlusion from the horizon in pc.ao_dirs directions.
#include "terrain_pc.glsl"
#include "terrain_hmap.glsl"
#include "terrain_trees.glsl"
#include "terrain_rt.glsl"

layout(location = 0) flat in vec3 albedo;
layout(location = 1) flat in vec3 normal;
layout(location = 2) flat in int water;
layout(location = 3) in vec3 world;
layout(location = 0) out vec4 outColor;

void main() {
    vec3 view = world - pc.eye.xyz;
    float dist = length(view);
    view /= dist;
    float sh = pc.rt >= 1 ? shadow(world) : 1.0;
    float ao = water != 0 ? 1.0 : canopy_occlusion(world) * (pc.rt >= 3 ? ambient_occlusion(world) : 1.0);
    vec3 c = light(albedo, normal, sh, ao);
    if (water != 0 && pc.rt >= 2) {
        vec3 r = reflect(view, vec3(0, 1, 0));
        float fresnel = 0.05 + 0.95 * pow(1.0 - max(r.y, 0.0), 5.0);
        c = mix(c, reflection(world, r, dist), fresnel);
    }
    outColor = vec4(mix(c, SKY, fog(dist)), 1.0);
}
