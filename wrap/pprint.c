#include <assert.h>
#include <stdbool.h>

#define PPRINT(fp, data, type) pprint_##type(fp, data)

#define PPRINT_CASE(name) \
    case name: \
        return #name

#define UNUSED(x) (void)x

static const char *pprint_PVRSRV_ERROR(FILE *fp, PVRSRV_ERROR data) {
    UNUSED(fp);
    switch(data) {
        PPRINT_CASE(PVRSRV_OK);
        PPRINT_CASE(PVRSRV_ERROR_OUT_OF_MEMORY);
        PPRINT_CASE(PVRSRV_ERROR_TOO_FEW_BUFFERS);
        PPRINT_CASE(PVRSRV_ERROR_INVALID_PARAMS);
        PPRINT_CASE(PVRSRV_ERROR_INIT_FAILURE);
        PPRINT_CASE(PVRSRV_ERROR_CANT_REGISTER_CALLBACK);
        PPRINT_CASE(PVRSRV_ERROR_INVALID_DEVICE);

        PPRINT_CASE(PVRSRV_ERROR_NOT_OWNER);
        PPRINT_CASE(PVRSRV_ERROR_BAD_MAPPING);
        PPRINT_CASE(PVRSRV_ERROR_TIMEOUT);
        PPRINT_CASE(PVRSRV_ERROR_FLIP_CHAIN_EXISTS);
        PPRINT_CASE(PVRSRV_ERROR_INVALID_SWAPINTERVAL);
        PPRINT_CASE(PVRSRV_ERROR_SCENE_INVALID);
        PPRINT_CASE(PVRSRV_ERROR_STREAM_ERROR);
        PPRINT_CASE(PVRSRV_ERROR_FAILED_DEPENDENCIES);
        PPRINT_CASE(PVRSRV_ERROR_CMD_NOT_PROCESSED);
        PPRINT_CASE(PVRSRV_ERROR_CMD_TOO_BIG);
        PPRINT_CASE(PVRSRV_ERROR_DEVICE_REGISTER_FAILED);
        PPRINT_CASE(PVRSRV_ERROR_TOOMANYBUFFERS);
        PPRINT_CASE(PVRSRV_ERROR_NOT_SUPPORTED);
        PPRINT_CASE(PVRSRV_ERROR_PROCESSING_BLOCKED);

        PPRINT_CASE(PVRSRV_ERROR_CANNOT_FLUSH_QUEUE);
        PPRINT_CASE(PVRSRV_ERROR_CANNOT_GET_QUEUE_SPACE);
        PPRINT_CASE(PVRSRV_ERROR_CANNOT_GET_RENDERDETAILS);
        PPRINT_CASE(PVRSRV_ERROR_RETRY);

        PPRINT_CASE(PVRSRV_ERROR_DDK_VERSION_MISMATCH);
        PPRINT_CASE(PVRSRV_ERROR_BUILD_MISMATCH);
        PPRINT_CASE(PVRSRV_ERROR_CORE_REVISION_MISMATCH);

        PPRINT_CASE(PVRSRV_ERROR_UPLOAD_TOO_BIG);

        PPRINT_CASE(PVRSRV_ERROR_INVALID_FLAGS);
        PPRINT_CASE(PVRSRV_ERROR_FAILED_TO_REGISTER_PROCESS);

        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_LOAD_LIBRARY);
        PPRINT_CASE(PVRSRV_ERROR_UNABLE_GET_FUNC_ADDR);
        PPRINT_CASE(PVRSRV_ERROR_UNLOAD_LIBRARY_FAILED);

        PPRINT_CASE(PVRSRV_ERROR_BRIDGE_CALL_FAILED);
        PPRINT_CASE(PVRSRV_ERROR_IOCTL_CALL_FAILED);

        PPRINT_CASE(PVRSRV_ERROR_MMU_CONTEXT_NOT_FOUND);
        PPRINT_CASE(PVRSRV_ERROR_BUFFER_DEVICE_NOT_FOUND);
        PPRINT_CASE(PVRSRV_ERROR_BUFFER_DEVICE_ALREADY_PRESENT);

        PPRINT_CASE(PVRSRV_ERROR_PCI_DEVICE_NOT_FOUND);
        PPRINT_CASE(PVRSRV_ERROR_PCI_CALL_FAILED);
        PPRINT_CASE(PVRSRV_ERROR_PCI_REGION_TOO_SMALL);
        PPRINT_CASE(PVRSRV_ERROR_PCI_REGION_UNAVAILABLE);
        PPRINT_CASE(PVRSRV_ERROR_BAD_REGION_SIZE_MISMATCH);

        PPRINT_CASE(PVRSRV_ERROR_REGISTER_BASE_NOT_SET);

        PPRINT_CASE(PVRSRV_ERROR_BM_BAD_SHAREMEM_HANDLE);

        PPRINT_CASE(PVRSRV_ERROR_FAILED_TO_ALLOC_USER_MEM);
        PPRINT_CASE(PVRSRV_ERROR_FAILED_TO_ALLOC_VP_MEMORY);
        PPRINT_CASE(PVRSRV_ERROR_FAILED_TO_MAP_SHARED_PBDESC);
        PPRINT_CASE(PVRSRV_ERROR_FAILED_TO_GET_PHYS_ADDR);

        PPRINT_CASE(PVRSRV_ERROR_FAILED_TO_ALLOC_VIRT_MEMORY);
        PPRINT_CASE(PVRSRV_ERROR_FAILED_TO_COPY_VIRT_MEMORY);

        PPRINT_CASE(PVRSRV_ERROR_FAILED_TO_ALLOC_PAGES);
        PPRINT_CASE(PVRSRV_ERROR_FAILED_TO_FREE_PAGES);
        PPRINT_CASE(PVRSRV_ERROR_FAILED_TO_COPY_PAGES);
        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_LOCK_PAGES);
        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_UNLOCK_PAGES);
        PPRINT_CASE(PVRSRV_ERROR_STILL_MAPPED);
        PPRINT_CASE(PVRSRV_ERROR_MAPPING_NOT_FOUND);
        PPRINT_CASE(PVRSRV_ERROR_PHYS_ADDRESS_EXCEEDS_32BIT);
        PPRINT_CASE(PVRSRV_ERROR_FAILED_TO_MAP_PAGE_TABLE);

        PPRINT_CASE(PVRSRV_ERROR_INVALID_SEGMENT_BLOCK);
        PPRINT_CASE(PVRSRV_ERROR_INVALID_SGXDEVDATA);
        PPRINT_CASE(PVRSRV_ERROR_INVALID_DEVINFO);
        PPRINT_CASE(PVRSRV_ERROR_INVALID_MEMINFO);
        PPRINT_CASE(PVRSRV_ERROR_INVALID_MISCINFO);
        PPRINT_CASE(PVRSRV_ERROR_UNKNOWN_IOCTL);
        PPRINT_CASE(PVRSRV_ERROR_INVALID_CONTEXT);
        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_DESTROY_CONTEXT);
        PPRINT_CASE(PVRSRV_ERROR_INVALID_HEAP);
        PPRINT_CASE(PVRSRV_ERROR_INVALID_KERNELINFO);
        PPRINT_CASE(PVRSRV_ERROR_UNKNOWN_POWER_STATE);
        PPRINT_CASE(PVRSRV_ERROR_INVALID_HANDLE_TYPE);
        PPRINT_CASE(PVRSRV_ERROR_INVALID_WRAP_TYPE);
        PPRINT_CASE(PVRSRV_ERROR_INVALID_PHYS_ADDR);
        PPRINT_CASE(PVRSRV_ERROR_INVALID_CPU_ADDR);
        PPRINT_CASE(PVRSRV_ERROR_INVALID_HEAPINFO);
        PPRINT_CASE(PVRSRV_ERROR_INVALID_PERPROC);
        PPRINT_CASE(PVRSRV_ERROR_FAILED_TO_RETRIEVE_HEAPINFO);
        PPRINT_CASE(PVRSRV_ERROR_INVALID_MAP_REQUEST);
        PPRINT_CASE(PVRSRV_ERROR_INVALID_UNMAP_REQUEST);
        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_FIND_MAPPING_HEAP);
        PPRINT_CASE(PVRSRV_ERROR_MAPPING_STILL_IN_USE);

        PPRINT_CASE(PVRSRV_ERROR_EXCEEDED_HW_LIMITS);
        PPRINT_CASE(PVRSRV_ERROR_NO_STAGING_BUFFER_ALLOCATED);

        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_CREATE_PERPROC_AREA);
        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_CREATE_EVENT);
        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_ENABLE_EVENT);
        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_REGISTER_EVENT);
        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_DESTROY_EVENT);
        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_CREATE_THREAD);
        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_CLOSE_THREAD);
        PPRINT_CASE(PVRSRV_ERROR_THREAD_READ_ERROR);
        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_REGISTER_ISR_HANDLER);
        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_INSTALL_ISR);
        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_UNINSTALL_ISR);
        PPRINT_CASE(PVRSRV_ERROR_ISR_ALREADY_INSTALLED);
        PPRINT_CASE(PVRSRV_ERROR_ISR_NOT_INSTALLED);
        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_INITIALISE_INTERRUPT);
        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_RETRIEVE_INFO);
        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_DO_BACKWARDS_BLIT);
        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_CLOSE_SERVICES);
        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_REGISTER_CONTEXT);
        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_REGISTER_RESOURCE);
        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_CLOSE_HANDLE);

        PPRINT_CASE(PVRSRV_ERROR_INVALID_CCB_COMMAND);

        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_LOCK_RESOURCE);
        PPRINT_CASE(PVRSRV_ERROR_INVALID_LOCK_ID);
        PPRINT_CASE(PVRSRV_ERROR_RESOURCE_NOT_LOCKED);

        PPRINT_CASE(PVRSRV_ERROR_FLIP_FAILED);
        PPRINT_CASE(PVRSRV_ERROR_UNBLANK_DISPLAY_FAILED);

        PPRINT_CASE(PVRSRV_ERROR_TIMEOUT_POLLING_FOR_VALUE);

        PPRINT_CASE(PVRSRV_ERROR_CREATE_RENDER_CONTEXT_FAILED);
        PPRINT_CASE(PVRSRV_ERROR_UNKNOWN_PRIMARY_FRAG);
        PPRINT_CASE(PVRSRV_ERROR_UNEXPECTED_SECONDARY_FRAG);
        PPRINT_CASE(PVRSRV_ERROR_UNEXPECTED_PRIMARY_FRAG);

        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_INSERT_FENCE_ID);

        PPRINT_CASE(PVRSRV_ERROR_BLIT_SETUP_FAILED);

        PPRINT_CASE(PVRSRV_ERROR_PDUMP_NOT_AVAILABLE);
        PPRINT_CASE(PVRSRV_ERROR_PDUMP_BUFFER_FULL);
        PPRINT_CASE(PVRSRV_ERROR_PDUMP_BUF_OVERFLOW);
        PPRINT_CASE(PVRSRV_ERROR_PDUMP_NOT_ACTIVE);
        PPRINT_CASE(PVRSRV_ERROR_INCOMPLETE_LINE_OVERLAPS_PAGES);

        PPRINT_CASE(PVRSRV_ERROR_MUTEX_DESTROY_FAILED);
        PPRINT_CASE(PVRSRV_ERROR_MUTEX_INTERRUPTIBLE_ERROR);

        PPRINT_CASE(PVRSRV_ERROR_INSUFFICIENT_SCRIPT_SPACE);
        PPRINT_CASE(PVRSRV_ERROR_INSUFFICIENT_SPACE_FOR_COMMAND);

        PPRINT_CASE(PVRSRV_ERROR_PROCESS_NOT_INITIALISED);
        PPRINT_CASE(PVRSRV_ERROR_PROCESS_NOT_FOUND);
        PPRINT_CASE(PVRSRV_ERROR_SRV_CONNECT_FAILED);
        PPRINT_CASE(PVRSRV_ERROR_SRV_DISCONNECT_FAILED);
        PPRINT_CASE(PVRSRV_ERROR_DEINT_PHASE_FAILED);
        PPRINT_CASE(PVRSRV_ERROR_INIT2_PHASE_FAILED);

        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_FIND_RESOURCE);

        PPRINT_CASE(PVRSRV_ERROR_NO_DC_DEVICES_FOUND);
        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_OPEN_DC_DEVICE);
        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_REMOVE_DEVICE);
        PPRINT_CASE(PVRSRV_ERROR_NO_DEVICEDATA_FOUND);
        PPRINT_CASE(PVRSRV_ERROR_NO_DEVICENODE_FOUND);
        PPRINT_CASE(PVRSRV_ERROR_NO_CLIENTNODE_FOUND);
        PPRINT_CASE(PVRSRV_ERROR_FAILED_TO_PROCESS_QUEUE);

        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_INIT_TASK);
        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_SCHEDULE_TASK);
        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_KILL_TASK);

        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_ENABLE_TIMER);
        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_DISABLE_TIMER);
        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_REMOVE_TIMER);

        PPRINT_CASE(PVRSRV_ERROR_UNKNOWN_PIXEL_FORMAT);
        PPRINT_CASE(PVRSRV_ERROR_UNKNOWN_SCRIPT_OPERATION);

        PPRINT_CASE(PVRSRV_ERROR_HANDLE_INDEX_OUT_OF_RANGE);
        PPRINT_CASE(PVRSRV_ERROR_HANDLE_NOT_ALLOCATED);
        PPRINT_CASE(PVRSRV_ERROR_HANDLE_TYPE_MISMATCH);
        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_ADD_HANDLE);
        PPRINT_CASE(PVRSRV_ERROR_HANDLE_NOT_SHAREABLE);
        PPRINT_CASE(PVRSRV_ERROR_HANDLE_NOT_FOUND);
        PPRINT_CASE(PVRSRV_ERROR_INVALID_SUBHANDLE);
        PPRINT_CASE(PVRSRV_ERROR_HANDLE_BATCH_IN_USE);
        PPRINT_CASE(PVRSRV_ERROR_HANDLE_BATCH_COMMIT_FAILURE);

        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_CREATE_HASH_TABLE);
        PPRINT_CASE(PVRSRV_ERROR_INSERT_HASH_TABLE_DATA_FAILED);

        PPRINT_CASE(PVRSRV_ERROR_UNSUPPORTED_BACKING_STORE);
        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_DESTROY_BM_HEAP);

        PPRINT_CASE(PVRSRV_ERROR_UNKNOWN_INIT_SERVER_STATE);

        PPRINT_CASE(PVRSRV_ERROR_NO_FREE_DEVICEIDS_AVALIABLE);
        PPRINT_CASE(PVRSRV_ERROR_INVALID_DEVICEID);
        PPRINT_CASE(PVRSRV_ERROR_DEVICEID_NOT_FOUND);

        PPRINT_CASE(PVRSRV_ERROR_MEMORY_TEST_FAILED);
        PPRINT_CASE(PVRSRV_ERROR_CPUPADDR_TEST_FAILED);
        PPRINT_CASE(PVRSRV_ERROR_COPY_TEST_FAILED);

        PPRINT_CASE(PVRSRV_ERROR_SEMAPHORE_NOT_INITIALISED);

        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_RELEASE_CLOCK);
        PPRINT_CASE(PVRSRV_ERROR_CLOCK_REQUEST_FAILED);
        PPRINT_CASE(PVRSRV_ERROR_DISABLE_CLOCK_FAILURE);
        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_SET_CLOCK_RATE);
        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_ROUND_CLOCK_RATE);
        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_ENABLE_CLOCK);
        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_GET_CLOCK);
        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_GET_PARENT_CLOCK);
        PPRINT_CASE(PVRSRV_ERROR_UNABLE_TO_GET_SYSTEM_CLOCK);

        PPRINT_CASE(PVRSRV_ERROR_UNKNOWN_SGL_ERROR);

        PPRINT_CASE(PVRSRV_ERROR_SYSTEM_POWER_CHANGE_FAILURE);
        PPRINT_CASE(PVRSRV_ERROR_DEVICE_POWER_CHANGE_FAILURE);

        PPRINT_CASE(PVRSRV_ERROR_BAD_SYNC_STATE);

        PPRINT_CASE(PVRSRV_ERROR_CACHEOP_FAILED);

        PPRINT_CASE(PVRSRV_ERROR_CACHE_INVALIDATE_FAILED);
        
        PPRINT_CASE(PVRSRV_ERROR_FORCE_I32);
         default:
            printf("Got value %d\n", data);
            assert(false && "Reached end of case\n");
            return NULL;           
    }
}

