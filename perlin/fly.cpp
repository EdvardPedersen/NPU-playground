// Flight over a Perlin-noise landscape. The heightmap is generated every frame by
// the NPU (kernel.cpp) or the GPU (perlin.comp), see perlin_params.h, and a
// geometry shader (terrain.geom) turns it into flat-shaded triangles. Advancing
// the noise time scrolls the terrain along the diagonal, which the camera faces.
// Procedural trees (terrain_trees.glsl) are culled by a compute shader (tree.comp)
// into an indirect instanced draw, and are part of the ray-traced shadows and
// reflections.
//
// Keys: Tab = NPU/GPU generator, +/- = octaves, Up/Down = speed, 1/2/4/8 = grid
// step, R = ray tracing (off, shadows, + water reflections, + ambient occlusion),
// [/] = halve/double the ambient occlusion directions (GPU load), T = tree density
// (1, 1/2, 1/4, none), Esc/Q = quit.
// Options: --npu (start on the NPU), --octaves N, --rt 0-3, --ao DIRS, --trees D
// (density, 0-1), --time T (start position), --screenshot file.ppm (save a frame
// after 3 s and exit).

#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <utility>
#include <vector>

#include "xrt/xrt_bo.h"
#include "xrt/xrt_device.h"
#include "xrt/xrt_kernel.h"
#include "xrt/experimental/xrt_elf.h"
#include "xrt/experimental/xrt_ext.h"
#include "xrt/experimental/xrt_module.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>

#include "perlin_params.h"

#define VK_CHECK(x)                                                                        \
    do {                                                                                   \
        VkResult r_ = (x);                                                                 \
        if (r_ != VK_SUCCESS) {                                                            \
            fprintf(stderr, "Vulkan error %d at %s:%d\n", r_, __FILE__, __LINE__);         \
            abort();                                                                       \
        }                                                                                  \
    } while (0)

constexpr VkDeviceSize HMAP_BYTES = PERLIN_W * PERLIN_H * 4;
constexpr VkFormat DEPTH_FORMAT = VK_FORMAT_D32_SFLOAT;

// Scene, in world units: the heightmap spans 1024 of them at any resolution
constexpr float PX = 1024.0f / PERLIN_W; // world units per heightmap pixel (terrain.geom)
constexpr float CAM_POS = 120.0f;   // camera position on the diagonal
constexpr float CLEARANCE = 22.0f;  // camera height above the terrain ahead
constexpr float FOG_START = 250.0f, FOG_END = 950.0f;
constexpr float VSCALE = 0.5f;      // world height per 8-bit level
constexpr float WATER = 100.0f;     // water level, in 8-bit levels
constexpr int TREE_CELL = 4;        // heightmap pixels per tree cell (terrain_trees.glsl)
constexpr int TREE_GRID = PERLIN_W / TREE_CELL + 2; // cells per row that can touch the map
constexpr uint32_t TREE_VERTS = 24 + 3 * 8 * 3;     // trunk + 3 cones (tree.vert)

struct TerrainPC { // terrain_pc.glsl
    float viewProj[16];
    float eye[4];
    float sun[4];
    int32_t step, cells;
    float vscale, water;
    int32_t rt, ao_dirs;
    int32_t tree_cell[2];
    float tree_frac, trees;
};

// ---- column-major 4x4 matrices, Vulkan clip space ---------------------------
struct V3 {
    float x, y, z;
};
static V3 operator-(V3 a, V3 b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
static V3 cross(V3 a, V3 b) { return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x}; }
static float dot(V3 a, V3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
static V3 normalize(V3 a) {
    float l = sqrtf(dot(a, a));
    return {a.x / l, a.y / l, a.z / l};
}

static void mat_mul(float *r, const float *a, const float *b) {
    for (int c = 0; c < 4; c++)
        for (int i = 0; i < 4; i++) {
            r[c * 4 + i] = 0;
            for (int k = 0; k < 4; k++) r[c * 4 + i] += a[k * 4 + i] * b[c * 4 + k];
        }
}

static void perspective(float *m, float fovy, float aspect, float n, float f) {
    float t = 1.0f / tanf(fovy / 2);
    memset(m, 0, 16 * sizeof(float));
    m[0] = t / aspect;
    m[5] = -t; // Vulkan clip y points down
    m[10] = f / (n - f);
    m[11] = -1;
    m[14] = n * f / (n - f);
}

static void look_at(float *m, V3 eye, V3 at, V3 up) {
    V3 f = normalize(at - eye), s = normalize(cross(f, up)), u = cross(s, f);
    float r[16] = {s.x, u.x, -f.x, 0, s.y, u.y, -f.y, 0, s.z, u.z, -f.z, 0,
                   -dot(s, eye), -dot(u, eye), dot(f, eye), 1};
    memcpy(m, r, sizeof(r));
}

// ---- Vulkan helpers ---------------------------------------------------------
static VkShaderModule load_shader(VkDevice dev, const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) { fprintf(stderr, "cannot open %s\n", path); exit(1); }
    std::vector<char> code;
    char buf[4096];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), f)) > 0) code.insert(code.end(), buf, buf + n);
    fclose(f);
    VkShaderModuleCreateInfo ci{VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
    ci.codeSize = code.size();
    ci.pCode = (const uint32_t *)code.data();
    VkShaderModule m;
    VK_CHECK(vkCreateShaderModule(dev, &ci, nullptr, &m));
    return m;
}

static uint32_t find_mem_type(VkPhysicalDevice pd, uint32_t bits, VkMemoryPropertyFlags props) {
    VkPhysicalDeviceMemoryProperties mp;
    vkGetPhysicalDeviceMemoryProperties(pd, &mp);
    for (uint32_t i = 0; i < mp.memoryTypeCount; i++)
        if ((bits >> i & 1) && (mp.memoryTypes[i].propertyFlags & props) == props) return i;
    fprintf(stderr, "no suitable memory type\n");
    exit(1);
}

