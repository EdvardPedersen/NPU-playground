// Ray tracing against the heightmap and the trees, shared by terrain.frag and
// tree.frag. Needs terrain_pc.glsl, terrain_hmap.glsl and terrain_trees.glsl.

float fog(float d) { return smoothstep(pc.sun.w, pc.eye.w, d); }

bool outside(vec2 xz) { return any(lessThan(xz, vec2(0.0))) || any(greaterThan(xz, vec2(W * PX))); }

// Tree lookups along a ray, cached per cell
struct TreeCache {
    ivec2 cell;
    bool has;
    vec4 tree;
};

TreeCache no_tree() { return TreeCache(ivec2(0x7FFFFFFF), false, vec4(0.0)); }

float trees_sdf(vec3 q, inout TreeCache tc) {
    ivec2 c = tree_cell_of(q.xz);
    if (c != tc.cell) {
        tc.cell = c;
        tc.has = tree_at(c, tc.tree);
    }
    return tc.has ? tree_sdf(q, tc.tree) : 1e9;
}

// Near the ground, rays step at most half a cell (so no tree is skipped) and no
// further than the distance to the tree in the current cell
bool near_trees(float d) { return pc.trees > 0.0 && d < TREE_MAX_H + 0.5; }
float tree_step(float dt, float s) { return min(min(dt, max(s, 0.1)), 0.5 * TREE_CELL_W); }

// Soft shadow: march toward the sun, tracking how narrowly the ray clears the
// terrain and the trees. Rays end above the highest possible terrain or off the map.
float shadow(vec3 p) {
    vec3 l = pc.sun.xyz;
    float top = 255.0 * pc.vscale, res = 1.0, t = 0.5;
    p.y += 0.4; // the triangles and the bilinear heightmap differ slightly
    TreeCache tc = no_tree();
    for (int i = 0; i < 256; i++) {
        vec3 q = p + l * t;
        if (q.y > top || outside(q.xz)) break;
        float d = q.y - surface_y(q.xz);
        float dt = clamp(0.5 * d, 0.2, 4.0);
        if (near_trees(d)) {
            float s = trees_sdf(q, tc);
            d = min(d, s);
            dt = tree_step(dt, s);
        }
        res = min(res, 10.0 * d / t);
        if (res < 0.0) break;
        t += dt;
    }
    return smoothstep(0.0, 1.0, clamp(res, 0.0, 1.0));
}

// Fraction of the sky visible from p: marches outward in pc.ao_dirs directions,
// finding the highest horizon within ~16 units in each.
float ambient_occlusion(vec3 p) {
    float occluded = 0.0;
    for (int k = 0; k < pc.ao_dirs; k++) {
        float a = (float(k) + 0.5) * 6.2831853 / float(pc.ao_dirs);
        vec2 dir = vec2(cos(a), sin(a));
        float slope = 0.0;
        for (int i = 1; i <= 16; i++) {
            float r = 0.3 + 0.06 * float(i * i);
            slope = max(slope, (surface_y(p.xz + dir * r) - p.y) / r);
        }
        occluded += slope * inversesqrt(1.0 + slope * slope); // sine of the horizon angle
    }
    return 1.0 - occluded / float(pc.ao_dirs);
}

// Sky light blocked by the canopy above ground point p
float canopy_occlusion(vec3 p) {
    vec4 tree;
    if (pc.trees <= 0.0 || !tree_at(tree_cell_of(p.xz), tree)) return 1.0;
    float r = TREE_R * tree.w;
    return mix(0.45, 1.0, smoothstep(0.5 * r, 1.6 * r, length(p.xz - tree.xz)));
}

vec3 light(vec3 base, vec3 n, float sh, float ao) {
    return base * (0.3 * ao + 0.75 * max(dot(n, pc.sun.xyz), 0.0) * sh);
}

// Terrain seen at p, with the normal and colour taken from the heightmap
vec3 shade_hit(vec3 p) {
    const float e = 0.75;
    float hl = surface_y(p.xz - vec2(e, 0)), hr = surface_y(p.xz + vec2(e, 0));
    float hd = surface_y(p.xz - vec2(0, e)), hu = surface_y(p.xz + vec2(0, e));
    vec3 n = normalize(vec3(hl - hr, 2.0 * e, hd - hu));
    float lv = level_at(p.xz);
    return light(lv < pc.water ? water_color(lv) : land(lv, n.y), n, shadow(p), 1.0);
}

vec3 shade_tree_hit(vec3 p, vec4 tree) {
    vec3 n = tree_normal(p, tree);
    return light(tree_color(tree), n, shadow(p + 0.3 * n), 0.8);
}

vec3 sky(vec3 r) {
    return SKY + vec3(1.0, 0.9, 0.7) * pow(max(dot(r, pc.sun.xyz), 0.0), 400.0) * 4.0;
}

// The reflected ray from the water surface at p, fogged over its full path
vec3 reflection(vec3 p, vec3 r, float dist0) {
    float top = 255.0 * pc.vscale, t = 0.3, prev_t = 0.0, prev_d = 0.0;
    TreeCache tc = no_tree();
    for (int i = 0; i < 300; i++) {
        vec3 q = p + r * t;
        if (q.y > top || outside(q.xz)) break;
        float d = q.y - surface_y(q.xz);
        float dt = clamp(0.5 * d, 0.2, 6.0);
        if (near_trees(d)) {
            float s = trees_sdf(q, tc);
            if (s < 0.0) return mix(shade_tree_hit(q, tc.tree), SKY, fog(dist0 + t));
            dt = tree_step(dt, s);
        }
        if (d < 0.0) {
            t = prev_t + (t - prev_t) * prev_d / (prev_d - d);
            return mix(shade_hit(p + r * t), SKY, fog(dist0 + t));
        }
        prev_t = t;
        prev_d = d;
        t += dt;
    }
    return sky(r);
}