static const char *pprint_PVRSRV_DEVICE_TYPE(FILE *fp, PVRSRV_DEVICE_TYPE data) {
    UNUSED(fp);
    switch(data) {
        PPRINT_CASE(PVRSRV_DEVICE_TYPE_UNKNOWN);
        PPRINT_CASE(PVRSRV_DEVICE_TYPE_MBX1);
        PPRINT_CASE(PVRSRV_DEVICE_TYPE_MBX1_LITE);
        PPRINT_CASE(PVRSRV_DEVICE_TYPE_M24VA);
        PPRINT_CASE(PVRSRV_DEVICE_TYPE_MVDA2);
        PPRINT_CASE(PVRSRV_DEVICE_TYPE_MVED1);
        PPRINT_CASE(PVRSRV_DEVICE_TYPE_MSVDX);
        PPRINT_CASE(PVRSRV_DEVICE_TYPE_SGX);
        PPRINT_CASE(PVRSRV_DEVICE_TYPE_VGX);
        PPRINT_CASE(PVRSRV_DEVICE_TYPE_EXT);
        PPRINT_CASE(PVRSRV_DEVICE_TYPE_FORCE_I32);
        default:
            assert(false && "Reached end of case\n");
            return NULL;
    }
}

static const char *pprint_PVRSRV_DEVICE_CLASS(FILE *fp, PVRSRV_DEVICE_CLASS data) {
    UNUSED(fp);
    switch(data) {
        PPRINT_CASE(PVRSRV_DEVICE_CLASS_3D);
        PPRINT_CASE(PVRSRV_DEVICE_CLASS_DISPLAY);
        PPRINT_CASE(PVRSRV_DEVICE_CLASS_BUFFER);
        PPRINT_CASE(PVRSRV_DEVICE_CLASS_VIDEO);
        PPRINT_CASE(PVRSRV_DEVICE_CLASS_FORCE_I32);
        default:
            assert(false && "Reached end of case\n");
            return NULL;
    }
}

