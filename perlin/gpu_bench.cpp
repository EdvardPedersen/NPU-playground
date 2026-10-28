// Headless Vulkan runner for perlin.comp.
//   gpu_bench dump  t s h octaves file [t s h octaves file ...]   write raw frames
//   gpu_bench bench octaves [seconds]                             time (and power)
#include <vulkan/vulkan.h>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>
#include "perlin_params.h"
#include "power.h"

#define VK_CHECK(x) do { VkResult r_ = (x); if (r_ != VK_SUCCESS) { fprintf(stderr, "%s failed: %d\n", #x, r_); exit(1); } } while (0)

constexpr VkDeviceSize OUT_BYTES = PERLIN_W * PERLIN_H * 4;

struct Gpu {
    VkInstance instance;
    VkPhysicalDevice pd;
    VkDevice dev;
    VkQueue queue;
    VkBuffer buf;
    VkDeviceMemory mem;
    uint32_t *mapped;
    VkPipelineLayout layout;
    VkPipeline pipe;
    VkDescriptorSet set;
    VkCommandBuffer cmd;
    VkFence fence;
    VkQueryPool queries;
    float ns_per_tick;

    Gpu() {
        VkApplicationInfo app{VK_STRUCTURE_TYPE_APPLICATION_INFO};
        app.apiVersion = VK_API_VERSION_1_2;
        VkInstanceCreateInfo ici{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
        ici.pApplicationInfo = &app;
        VK_CHECK(vkCreateInstance(&ici, nullptr, &instance));

        uint32_t n = 0;
        vkEnumeratePhysicalDevices(instance, &n, nullptr);
        std::vector<VkPhysicalDevice> pds(n);
        vkEnumeratePhysicalDevices(instance, &n, pds.data());
        pd = pds[0];
        for (auto p : pds) {
            VkPhysicalDeviceProperties pr;
            vkGetPhysicalDeviceProperties(p, &pr);
            if (pr.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU || pr.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) { pd = p; break; }
        }
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(pd, &props);
        ns_per_tick = props.limits.timestampPeriod;

        vkGetPhysicalDeviceQueueFamilyProperties(pd, &n, nullptr);
        std::vector<VkQueueFamilyProperties> qf(n);
        vkGetPhysicalDeviceQueueFamilyProperties(pd, &n, qf.data());
        uint32_t family = 0;
        while (!(qf[family].queueFlags & VK_QUEUE_COMPUTE_BIT)) family++;

        float prio = 1.0f;
        VkDeviceQueueCreateInfo qci{VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
        qci.queueFamilyIndex = family;
        qci.queueCount = 1;
        qci.pQueuePriorities = &prio;
        VkDeviceCreateInfo dci{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
        dci.queueCreateInfoCount = 1;
        dci.pQueueCreateInfos = &qci;
        VK_CHECK(vkCreateDevice(pd, &dci, nullptr, &dev));
        vkGetDeviceQueue(dev, family, 0, &queue);

        // Output buffer in host-visible device memory (UMA), read back like the NPU's
        VkBufferCreateInfo bi{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
        bi.size = OUT_BYTES;
        bi.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        VK_CHECK(vkCreateBuffer(dev, &bi, nullptr, &buf));
        VkMemoryRequirements req;
        vkGetBufferMemoryRequirements(dev, buf, &req);
        VkPhysicalDeviceMemoryProperties mp;
        vkGetPhysicalDeviceMemoryProperties(pd, &mp);
        const VkMemoryPropertyFlags want = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        uint32_t type = 0;
        while (!((req.memoryTypeBits >> type) & 1) || (mp.memoryTypes[type].propertyFlags & want) != want) type++;
        VkMemoryAllocateInfo ai{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
        ai.allocationSize = req.size;
        ai.memoryTypeIndex = type;
        VK_CHECK(vkAllocateMemory(dev, &ai, nullptr, &mem));
        VK_CHECK(vkBindBufferMemory(dev, buf, mem, 0));
        VK_CHECK(vkMapMemory(dev, mem, 0, OUT_BYTES, 0, (void **)&mapped));

        std::ifstream f("perlin.comp.spv", std::ios::binary);
        std::vector<char> code((std::istreambuf_iterator<char>(f)), {});
        VkShaderModuleCreateInfo smci{VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
        smci.codeSize = code.size();
        smci.pCode = (const uint32_t *)code.data();
        VkShaderModule sm;
        VK_CHECK(vkCreateShaderModule(dev, &smci, nullptr, &sm));

        VkDescriptorSetLayoutBinding b{0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT};
        VkDescriptorSetLayoutCreateInfo dl{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
        dl.bindingCount = 1;
        dl.pBindings = &b;
        VkDescriptorSetLayout dsl;
        VK_CHECK(vkCreateDescriptorSetLayout(dev, &dl, nullptr, &dsl));
        VkPushConstantRange pcr{VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(PerlinQ12)};
        VkPipelineLayoutCreateInfo plci{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
        plci.setLayoutCount = 1;
        plci.pSetLayouts = &dsl;
        plci.pushConstantRangeCount = 1;
        plci.pPushConstantRanges = &pcr;
        VK_CHECK(vkCreatePipelineLayout(dev, &plci, nullptr, &layout));
        VkComputePipelineCreateInfo cpci{VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO};
        cpci.stage = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0, VK_SHADER_STAGE_COMPUTE_BIT, sm, "main"};
        cpci.layout = layout;
        VK_CHECK(vkCreateComputePipelines(dev, VK_NULL_HANDLE, 1, &cpci, nullptr, &pipe));

        VkDescriptorPoolSize ps{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1};
        VkDescriptorPoolCreateInfo dpci{VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
        dpci.maxSets = 1;
        dpci.poolSizeCount = 1;
        dpci.pPoolSizes = &ps;
        VkDescriptorPool pool;
        VK_CHECK(vkCreateDescriptorPool(dev, &dpci, nullptr, &pool));
        VkDescriptorSetAllocateInfo dsai{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
        dsai.descriptorPool = pool;
        dsai.descriptorSetCount = 1;
        dsai.pSetLayouts = &dsl;
        VK_CHECK(vkAllocateDescriptorSets(dev, &dsai, &set));
        VkDescriptorBufferInfo dbi{buf, 0, OUT_BYTES};
        VkWriteDescriptorSet w{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
        w.dstSet = set;
        w.descriptorCount = 1;
        w.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        w.pBufferInfo = &dbi;
        vkUpdateDescriptorSets(dev, 1, &w, 0, nullptr);

        VkCommandPoolCreateInfo cpi{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
        cpi.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        cpi.queueFamilyIndex = family;
        VkCommandPool cp;
        VK_CHECK(vkCreateCommandPool(dev, &cpi, nullptr, &cp));
        VkCommandBufferAllocateInfo cbai{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
        cbai.commandPool = cp;
        cbai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        cbai.commandBufferCount = 1;
        VK_CHECK(vkAllocateCommandBuffers(dev, &cbai, &cmd));
        VkFenceCreateInfo fci{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
        VK_CHECK(vkCreateFence(dev, &fci, nullptr, &fence));
        VkQueryPoolCreateInfo qpci{VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO};
        qpci.queryType = VK_QUERY_TYPE_TIMESTAMP;
        qpci.queryCount = 2;
        VK_CHECK(vkCreateQueryPool(dev, &qpci, nullptr, &queries));
    }

    void record(const PerlinQ12 &q) {
        VK_CHECK(vkResetCommandBuffer(cmd, 0));
        VkCommandBufferBeginInfo cbi{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
        VK_CHECK(vkBeginCommandBuffer(cmd, &cbi));
        vkCmdResetQueryPool(cmd, queries, 0, 2);
        vkCmdWriteTimestamp(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, queries, 0);
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, pipe);
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, layout, 0, 1, &set, 0, nullptr);
        vkCmdPushConstants(cmd, layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(q), &q);
        vkCmdDispatch(cmd, PERLIN_W / 16, PERLIN_H / 16, 1);
        VkMemoryBarrier mb{VK_STRUCTURE_TYPE_MEMORY_BARRIER, nullptr, VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_HOST_READ_BIT};
        vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_HOST_BIT, 0, 1, &mb, 0, nullptr, 0, nullptr);
        vkCmdWriteTimestamp(cmd, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, queries, 1);
        VK_CHECK(vkEndCommandBuffer(cmd));
    }

    void run() {
        VkSubmitInfo si{VK_STRUCTURE_TYPE_SUBMIT_INFO};
        si.commandBufferCount = 1;
        si.pCommandBuffers = &cmd;
        VK_CHECK(vkQueueSubmit(queue, 1, &si, fence));
        VK_CHECK(vkWaitForFences(dev, 1, &fence, VK_TRUE, UINT64_MAX));
        VK_CHECK(vkResetFences(dev, 1, &fence));
    }

    double gpu_ms() {
        uint64_t ts[2];
        VK_CHECK(vkGetQueryPoolResults(dev, queries, 0, 2, sizeof(ts), ts, 8, VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT));
        return (ts[1] - ts[0]) * ns_per_tick * 1e-6;
    }
};


int main(int argc, char **argv) {
    if (argc < 3) { fprintf(stderr, "usage: %s dump t s h octaves file ... | bench octaves [seconds]\n", argv[0]); return 1; }
    Gpu gpu;
    if (!strcmp(argv[1], "dump")) {
        for (int k = 2; k + 5 <= argc; k += 5) {
            PerlinParams p{(float)atof(argv[k]), (float)atof(argv[k + 1]), (float)atof(argv[k + 2]), atoi(argv[k + 3])};
            PerlinQ12 q = perlin_q12(p);
            gpu.record(q);
            gpu.run();
            FILE *f = fopen(argv[k + 4], "wb");
            fwrite(gpu.mapped, 4, PERLIN_W * PERLIN_H, f);
            fclose(f);
            printf("%s %d %d %d %d\n", argv[k + 4], q.t, q.s, q.h, q.octaves);
        }
        return 0;
    }
    using C = std::chrono::steady_clock;
    PerlinParams p{12.3f, 0.3f, 1.0f, atoi(argv[2])};
    double secs = argc > 3 ? atof(argv[3]) : 0;
    gpu.record(perlin_q12(p));
    for (int i = 0; i < 20; i++) gpu.run();
    const int R = 500;
    double gpu_sum = 0;
    auto t0 = C::now();
    for (int i = 0; i < R; i++) { gpu.run(); gpu_sum += gpu.gpu_ms(); }
    double ms = std::chrono::duration<double, std::milli>(C::now() - t0).count() / R;
    printf("octaves %d  wall %.3f ms/frame  gpu %.3f ms\n", p.octaves, ms, gpu_sum / R);
    if (secs > 0) {
        double sum = 0;
        int n = 0;
        auto end = C::now() + std::chrono::milliseconds((long)(secs * 1000)), next = C::now() + std::chrono::seconds(1);
        while (C::now() < end) {
            gpu.run();
            if (C::now() > next) { sum += apu_power_w(); n++; next += std::chrono::milliseconds(250); }
        }
        printf("power %.1f W\n", sum / n);
    }
}
