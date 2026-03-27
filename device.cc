
extern "C" {
void mandelbrot_kernel(float *data) {
    for(int i = 0; i < 256; i++) {
        data[i] = 0.33;
    }
    return;
}
}