static const char *pprint_SGX_MISC_INFO_REQUEST(FILE *fp, SGX_MISC_INFO_REQUEST data) {
    UNUSED(fp);
    switch(data) {
        PPRINT_CASE(SGX_MISC_INFO_REQUEST_CLOCKSPEED);
        PPRINT_CASE(SGX_MISC_INFO_REQUEST_CLOCKSPEED_SLCSIZE);
        PPRINT_CASE(SGX_MISC_INFO_REQUEST_SGXREV);
        PPRINT_CASE(SGX_MISC_INFO_REQUEST_DRIVER_SGXREV);
        PPRINT_CASE(SGX_MISC_INFO_REQUEST_SET_HWPERF_STATUS);
        PPRINT_CASE(SGX_MISC_INFO_DUMP_DEBUG_INFO);
        PPRINT_CASE(SGX_MISC_INFO_DUMP_DEBUG_INFO_FORCE_REGS);
        PPRINT_CASE(SGX_MISC_INFO_PANIC);
        PPRINT_CASE(SGX_MISC_INFO_REQUEST_SPM);
        PPRINT_CASE(SGX_MISC_INFO_REQUEST_ACTIVEPOWER);
        PPRINT_CASE(SGX_MISC_INFO_REQUEST_LOCKUPS);
        PPRINT_CASE(SGX_MISC_INFO_REQUEST_FORCE_I16);
        default:
            assert(false && "Reached end of case\n");
            return NULL;
    }
}

