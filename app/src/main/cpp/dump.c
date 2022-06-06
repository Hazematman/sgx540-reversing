#include <stdio.h>
#include <android/log.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stdbool.h>
#include <string.h>
#include <linux/ioctl.h>
#include <stdint.h>

static int (*real_ioctl)(int d, unsigned long request, char *argp)=NULL;

#define GPU_FILE "/dev/pvrsrvkm"
#define ASSERT_ARRAY(a) \
    sizeof(char[1-2*__builtin_types_compatible_p(__typeof__(a), __typeof__(&(a)[0]))])

#define S(a) \
    (ASSERT_ARRAY(a)*0 + sizeof(a)/sizeof((a)[0]))

static void mtrace_init(void) {
    real_ioctl = dlsym(RTLD_NEXT, "ioctl");
    if (NULL == real_ioctl) {
        fprintf(stderr, "Error in `dlsym`: %s\n", dlerror());
    }
}

extern ssize_t readlink(const char *restrict pathname, char *restrict buf,
                size_t bufsiz);


bool is_gpu(int fd) {
    char in[128] = { 0 }, out[128] = { 0 };
    snprintf(in, sizeof(in), "/proc/self/fd/%d", fd);

    int count = readlink(in, out, sizeof(out) - 1);
    return count == strlen(GPU_FILE) && strncmp(out, GPU_FILE, count) == 0;
}

int ioctl(int d, unsigned long request, void *argp) {
    if(real_ioctl==NULL) {
        mtrace_init();
    }

    int p = real_ioctl(d, request, argp);
    //this second check is technically not necessary
    if (is_gpu(d) && (_IOC_TYPE(request) == 0x67)) {
        int callnum = _IOC_NR(request);
        __android_log_print(ANDROID_LOG_VERBOSE, "PVR: UNKNOWN", "ioctl: type: %x number: %x size: %x", _IOC_TYPE(request), callnum, _IOC_SIZE(request));
    }

    return p;
}
