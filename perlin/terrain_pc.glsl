// Push constants shared by the terrain and tree shaders (TerrainPC in fly.cpp)
layout(push_constant) uniform PC {
    mat4 viewProj;
    vec4 eye;     // xyz camera position, w fog end
    vec4 sun;     // xyz direction to the sun, w fog start
    int step;     // heightmap pixels per grid cell
    int cells;    // grid cells per row
    float vscale; // world height per 8-bit level
    float water;  // water level, in 8-bit levels
    int rt;       // ray tracing: 0 off, 1 shadows, 2 + reflections, 3 + ambient occlusion
    int ao_dirs;  // ambient occlusion directions
    ivec2 tree_cell; // the generator's scroll in noise space: whole tree cells
    float tree_frac; // and the remaining pixels
    float trees;     // tree density, 0 = none
} pc;