static void pprint_PVRSRV_BRIDGE_PACKAGE(FILE *fp, PVRSRV_BRIDGE_PACKAGE *data) {
    fprintf(fp, "{\n.ui32BridgeID = 0x%x,\n,.ui32Size = 0x%x,\n.pvParamIn = %p,\n.ui32InBufferSize = 0x%x,\n.pvParamOut = %p,\n.ui32OutBufferSize = 0x%x,\n.hKernelServices = %p,\n}",
                data->ui32BridgeID, data->ui32Size, data->pvParamIn, data->ui32InBufferSize, data->pvParamOut, data->ui32OutBufferSize, data->hKernelServices);
}

static void pprint_PVRSRV_BRIDGE_IN_CONNECT_SERVICES(FILE *fp, PVRSRV_BRIDGE_IN_CONNECT_SERVICES *data) {
    fprintf(fp, "{\n.ui32BridgeFlags = 0x%x,\n.ui32Flags = 0x%x,\n}", data->ui32BridgeFlags, data->ui32Flags);
}

static void pprint_PVRSRV_BRIDGE_IN_ACQUIRE_DEVICEINFO(FILE *fp, PVRSRV_BRIDGE_IN_ACQUIRE_DEVICEINFO *data) {
    fprintf(fp, "{\n.ui32BridgeFlags = 0x%x,\n.uiDevIndex = 0x%x,\n.eDeviceType = %s,\n}",
                data->ui32BridgeFlags, data->uiDevIndex, PPRINT(fp, data->eDeviceType, PVRSRV_DEVICE_TYPE)); 
}

