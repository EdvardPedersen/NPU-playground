// Procedural conifers, shared by tree.comp (placement and culling), tree.vert and
// the ray tracing in terrain_rt.glsl. Needs terrain_pc.glsl and terrain_hmap.glsl.
//
// There is at most one tree per TREE_CELL x TREE_CELL pixel cell. Cells are numbered
// in noise space, offset by the generator's scroll (pc.tree_cell, pc.tree_frac), so
// the trees move with the terrain. A tree is vec4(base position, height).
const int TREE_CELL = 4;                        // heightmap pixels per cell
const float TREE_CELL_W = float(TREE_CELL) * PX; // world units per cell
const float TREE_MIN_H = 1.2, TREE_MAX_H = 2.2;
const float TREE_R = 0.22;                      // canopy radius / height
const float TREE_Y0 = 0.25;                     // canopy bottom / height

uint hash(uvec2 v) {
    v = v * 1664525u + 1013904223u;
    v.x += v.y * 1664525u;
    v.y += v.x * 1664525u;
    v ^= v >> 16u;
    v.x += v.y * 1664525u;
    v.y += v.x * 1664525u;
    v ^= v >> 16u;
    return v.x ^ v.y;
}

float u01(uint h) { return float(h >> 8) * (1.0 / 16777216.0); }

// Value noise over cells, for forests and clearings
float forest(ivec2 c) {
    vec2 q = vec2(c) / 8.0;
    ivec2 i = ivec2(floor(q));
    vec2 f = q - vec2(i);
    f = f * f * (3.0 - 2.0 * f);
    float a = u01(hash(uvec2(i))), b = u01(hash(uvec2(i + ivec2(1, 0))));
    float c0 = u01(hash(uvec2(i + ivec2(0, 1)))), d = u01(hash(uvec2(i + ivec2(1, 1))));
    return mix(mix(a, b, f.x), mix(c0, d, f.x), f.y);
}

ivec2 tree_cell_of(vec2 xz) { return ivec2(floor((xz / PX + pc.tree_frac) / float(TREE_CELL))) + pc.tree_cell; }

// The tree in cell c, if any: on land below the snow, not on steep slopes. The
// detail octaves scroll a little slower than the base one, so the terrain under a
// tree changes as it scrolls; trees shrink toward these limits instead of popping.
bool tree_at(ivec2 c, out vec4 tree) {
    tree = vec4(0.0);
    uint h = hash(uvec2(c));
    if (u01(h) >= pc.trees * smoothstep(0.15, 0.4, forest(c)))
        return false;
    float height = mix(TREE_MIN_H, TREE_MAX_H, u01(hash(uvec2(h, 1u))));
    float r = TREE_R * height;
    vec2 jitter = vec2(u01(hash(uvec2(h, 2u))), u01(hash(uvec2(h, 3u))));
    vec2 origin = (vec2(c - pc.tree_cell) * float(TREE_CELL) - pc.tree_frac) * PX;
    vec2 xz = origin + r + jitter * (TREE_CELL_W - 2.0 * r); // the canopy stays inside the cell
    if (any(lessThan(xz, vec2(2.0))) || any(greaterThan(xz, vec2(W * PX - 2.0))))
        return false;
    float lv = level_at(xz);
    ivec2 p = ivec2(xz / PX);
    vec2 g = vec2(level(p + ivec2(3, 0)) - level(p - ivec2(3, 0)), level(p + ivec2(0, 3)) - level(p - ivec2(0, 3)));
    float slope = length(g) * pc.vscale / (6.0 * PX);
    float k = smoothstep(pc.water + 4.0, pc.water + 10.0, lv) * (1.0 - smoothstep(170.0, 190.0, lv)) *
              (1.0 - smoothstep(0.6, 1.0, slope));
    if (k < 0.2)
        return false;
    tree = vec4(xz.x, lv * pc.vscale - 0.3, xz.y, height * k); // sunk a little: the triangles are averaged
    return true;
}

vec3 tree_color(vec4 tree) {
    float v = u01(hash(floatBitsToUint(tree.xz)));
    return mix(vec3(0.10, 0.30, 0.10), vec3(0.26, 0.40, 0.12), v);
}

// Approximate signed distance from q to the tree's canopy, a single cone
float tree_sdf(vec3 q, vec4 tree) {
    float h = tree.w, r = TREE_R * h, y0 = TREE_Y0 * h, y = q.y - tree.y;
    float rc = r * clamp((h - y) / (h - y0), 0.0, 1.0);
    float side = (length(q.xz - tree.xz) - rc) * (h - y0) * inversesqrt((h - y0) * (h - y0) + r * r);
    return max(side, max(y0 - y, y - h));
}

// Outward normal of the canopy cone at q
vec3 tree_normal(vec3 q, vec4 tree) {
    float h = tree.w;
    vec2 d = q.xz - tree.xz;
    d = dot(d, d) > 1e-6 ? normalize(d) : vec2(1.0, 0.0);
    return normalize(vec3(d * (h - TREE_Y0 * h), TREE_R * h));
}
