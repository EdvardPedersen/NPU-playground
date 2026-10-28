// Heightmap access shared by terrain.geom and terrain.frag; needs terrain_pc.glsl
layout(std430, binding = 0) readonly buffer Hmap { uint px[]; };

const int W = 4096;          // PERLIN_W
const float PX = 1024.0 / W; // world units per heightmap pixel
const vec3 SKY = vec3(0.62, 0.75, 0.90);

float level(ivec2 p) {
    p = clamp(p, ivec2(0), ivec2(W - 1));
    return float(px[p.y * W + p.x] & 255u);
}

// Bilinear 8-bit level at world position xz; pixel p is centred on p * PX
float level_at(vec2 xz) {
    vec2 q = xz / PX;
    ivec2 i = ivec2(floor(q));
    vec2 f = q - vec2(i);
    return mix(mix(level(i), level(i + ivec2(1, 0)), f.x),
               mix(level(i + ivec2(0, 1)), level(i + ivec2(1, 1)), f.x), f.y);
}

// World height of the terrain or water surface
float surface_y(vec2 xz) { return max(level_at(xz), pc.water) * pc.vscale; }

vec3 land(float lv, float up) {
    float above = lv - pc.water;
    vec3 c = mix(vec3(0.30, 0.52, 0.20), vec3(0.20, 0.38, 0.16), clamp(above / 60.0, 0.0, 1.0));
    c = mix(vec3(0.82, 0.76, 0.55), c, smoothstep(2.0, 6.0, above));             // beach
    c = mix(vec3(0.45, 0.42, 0.40), c, smoothstep(0.62, 0.78, up));              // rock on steep slopes
    c = mix(c, vec3(0.95, 0.96, 1.0), smoothstep(190.0, 205.0, lv) * smoothstep(0.55, 0.7, up)); // snow
    return c;
}

vec3 water_color(float lv) {
    return mix(vec3(0.22, 0.50, 0.62), vec3(0.06, 0.18, 0.38), clamp((pc.water - lv) / 40.0, 0.0, 1.0));
}