static void pprint_PVRSRV_BRIDGE_IN_SGXGETMISCINFO(FILE *fp, PVRSRV_BRIDGE_IN_SGXGETMISCINFO *data) {
    fprintf(fp, "{\n.ui32BridgeFlags = 0x%x,\n.hDevCookie = %p,\n.psMiscInfo = {\n\t.eRequest = %s\n},\n}",
                 data->ui32BridgeFlags, data->hDevCookie, PPRINT(fp, data->psMiscInfo->eRequest, SGX_MISC_INFO_REQUEST));
}

static void pprint_PVRSRV_BRIDGE_IN_CREATE_DEVMEMCONTEXT(FILE *fp, PVRSRV_BRIDGE_IN_CREATE_DEVMEMCONTEXT *data) {
    fprintf(fp, "{\n.ui32BridgeFlags = 0x%x,\n.hDevCookie = %p,\n}\n}",
                 data->ui32BridgeFlags, data->hDevCookie);
}
static void pprint_PVRSRV_BRIDGE_IN_GETCLIENTINFO(FILE *fp, PVRSRV_BRIDGE_IN_GETCLIENTINFO *data) {
    fprintf(fp, "{\n.ui32BridgeFlags = 0x%x,\n.hDevCookie = %p,\n}",
                 data->ui32BridgeFlags, data->hDevCookie);
}

