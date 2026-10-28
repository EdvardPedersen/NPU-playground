#version 450
#extension GL_GOOGLE_include_directive : require
// Expands each point (one per grid cell) into two flat-shaded triangles of the
// landscape, using the heightmap the Perlin generator wrote (8-bit grey pixels).
// Lighting is done per pixel in terrain.frag.
layout(points) in;
layout(triangle_strip, max_vertices = 6) out;

#include "terrain_pc.glsl"
#include "terrain_hmap.glsl"

layout(location = 0) flat out vec3 albedo;
layout(location = 1) flat out vec3 normal;
layout(location = 2) flat out int water;
layout(location = 3) out vec3 world;

// Mean of the step x step pixel block at grid vertex v; also smooths the 8-bit steps
float height(ivec2 v) {
    ivec2 p = v * pc.step;
    float s = 0.0;
    for (int y = 0; y < pc.step; y++)
        for (int x = 0; x < pc.step; x++)
            s += level(p + ivec2(x, y));
    return s / float(pc.step * pc.step);
}

void tri(vec3 a, vec3 b, vec3 c, vec3 levels) {
    vec3 n = normalize(cross(b - a, c - a));
    n *= sign(n.y);
    float lv = (levels.x + levels.y + levels.z) / 3.0;
    bool wet = max(levels.x, max(levels.y, levels.z)) < pc.water;
    vec3 v[3] = vec3[](a, b, c);
    for (int i = 0; i < 3; i++) {
        albedo = wet ? water_color(lv) : land(lv, n.y);
        normal = wet ? vec3(0, 1, 0) : n;
        water = int(wet);
        world = v[i];
        gl_Position = pc.viewProj * vec4(v[i], 1.0);
        EmitVertex();
    }
    EndPrimitive();
}

// True if all four corners are outside the same clip plane
bool culled(vec4 c[4]) {
    bool px = true, nx = true, py = true, ny = true, nz = true, fz = true;
    for (int i = 0; i < 4; i++) {
        px = px && c[i].x > c[i].w;
        nx = nx && c[i].x < -c[i].w;
        py = py && c[i].y > c[i].w;
        ny = ny && c[i].y < -c[i].w;
        nz = nz && c[i].z < 0.0;
        fz = fz && c[i].z > c[i].w;
    }
    return px || nx || py || ny || nz || fz;
}

void main() {
    ivec2 cell = ivec2(gl_PrimitiveIDIn % pc.cells, gl_PrimitiveIDIn / pc.cells);
    vec3 p[4];
    float l[4];
    vec4 clip[4];
    for (int i = 0; i < 4; i++) {
        ivec2 v = cell + ivec2(i & 1, i >> 1);
        l[i] = height(v);
        // at the centre of the averaged block, to match the heightmap the fragment shader traces
        vec2 xz = (vec2(v * pc.step) + 0.5 * float(pc.step - 1)) * PX;
        p[i] = vec3(xz.x, max(l[i], pc.water) * pc.vscale, xz.y);
        clip[i] = pc.viewProj * vec4(p[i], 1.0);
    }
    if (culled(clip))
        return;
    tri(p[0], p[1], p[3], vec3(l[0], l[1], l[3]));
    tri(p[0], p[3], p[2], vec3(l[0], l[3], l[2]));
}
