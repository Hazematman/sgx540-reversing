The powerVR driver is separated into a few differt components:

- `libGLESv*_POWERVR_*`: opengl ES implementation
- `libIMGegl_*`: EGL implementation
- `libsrv_um_*`: low-level code including memory allocation and other system calls
- `libglslcompiler`: actual glsl-to-unixflex compiler, note that this is loaded using a function provided by `libsrv_um`
- `libpvr2d_*`: exact purpose is unknown at the moment
- `libsrv_init`: library to load the microkernel onto the gpu and do general userspace initialization

TODO: add information obtained by reverse engineering the libraries for the SGX540