static void pprint_PVRSRV_BRIDGE_OUT_CONNECT_SERVICES(FILE *fp, PVRSRV_BRIDGE_OUT_CONNECT_SERVICES *data) {
    fprintf(fp, "{\n.eError = %s,\n.hKernelServices = %p,\n}",
                PPRINT(fp, data->eError, PVRSRV_ERROR), data->hKernelServices);
}

static void pprint_PVRSRV_BRIDGE_OUT_ENUMDEVICE(FILE *fp, PVRSRV_BRIDGE_OUT_ENUMDEVICE *data) {
    fprintf(fp, "{\n.eError = %s,\n.ui32NumDevices = %d,\n.asDeviceIdentifier = {\n",
                PPRINT(fp, data->eError, PVRSRV_ERROR), data->ui32NumDevices);
    for(uint32_t i = 0; i < data->ui32NumDevices; i++) {
        PVRSRV_DEVICE_IDENTIFIER *di = &data->asDeviceIdentifier[i];
        fprintf(fp, "{\n.eDeviceType = %s,\n.eDeviceClass = %s,\n.ui32DeviceIndex = %d,\n.pszPDumpDevName = %s,\n.pszPDumpRegName = %s\n},",
                     PPRINT(fp, di->eDeviceType, PVRSRV_DEVICE_TYPE),
                     PPRINT(fp, di->eDeviceClass, PVRSRV_DEVICE_CLASS),
                     di->ui32DeviceIndex,
                     di->pszPDumpDevName,
                     di->pszPDumpRegName);
    }
    fprintf(fp,"\n}");
}

static void pprint_PVRSRV_BRIDGE_OUT_ACQUIRE_DEVICEINFO(FILE *fp, PVRSRV_BRIDGE_OUT_ACQUIRE_DEVICEINFO *data) {
    fprintf(fp, "{\n.eError = %s,\n.hDevCookie = %p,\n}",
                PPRINT(fp, data->eError, PVRSRV_ERROR), data->hDevCookie);
}