struct Buffer {
    VkBuffer buf;
    VkDeviceMemory mem;
    void *mapped;
};

static Buffer host_buffer(VkPhysicalDevice pd, VkDevice dev, VkDeviceSize size, VkBufferUsageFlags usage,
                          VkMemoryPropertyFlags extra = 0) {
    Buffer b{};
    VkBufferCreateInfo bi{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    bi.size = size;
    bi.usage = usage;
    VK_CHECK(vkCreateBuffer(dev, &bi, nullptr, &b.buf));
    VkMemoryRequirements req;
    vkGetBufferMemoryRequirements(dev, b.buf, &req);
    VkMemoryAllocateInfo ai{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    ai.allocationSize = req.size;
    ai.memoryTypeIndex = find_mem_type(pd, req.memoryTypeBits,
                                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | extra);
    VK_CHECK(vkAllocateMemory(dev, &ai, nullptr, &b.mem));
    VK_CHECK(vkBindBufferMemory(dev, b.buf, b.mem, 0));
    VK_CHECK(vkMapMemory(dev, b.mem, 0, size, 0, &b.mapped));
    return b;
}

// Imports a dma-buf (here the NPU's output buffer) as a transfer source, so the GPU
// can copy what the NPU wrote without a CPU copy. Vulkan takes ownership of the fd.
static Buffer import_dmabuf(VkDevice dev, int fd, VkDeviceSize size) {
    constexpr auto type = VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT;
    Buffer b{};
    VkExternalMemoryBufferCreateInfo ext{VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_BUFFER_CREATE_INFO};
    ext.handleTypes = type;
    VkBufferCreateInfo bi{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    bi.pNext = &ext;
    bi.size = size;
    bi.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    VK_CHECK(vkCreateBuffer(dev, &bi, nullptr, &b.buf));
    VkMemoryRequirements req;
    vkGetBufferMemoryRequirements(dev, b.buf, &req);
    auto fd_props = (PFN_vkGetMemoryFdPropertiesKHR)vkGetDeviceProcAddr(dev, "vkGetMemoryFdPropertiesKHR");
    VkMemoryFdPropertiesKHR props{VK_STRUCTURE_TYPE_MEMORY_FD_PROPERTIES_KHR};
    VK_CHECK(fd_props(dev, type, fd, &props));
    uint32_t bits = props.memoryTypeBits & req.memoryTypeBits;
    if (!bits) {
        fprintf(stderr, "dma-buf is not importable as a buffer\n");
        exit(1);
    }
    VkImportMemoryFdInfoKHR import{VK_STRUCTURE_TYPE_IMPORT_MEMORY_FD_INFO_KHR, nullptr, type, fd};
    VkMemoryAllocateInfo ai{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    ai.pNext = &import;
    ai.allocationSize = size;
    ai.memoryTypeIndex = __builtin_ctz(bits);
    VK_CHECK(vkAllocateMemory(dev, &ai, nullptr, &b.mem));
    VK_CHECK(vkBindBufferMemory(dev, b.buf, b.mem, 0));
    return b;
}

static void image_barrier(VkCommandBuffer cmd, VkImage img, VkImageAspectFlags aspect, VkImageLayout from,
                          VkImageLayout to, VkAccessFlags src_access, VkAccessFlags dst_access,
                          VkPipelineStageFlags src_stage, VkPipelineStageFlags dst_stage) {
    VkImageMemoryBarrier b{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
    b.srcAccessMask = src_access;
    b.dstAccessMask = dst_access;
    b.oldLayout = from;
    b.newLayout = to;
    b.srcQueueFamilyIndex = b.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    b.image = img;
    b.subresourceRange = {aspect, 0, 1, 0, 1};
    vkCmdPipelineBarrier(cmd, src_stage, dst_stage, 0, 0, nullptr, 0, nullptr, 1, &b);
}

// ---- NPU generator ----------------------------------------------------------
static xrt::device npu_device(const xrt::xclbin &xclbin) {
    xrt::device device(0);
    device.register_xclbin(xclbin);
    return device;
}

struct Npu {
    xrt::xclbin xclbin{std::string("device.xclbin")};
    xrt::device device = npu_device(xclbin);
    xrt::hw_context context;
    xrt::ext::kernel kernel;
    xrt::bo in, out[2]; // heightmaps, double-buffered; the viewer imports them into Vulkan
    xrt::run run;
    int pending = -1;   // out buffer the run in flight writes, or -1
    float time[2] = {}; // the noise time each out buffer was generated for

    Npu()
        : context(device, xclbin.get_uuid()),
          kernel(context, xrt::module{xrt::elf{std::string("insts.elf")}}, "MLIR_AIE"),
          in(xrt::ext::bo{device, PERLIN_NPU_IN * 4}),
          out{xrt::ext::bo{device, HMAP_BYTES}, xrt::ext::bo{device, HMAP_BYTES}} {
        // The CPU never touches out again: the GPU copies it through the dma-buf.
        // Flushing it from the CPU caches once makes the per-frame sync, which is
        // 2.7 ms of cache maintenance over 64 MB, unnecessary.
        for (auto &b : out) b.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    }

    // Starts generating into out[b]. At most one run is in flight, so in is free.
    void start(const PerlinParams &p, int b) {
        perlin_fill_npu_input(in.map<int32_t *>(), p);
        in.sync(XCL_BO_SYNC_BO_TO_DEVICE);
        run = kernel(3, 0, 0, in, out[b]);
        pending = b;
        time[b] = p.time;
    }

    // Waits for the run in flight and returns the buffer it wrote
    int finish() {
        run.wait2();
        int b = pending;
        pending = -1;
        return b;
    }
};

int main(int argc, char **argv) {
    bool npu_mode = false;
    int rt = 3, ao_dirs = 8; // terrain.frag ray tracing level and load
    float trees = 1.0f;      // tree density
    const char *screenshot = nullptr;
    PerlinParams params{0.0f, 0.005f * PX, 1.3f, 6};
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--npu")) npu_mode = true;
        else if (!strcmp(argv[i], "--octaves") && i + 1 < argc) params.octaves = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--screenshot") && i + 1 < argc) screenshot = argv[++i];
        else if (!strcmp(argv[i], "--rt") && i + 1 < argc) rt = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--time") && i + 1 < argc) params.time = atof(argv[++i]);
        else if (!strcmp(argv[i], "--ao") && i + 1 < argc) ao_dirs = std::max(atoi(argv[++i]), 1);
        else if (!strcmp(argv[i], "--trees") && i + 1 < argc) trees = atof(argv[++i]);
        else {
            fprintf(stderr, "usage: %s [--npu] [--octaves N] [--rt 0-3] [--ao DIRS] [--trees D] [--time T] [--screenshot file.ppm]\n", argv[0]);
            return 1;
        }
    }

    Npu npu;

    // ===== Window, instance, device =========================================
    if (!SDL_Init(SDL_INIT_VIDEO)) { fprintf(stderr, "SDL_Init: %s\n", SDL_GetError()); return 1; }
    SDL_Window *window = SDL_CreateWindow("Perlin flight", 1280, 800, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
    if (!window) { fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError()); return 1; }

    uint32_t n_ext = 0;
    const char *const *sdl_ext = SDL_Vulkan_GetInstanceExtensions(&n_ext);
    VkApplicationInfo app{VK_STRUCTURE_TYPE_APPLICATION_INFO};
    app.pApplicationName = "perlin-fly";
    app.apiVersion = VK_API_VERSION_1_3;
    VkInstanceCreateInfo ici{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    ici.pApplicationInfo = &app;
    ici.enabledExtensionCount = n_ext;
    ici.ppEnabledExtensionNames = sdl_ext;
    VkInstance instance;
    VK_CHECK(vkCreateInstance(&ici, nullptr, &instance));
    VkSurfaceKHR surface;
    if (!SDL_Vulkan_CreateSurface(window, instance, nullptr, &surface)) {
        fprintf(stderr, "SDL_Vulkan_CreateSurface: %s\n", SDL_GetError());
        return 1;
    }

    VkPhysicalDevice pd = VK_NULL_HANDLE;
    uint32_t qfam = 0;
    {
        uint32_t n = 0;
        vkEnumeratePhysicalDevices(instance, &n, nullptr);
        std::vector<VkPhysicalDevice> pds(n);
        vkEnumeratePhysicalDevices(instance, &n, pds.data());
        for (auto cand : pds) {
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(cand, &props);
            if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU) continue;
            uint32_t nq = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(cand, &nq, nullptr);
            std::vector<VkQueueFamilyProperties> qp(nq);
            vkGetPhysicalDeviceQueueFamilyProperties(cand, &nq, qp.data());
            for (uint32_t i = 0; i < nq && !pd; i++) {
                VkBool32 present = VK_FALSE;
                vkGetPhysicalDeviceSurfaceSupportKHR(cand, i, surface, &present);
                if (present && (qp[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && (qp[i].queueFlags & VK_QUEUE_COMPUTE_BIT)) {
                    pd = cand;
                    qfam = i;
                }
            }
            if (pd) break;
        }
    }
    if (!pd) { fprintf(stderr, "no suitable Vulkan device\n"); return 1; }
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(pd, &props);

    float prio = 1.0f;
    VkDeviceQueueCreateInfo qci{VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
    qci.queueFamilyIndex = qfam;
    qci.queueCount = 1;
    qci.pQueuePriorities = &prio;
    const char *dev_ext[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_EXTERNAL_MEMORY_FD_EXTENSION_NAME,
                             VK_EXT_EXTERNAL_MEMORY_DMA_BUF_EXTENSION_NAME};
    VkPhysicalDeviceFeatures features{};
    features.geometryShader = VK_TRUE;
    VkPhysicalDeviceVulkan13Features f13{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};
    f13.dynamicRendering = VK_TRUE;
    VkDeviceCreateInfo dci{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    dci.pNext = &f13;
    dci.queueCreateInfoCount = 1;
    dci.pQueueCreateInfos = &qci;
    dci.enabledExtensionCount = 3;
    dci.ppEnabledExtensionNames = dev_ext;
    dci.pEnabledFeatures = &features;
    VkDevice dev;
    VK_CHECK(vkCreateDevice(pd, &dci, nullptr, &dev));
    VkQueue queue;
    vkGetDeviceQueue(dev, qfam, 0, &queue);

    // ===== Swapchain and depth buffer (recreated on resize) =================
    VkSurfaceFormatKHR surf_fmt;
    {
        uint32_t n = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(pd, surface, &n, nullptr);
        std::vector<VkSurfaceFormatKHR> fmts(n);
        vkGetPhysicalDeviceSurfaceFormatsKHR(pd, surface, &n, fmts.data());
        surf_fmt = fmts[0];
        for (auto &f : fmts)
            if (f.format == VK_FORMAT_B8G8R8A8_UNORM) surf_fmt = f;
    }
    VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
    {
        uint32_t n = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(pd, surface, &n, nullptr);
        std::vector<VkPresentModeKHR> modes(n);
        vkGetPhysicalDeviceSurfacePresentModesKHR(pd, surface, &n, modes.data());
        for (auto m : modes)
            if (m == VK_PRESENT_MODE_MAILBOX_KHR) present_mode = m;
    }

    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> sc_images;
    std::vector<VkImageView> sc_views;
    std::vector<VkSemaphore> sem_render; // per swapchain image
    VkExtent2D extent{};
    VkImage depth = VK_NULL_HANDLE;
    VkDeviceMemory depth_mem = VK_NULL_HANDLE;
    VkImageView depth_view = VK_NULL_HANDLE;

    auto destroy_swapchain = [&] {
        for (auto v : sc_views) vkDestroyImageView(dev, v, nullptr);
        for (auto s : sem_render) vkDestroySemaphore(dev, s, nullptr);
        sc_views.clear();
        sem_render.clear();
        vkDestroyImageView(dev, depth_view, nullptr);
        vkDestroyImage(dev, depth, nullptr);
        vkFreeMemory(dev, depth_mem, nullptr);
        vkDestroySwapchainKHR(dev, swapchain, nullptr);
    };
    auto create_swapchain = [&] {
        VkSurfaceCapabilitiesKHR caps;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pd, surface, &caps);
        extent = caps.currentExtent;
        if (extent.width == UINT32_MAX) {
            int w, h;
            SDL_GetWindowSizeInPixels(window, &w, &h);
            extent = {(uint32_t)w, (uint32_t)h};
        }
        uint32_t count = caps.minImageCount + 1;
        if (caps.maxImageCount && count > caps.maxImageCount) count = caps.maxImageCount;
        VkSwapchainCreateInfoKHR sci{VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
        sci.surface = surface;
        sci.minImageCount = count;
        sci.imageFormat = surf_fmt.format;
        sci.imageColorSpace = surf_fmt.colorSpace;
        sci.imageExtent = extent;
        sci.imageArrayLayers = 1;
        sci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        sci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        sci.preTransform = caps.currentTransform;
        sci.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        sci.presentMode = present_mode;
        sci.clipped = VK_TRUE;
        VK_CHECK(vkCreateSwapchainKHR(dev, &sci, nullptr, &swapchain));

        uint32_t n = 0;
        vkGetSwapchainImagesKHR(dev, swapchain, &n, nullptr);
        sc_images.resize(n);
        vkGetSwapchainImagesKHR(dev, swapchain, &n, sc_images.data());
        sc_views.resize(n);
        sem_render.resize(n);
        for (uint32_t i = 0; i < n; i++) {
            VkImageViewCreateInfo vi{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
            vi.image = sc_images[i];
            vi.viewType = VK_IMAGE_VIEW_TYPE_2D;
            vi.format = surf_fmt.format;
            vi.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
            VK_CHECK(vkCreateImageView(dev, &vi, nullptr, &sc_views[i]));
            VkSemaphoreCreateInfo si{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
            VK_CHECK(vkCreateSemaphore(dev, &si, nullptr, &sem_render[i]));
        }

        VkImageCreateInfo ci{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
        ci.imageType = VK_IMAGE_TYPE_2D;
        ci.format = DEPTH_FORMAT;
        ci.extent = {extent.width, extent.height, 1};
        ci.mipLevels = ci.arrayLayers = 1;
        ci.samples = VK_SAMPLE_COUNT_1_BIT;
        ci.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        VK_CHECK(vkCreateImage(dev, &ci, nullptr, &depth));
        VkMemoryRequirements req;
        vkGetImageMemoryRequirements(dev, depth, &req);
        VkMemoryAllocateInfo ai{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
        ai.allocationSize = req.size;
        ai.memoryTypeIndex = find_mem_type(pd, req.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        VK_CHECK(vkAllocateMemory(dev, &ai, nullptr, &depth_mem));
        VK_CHECK(vkBindImageMemory(dev, depth, depth_mem, 0));
        VkImageViewCreateInfo vi{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
        vi.image = depth;
        vi.viewType = VK_IMAGE_VIEW_TYPE_2D;
        vi.format = DEPTH_FORMAT;
        vi.subresourceRange = {VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1};
        VK_CHECK(vkCreateImageView(dev, &vi, nullptr, &depth_view));
    };
    create_swapchain();

    // ===== Heightmap buffer, written by the GPU generator or copied from the
    // NPU's output buffer (shared via dma-buf). Device-local, as the geometry
    // shader reads each pixel several times and the NPU's buffer is CPU-cached.
    Buffer hmap = host_buffer(pd, dev, HMAP_BYTES,
                              VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    memset(hmap.mapped, 0, HMAP_BYTES);
    const uint32_t *heights = (const uint32_t *)hmap.mapped;
    Buffer npu_hmap[2] = {import_dmabuf(dev, npu.out[0].export_buffer(), HMAP_BYTES),
                          import_dmabuf(dev, npu.out[1].export_buffer(), HMAP_BYTES)};

    // Trees: the visible ones, written by tree.comp, and the indirect draw it counts
    // them in (host-visible, so the count can be shown)
    constexpr VkDeviceSize TREES_BYTES = (VkDeviceSize)TREE_GRID * TREE_GRID * 16;
    Buffer tree_buf = host_buffer(pd, dev, TREES_BYTES, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    Buffer tree_draw = host_buffer(pd, dev, sizeof(VkDrawIndirectCommand),
                                   VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT |
                                       VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    memset(tree_draw.mapped, 0, sizeof(VkDrawIndirectCommand));

    // Binding 0: heightmap, 1: trees, 2: tree draw
    const VkShaderStageFlags all_stages = VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_VERTEX_BIT |
                                          VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    VkDescriptorSetLayout dsl;
    {
        VkDescriptorSetLayoutBinding b[3];
        for (uint32_t i = 0; i < 3; i++) b[i] = {i, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, all_stages};
        VkDescriptorSetLayoutCreateInfo ci{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
        ci.bindingCount = 3;
        ci.pBindings = b;
        VK_CHECK(vkCreateDescriptorSetLayout(dev, &ci, nullptr, &dsl));
    }
    VkDescriptorSet set;
    {
        VkDescriptorPoolSize ps{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 3};
        VkDescriptorPoolCreateInfo ci{VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
        ci.maxSets = 1;
        ci.poolSizeCount = 1;
        ci.pPoolSizes = &ps;
        VkDescriptorPool pool;
        VK_CHECK(vkCreateDescriptorPool(dev, &ci, nullptr, &pool));
        VkDescriptorSetAllocateInfo ai{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
        ai.descriptorPool = pool;
        ai.descriptorSetCount = 1;
        ai.pSetLayouts = &dsl;
        VK_CHECK(vkAllocateDescriptorSets(dev, &ai, &set));
        VkDescriptorBufferInfo bi[3] = {{hmap.buf, 0, HMAP_BYTES}, {tree_buf.buf, 0, TREES_BYTES},
                                        {tree_draw.buf, 0, sizeof(VkDrawIndirectCommand)}};
        VkWriteDescriptorSet w{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
        w.dstSet = set;
        w.descriptorCount = 3;
        w.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        w.pBufferInfo = bi;
        vkUpdateDescriptorSets(dev, 1, &w, 0, nullptr);
    }

    auto pipeline_layout = [&](VkShaderStageFlags stages, uint32_t pc_size) {
        VkPushConstantRange pc{stages, 0, pc_size};
        VkPipelineLayoutCreateInfo ci{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
        ci.setLayoutCount = 1;
        ci.pSetLayouts = &dsl;
        ci.pushConstantRangeCount = 1;
        ci.pPushConstantRanges = &pc;
        VkPipelineLayout l;
        VK_CHECK(vkCreatePipelineLayout(dev, &ci, nullptr, &l));
        return l;
    };

    auto compute_pipeline = [&](VkPipelineLayout layout, const char *spv) {
        VkComputePipelineCreateInfo ci{VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO};
        ci.stage = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0, VK_SHADER_STAGE_COMPUTE_BIT,
                    load_shader(dev, spv), "main"};
        ci.layout = layout;
        VkPipeline p;
        VK_CHECK(vkCreateComputePipelines(dev, VK_NULL_HANDLE, 1, &ci, nullptr, &p));
        return p;
    };

    // GPU generator
    VkPipelineLayout gen_layout = pipeline_layout(VK_SHADER_STAGE_COMPUTE_BIT, sizeof(PerlinQ12));
    VkPipeline gen_pipe = compute_pipeline(gen_layout, "perlin.comp.spv");

    // Terrain and trees
    static_assert(sizeof(TerrainPC) == 136);
    if (sizeof(TerrainPC) > props.limits.maxPushConstantsSize) {
        fprintf(stderr, "push constants too large\n");
        return 1;
    }
    VkPipelineLayout draw_layout = pipeline_layout(all_stages, sizeof(TerrainPC));
    VkPipeline tree_cull_pipe = compute_pipeline(draw_layout, "tree.comp.spv");
    auto graphics_pipeline = [&](std::vector<std::pair<VkShaderStageFlagBits, const char *>> shaders,
                                 VkPrimitiveTopology topology) {
        std::vector<VkPipelineShaderStageCreateInfo> stages;
        for (auto [stage, spv] : shaders)
            stages.push_back({VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0, stage,
                              load_shader(dev, spv), "main"});
        VkPipelineVertexInputStateCreateInfo vin{VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
        VkPipelineInputAssemblyStateCreateInfo ia{VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
        ia.topology = topology;
        VkPipelineViewportStateCreateInfo vp{VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
        vp.viewportCount = vp.scissorCount = 1;
        VkPipelineRasterizationStateCreateInfo rs{VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
        rs.polygonMode = VK_POLYGON_MODE_FILL;
        rs.cullMode = VK_CULL_MODE_NONE;
        rs.lineWidth = 1.0f;
        VkPipelineMultisampleStateCreateInfo ms{VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
        ms.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        VkPipelineDepthStencilStateCreateInfo ds{VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
        ds.depthTestEnable = ds.depthWriteEnable = VK_TRUE;
        ds.depthCompareOp = VK_COMPARE_OP_LESS;
        VkPipelineColorBlendAttachmentState cba{};
        cba.colorWriteMask = 0xF;
        VkPipelineColorBlendStateCreateInfo cb{VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
        cb.attachmentCount = 1;
        cb.pAttachments = &cba;
        VkDynamicState dyn[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
        VkPipelineDynamicStateCreateInfo dy{VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
        dy.dynamicStateCount = 2;
        dy.pDynamicStates = dyn;
        VkPipelineRenderingCreateInfo rci{VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO};
        rci.colorAttachmentCount = 1;
        rci.pColorAttachmentFormats = &surf_fmt.format;
        rci.depthAttachmentFormat = DEPTH_FORMAT;
        VkGraphicsPipelineCreateInfo ci{VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
        ci.pNext = &rci;
        ci.stageCount = (uint32_t)stages.size();
        ci.pStages = stages.data();
        ci.pVertexInputState = &vin;
        ci.pInputAssemblyState = &ia;
        ci.pViewportState = &vp;
        ci.pRasterizationState = &rs;
        ci.pMultisampleState = &ms;
        ci.pDepthStencilState = &ds;
        ci.pColorBlendState = &cb;
        ci.pDynamicState = &dy;
        ci.layout = draw_layout;
        VkPipeline p;
        VK_CHECK(vkCreateGraphicsPipelines(dev, VK_NULL_HANDLE, 1, &ci, nullptr, &p));
        return p;
    };
    VkPipeline draw_pipe = graphics_pipeline({{VK_SHADER_STAGE_VERTEX_BIT, "terrain.vert.spv"},
                                              {VK_SHADER_STAGE_GEOMETRY_BIT, "terrain.geom.spv"},
                                              {VK_SHADER_STAGE_FRAGMENT_BIT, "terrain.frag.spv"}},
                                             VK_PRIMITIVE_TOPOLOGY_POINT_LIST);
    VkPipeline tree_pipe = graphics_pipeline({{VK_SHADER_STAGE_VERTEX_BIT, "tree.vert.spv"},
                                              {VK_SHADER_STAGE_FRAGMENT_BIT, "tree.frag.spv"}},
                                             VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

    VkCommandBuffer cmd;
    {
        VkCommandPoolCreateInfo ci{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
        ci.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        ci.queueFamilyIndex = qfam;
        VkCommandPool pool;
        VK_CHECK(vkCreateCommandPool(dev, &ci, nullptr, &pool));
        VkCommandBufferAllocateInfo ai{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
        ai.commandPool = pool;
        ai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        ai.commandBufferCount = 1;
        VK_CHECK(vkAllocateCommandBuffers(dev, &ai, &cmd));
    }
    VkSemaphore sem_acquire;
    VkFence fence;
    VkQueryPool queries; // timestamps around the GPU generator and the terrain draw
    {
        VkSemaphoreCreateInfo si{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
        VK_CHECK(vkCreateSemaphore(dev, &si, nullptr, &sem_acquire));
        VkFenceCreateInfo fi{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
        fi.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        VK_CHECK(vkCreateFence(dev, &fi, nullptr, &fence));
        VkQueryPoolCreateInfo qi{VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO};
        qi.queryType = VK_QUERY_TYPE_TIMESTAMP;
        qi.queryCount = 3;
        VK_CHECK(vkCreateQueryPool(dev, &qi, nullptr, &queries));
    }

    // ===== Main loop ========================================================
    using clk = std::chrono::steady_clock;
    auto start = clk::now(), last = start, title_time = start;
    float speed = 80.0f, alt = 200.0f * VSCALE, sim = 0.0f;
    int step = 4, frames = 0;
    double gen_ms = 0, draw_ms = 0, frame_ms = 0; // NPU gen: wait for the NPU + GPU copy
    double tree_count = 0;
    bool have_queries = false, running = true;
    Buffer shot{};

    while (running) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_EVENT_QUIT) running = false;
            if (ev.type != SDL_EVENT_KEY_DOWN) continue;
            switch (ev.key.key) {
            case SDLK_ESCAPE: case SDLK_Q: running = false; break;
            case SDLK_TAB: npu_mode = !npu_mode; break;
            case SDLK_EQUALS: case SDLK_PLUS: case SDLK_KP_PLUS: params.octaves = std::min(params.octaves + 1, 128); break;
            case SDLK_MINUS: case SDLK_KP_MINUS: params.octaves = std::max(params.octaves - 1, 1); break;
            case SDLK_UP: speed *= 1.25f; break;
            case SDLK_DOWN: speed /= 1.25f; break;
            case SDLK_1: step = 1; break;
            case SDLK_2: step = 2; break;
            case SDLK_4: step = 4; break;
            case SDLK_8: step = 8; break;
            case SDLK_R: rt = (rt + 1) % 4; break;
            case SDLK_LEFTBRACKET: ao_dirs = std::max(ao_dirs / 2, 1); break;
            case SDLK_RIGHTBRACKET: ao_dirs = std::min(ao_dirs * 2, 256); break;
            case SDLK_T: trees = trees >= 1.0f ? 0.5f : trees >= 0.5f ? 0.25f : trees > 0.0f ? 0.0f : 1.0f; break;
            }
        }

        auto now = clk::now();
        float dt = std::min(std::chrono::duration<float>(now - last).count(), 0.1f);
        last = now;
        sim += dt;
        // Raising time by d moves the terrain d/scale pixels along -x and -y
        params.time = fmodf(params.time + speed * dt * params.scale / (PX * sqrtf(2.0f)), 256.0f);

        VK_CHECK(vkWaitForFences(dev, 1, &fence, VK_TRUE, UINT64_MAX));
        if (have_queries) {
            uint64_t ts[3];
            VK_CHECK(vkGetQueryPoolResults(dev, queries, 0, 3, sizeof(ts), ts, 8, VK_QUERY_RESULT_64_BIT));
            gen_ms += (ts[1] - ts[0]) * props.limits.timestampPeriod * 1e-6;
            draw_ms += (ts[2] - ts[1]) * props.limits.timestampPeriod * 1e-6;
            if (trees > 0) tree_count += ((const VkDrawIndirectCommand *)tree_draw.mapped)->instanceCount;
        }

        // Camera: fixed spot on the diagonal with a gentle sway, kept above the
        // terrain ahead (from the previous frame's heights)
        float yaw = 0.1f * sinf(sim * 0.11f), side = 30.0f * sinf(sim * 0.07f);
        V3 fwd = {cosf((float)M_PI / 4 + yaw), 0, sinf((float)M_PI / 4 + yaw)};
        V3 eye = {CAM_POS + side * 0.7071f, 0, CAM_POS - side * 0.7071f};
        float ground = WATER;
        for (int k = 0; k <= 160; k += 4) {
            int x = (int)((eye.x + fwd.x * k) / PX), y = (int)((eye.z + fwd.z * k) / PX);
            ground = std::max(ground, (float)(heights[y * PERLIN_W + x] & 255) - k * 0.2f);
        }
        float target = ground * VSCALE + CLEARANCE;
        alt += (target - alt) * std::min(1.0f, dt * (target > alt ? 3.0f : 0.8f));
        eye.y = alt;

        // The NPU runs one frame ahead: wait for the heightmap started last frame
        // (the next one is started after this frame's GPU submit)
        int npu_buf = -1;
        if (npu_mode) {
            auto wait_start = clk::now();
            if (npu.pending < 0) npu.start(params, 0);
            npu_buf = npu.finish();
            gen_ms += std::chrono::duration<double, std::milli>(clk::now() - wait_start).count();
        } else if (npu.pending >= 0) {
            npu.finish(); // stale once the GPU generator has taken over
        }

        uint32_t img;
        VkResult acq = vkAcquireNextImageKHR(dev, swapchain, UINT64_MAX, sem_acquire, VK_NULL_HANDLE, &img);
        if (acq == VK_ERROR_OUT_OF_DATE_KHR) {
            vkDeviceWaitIdle(dev);
            destroy_swapchain();
            create_swapchain();
            continue;
        }
        VK_CHECK(vkResetFences(dev, 1, &fence));

        TerrainPC pc{};
        float proj[16], view[16];
        perspective(proj, 50.0f * (float)M_PI / 180.0f, (float)extent.width / extent.height, 1.0f, FOG_END);
        V3 at = {eye.x + fwd.x * 100, eye.y - 22.0f, eye.z + fwd.z * 100};
        look_at(view, eye, at, {0, 1, 0});
        mat_mul(pc.viewProj, proj, view);
        V3 sun = normalize({0.6f, 0.35f, -0.5f});
        pc.eye[0] = eye.x, pc.eye[1] = eye.y, pc.eye[2] = eye.z, pc.eye[3] = FOG_END;
        pc.sun[0] = sun.x, pc.sun[1] = sun.y, pc.sun[2] = sun.z, pc.sun[3] = FOG_START;
        pc.step = step;
        pc.cells = PERLIN_W / step - 1;
        pc.vscale = VSCALE;
        pc.water = WATER;
        pc.rt = rt;
        pc.ao_dirs = ao_dirs;
        {
            // Trees sit in noise space: pixel p of this heightmap is noise pixel p + t/s
            // (octave 0), with t the time the heightmap was generated for
            PerlinParams hp = params;
            if (npu_mode) hp.time = npu.time[npu_buf];
            PerlinQ12 q = perlin_q12(hp);
            double off = (double)q.t / q.s, cell = floor(off / TREE_CELL);
            pc.tree_cell[0] = pc.tree_cell[1] = (int32_t)cell;
            pc.tree_frac = (float)(off - cell * TREE_CELL);
        }
        pc.trees = trees;

        VK_CHECK(vkResetCommandBuffer(cmd, 0));
        VkCommandBufferBeginInfo bi{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
        bi.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        VK_CHECK(vkBeginCommandBuffer(cmd, &bi));
        vkCmdResetQueryPool(cmd, queries, 0, 3);
        vkCmdWriteTimestamp(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, queries, 0);
        const VkPipelineStageFlags hmap_readers = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT |
                                                  VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT |
                                                  VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        if (npu_mode) {
            VkBufferCopy region{0, 0, HMAP_BYTES};
            vkCmdCopyBuffer(cmd, npu_hmap[npu_buf].buf, hmap.buf, 1, &region);
            VkMemoryBarrier mb{VK_STRUCTURE_TYPE_MEMORY_BARRIER, nullptr, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT};
            vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, hmap_readers, 0, 1, &mb, 0, nullptr, 0, nullptr);
        } else {
            PerlinQ12 q = perlin_q12(params);
            vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, gen_pipe);
            vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, gen_layout, 0, 1, &set, 0, nullptr);
            vkCmdPushConstants(cmd, gen_layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(q), &q);
            vkCmdDispatch(cmd, PERLIN_W / 16, PERLIN_H / 16, 1);
            VkMemoryBarrier mb{VK_STRUCTURE_TYPE_MEMORY_BARRIER, nullptr, VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT};
            vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, hmap_readers, 0, 1, &mb, 0, nullptr, 0,
                                 nullptr);
        }
        vkCmdWriteTimestamp(cmd, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, queries, 1);

        // Tree placement and culling, into the indirect draw
        if (trees > 0) {
            VkDrawIndirectCommand reset{TREE_VERTS, 0, 0, 0};
            vkCmdUpdateBuffer(cmd, tree_draw.buf, 0, sizeof(reset), &reset);
            VkMemoryBarrier mb{VK_STRUCTURE_TYPE_MEMORY_BARRIER, nullptr, VK_ACCESS_TRANSFER_WRITE_BIT,
                               VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT};
            vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 1, &mb,
                                 0, nullptr, 0, nullptr);
            vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, tree_cull_pipe);
            vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, draw_layout, 0, 1, &set, 0, nullptr);
            vkCmdPushConstants(cmd, draw_layout, all_stages, 0, sizeof(pc), &pc);
            vkCmdDispatch(cmd, (TREE_GRID + 15) / 16, (TREE_GRID + 15) / 16, 1);
            mb = {VK_STRUCTURE_TYPE_MEMORY_BARRIER, nullptr, VK_ACCESS_SHADER_WRITE_BIT,
                  VK_ACCESS_INDIRECT_COMMAND_READ_BIT | VK_ACCESS_SHADER_READ_BIT};
            vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                 VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT | VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, 0, 1, &mb,
                                 0, nullptr, 0, nullptr);
        }

        image_barrier(cmd, sc_images[img], VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
                      VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
        image_barrier(cmd, depth, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
                      VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL, 0, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                      VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                      VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT);
        VkRenderingAttachmentInfo color_att{VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
        color_att.imageView = sc_views[img];
        color_att.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        color_att.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_att.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_att.clearValue.color = {{0.62f, 0.75f, 0.90f, 1.0f}}; // SKY in terrain.frag
        VkRenderingAttachmentInfo depth_att{VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
        depth_att.imageView = depth_view;
        depth_att.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
        depth_att.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth_att.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth_att.clearValue.depthStencil = {1.0f, 0};
        VkRenderingInfo ri{VK_STRUCTURE_TYPE_RENDERING_INFO};
        ri.renderArea = {{0, 0}, extent};
        ri.layerCount = 1;
        ri.colorAttachmentCount = 1;
        ri.pColorAttachments = &color_att;
        ri.pDepthAttachment = &depth_att;
        vkCmdBeginRendering(cmd, &ri);
        VkViewport viewport{0, 0, (float)extent.width, (float)extent.height, 0, 1};
        VkRect2D scissor{{0, 0}, extent};
        vkCmdSetViewport(cmd, 0, 1, &viewport);
        vkCmdSetScissor(cmd, 0, 1, &scissor);

        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, draw_pipe);
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, draw_layout, 0, 1, &set, 0, nullptr);
        vkCmdPushConstants(cmd, draw_layout, all_stages, 0, sizeof(pc), &pc);
        vkCmdDraw(cmd, pc.cells * pc.cells, 1, 0, 0);
        if (trees > 0) {
            vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, tree_pipe);
            vkCmdDrawIndirect(cmd, tree_draw.buf, 0, 1, sizeof(VkDrawIndirectCommand));
        }
        vkCmdEndRendering(cmd);
        vkCmdWriteTimestamp(cmd, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, queries, 2);

        bool capture = screenshot && now - start > std::chrono::seconds(3);
        VkImageLayout final_layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        if (capture) {
            shot = host_buffer(pd, dev, (VkDeviceSize)extent.width * extent.height * 4, VK_BUFFER_USAGE_TRANSFER_DST_BIT);
            image_barrier(cmd, sc_images[img], VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                          VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                          VK_ACCESS_TRANSFER_READ_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                          VK_PIPELINE_STAGE_TRANSFER_BIT);
            VkBufferImageCopy region{};
            region.imageSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
            region.imageExtent = {extent.width, extent.height, 1};
            vkCmdCopyImageToBuffer(cmd, sc_images[img], VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, shot.buf, 1, &region);
            image_barrier(cmd, sc_images[img], VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                          final_layout, VK_ACCESS_TRANSFER_READ_BIT, 0, VK_PIPELINE_STAGE_TRANSFER_BIT,
                          VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
        } else {
            image_barrier(cmd, sc_images[img], VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                          final_layout, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, 0,
                          VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
        }
        VK_CHECK(vkEndCommandBuffer(cmd));

        VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo si{VK_STRUCTURE_TYPE_SUBMIT_INFO};
        si.waitSemaphoreCount = 1;
        si.pWaitSemaphores = &sem_acquire;
        si.pWaitDstStageMask = &wait_stage;
        si.commandBufferCount = 1;
        si.pCommandBuffers = &cmd;
        si.signalSemaphoreCount = 1;
        si.pSignalSemaphores = &sem_render[img];
        VK_CHECK(vkQueueSubmit(queue, 1, &si, fence));
        // Generate the next heightmap into the other buffer while the GPU draws.
        // That buffer's last copy finished before the fence wait above. Starting it
        // after the submit matters: the GPU job references both shared buffers, and
        // implicit dma-buf sync would make it wait for an NPU run already in flight.
        if (npu_buf >= 0) npu.start(params, npu_buf ^ 1);
        have_queries = true;

        VkPresentInfoKHR pi{VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
        pi.waitSemaphoreCount = 1;
        pi.pWaitSemaphores = &sem_render[img];
        pi.swapchainCount = 1;
        pi.pSwapchains = &swapchain;
        pi.pImageIndices = &img;
        VkResult pres = vkQueuePresentKHR(queue, &pi);
        if (pres == VK_ERROR_OUT_OF_DATE_KHR || pres == VK_SUBOPTIMAL_KHR) {
            vkDeviceWaitIdle(dev);
            destroy_swapchain();
            create_swapchain();
        }

        if (capture) {
            VK_CHECK(vkWaitForFences(dev, 1, &fence, VK_TRUE, UINT64_MAX));
            FILE *f = fopen(screenshot, "wb");
            fprintf(f, "P6\n%u %u\n255\n", extent.width, extent.height);
            const uint8_t *p = (const uint8_t *)shot.mapped;
            bool bgr = surf_fmt.format == VK_FORMAT_B8G8R8A8_UNORM || surf_fmt.format == VK_FORMAT_B8G8R8A8_SRGB;
            for (uint32_t i = 0; i < extent.width * extent.height; i++, p += 4) {
                uint8_t rgb[3] = {p[bgr ? 2 : 0], p[1], p[bgr ? 0 : 2]};
                fwrite(rgb, 1, 3, f);
            }
            fclose(f);
            int n = std::max(frames, 1);
            printf("%s %s: gen %.3f ms (%s)  draw %.3f ms  frame %.3f ms  %.0f trees\n", screenshot,
                   npu_mode ? "NPU" : "GPU", gen_ms / n, npu_mode ? "NPU wait + copy" : "GPU", draw_ms / n,
                   frame_ms / n, tree_count / n);
            running = false;
        }

        frames++;
        frame_ms += std::chrono::duration<double, std::milli>(clk::now() - now).count();
        if (now - title_time > std::chrono::seconds(1)) {
            char title[200];
            snprintf(title, sizeof(title),
                     "Perlin flight | %s generator %.2f ms%s | draw %.2f ms | %d octaves | step %d | rt %d, ao %d | "
                     "%.0fk trees | %d fps",
                     npu_mode ? "NPU" : "GPU", gen_ms / frames, npu_mode ? " wait+copy" : "", draw_ms / frames,
                     params.octaves, step, rt, ao_dirs, tree_count / frames / 1000, frames);
            SDL_SetWindowTitle(window, title);
            frames = 0;
            gen_ms = draw_ms = frame_ms = tree_count = 0;
            title_time = now;
        }
    }

    vkDeviceWaitIdle(dev);
    if (npu.pending >= 0) npu.finish();
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
