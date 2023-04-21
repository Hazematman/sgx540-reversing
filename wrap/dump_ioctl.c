#include <stdio.h>

#define TRANSFER_QUEUE
#include "pvr_ioctl.h"

int main() {
    int arr_size = sizeof(pvrsrv_ioctl_names)/sizeof(pvrsrv_ioctl_names[0]);
    for(int i = 0; i < arr_size; i++) {
        printf("%d = %s\n", i, pvrsrv_ioctl_names[i]);
    }

    return 0;
}
