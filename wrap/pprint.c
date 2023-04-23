#define PPRINT(fp, data, type) pprint_##type(fp, data)

#define PPRINT_CASE(name) \
    case name: \
        return #name

#define UNUSED(x) (void)x

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
    fprintf(fp, "{\n.ui32BridgeFlags = 0x%x,\n.hDevCookie = %p,\n}\n}",
                 data->ui32BridgeFlags, data->hDevCookie);
}
