#include <xrt/xrt_device.h>
#include <xrt/xrt_kernel.h>
#include <stdio.h>

int main() {
    // Get device handle
    xrtDeviceHandle my_device = xrtDeviceOpen(0);
    printf("Got device\n");

    if(xrtDeviceLoadXclbinFile(my_device, "device.xclbin") != 0) goto error;
    printf("Loaded xclbin\n");
    xuid_t handle;
    if(xrtXclbinGetUUID(my_device, handle) != 0) goto error;
    printf("Got UUID\n");
    
    // Select(?) kernel
    xrtKernelHandle kernel = xrtPLKernelOpen(my_device, handle, "kernel_name");
    if(kernel == XRT_NULL_HANDLE) goto error;
    printf("Kernel selected\n");

    // Run kernel
    xrtRunHandle kernel_run = xrtKernelRun(kernel);
    xrtRunWait(kernel_run);
    printf("Ran kernel\n");

    // Release device
    xrtRunClose(kernel_run);
    printf("All done\n");
error:
    xrtDeviceClose(my_device);
}
