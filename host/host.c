#include <xrt/xrt_device.h>
#include <xrt/xrt_kernel.h>

int main() {
    // Get device handle
    xrtDeviceHandle my_device = xrtDeviceOpen(0);
    // Load xclbin
    if(xrtDeviceLoadXclbinFile(my_device, "device.xclbin") != 0) goto error;
    xuid_t handle;
    if(xrtXclbinGetUUID(my_device, handle) != 0) goto error;
    
    // Select(?) kernel
    xrtKernelHandle kernel = xrtPLKernelOpen(my_device, handle, "kernel_name");
    if(kernel == XRT_NULL_HANDLE) goto error;

    // Run kernel
    xrtRunHandle kernel_run = xrtKernelRun(kernel);
    xrtRunWait(kernel_run);

    // Release device
    xrtRunClose(kernel_run);
error:
    xrtDeviceClose(my_device);
}
