#ifndef PVR_TYPES_H
#define PVR_TYPES_H
#include <stdint.h>

#define SUPPORT_SGX_NEW_STATUS_VALS
#define SGX_MAX_TA_STATUS_VALS	32
#define SGX_MAX_3D_STATUS_VALS	4
/* sync info structure array size */
#define SGX_MAX_SRC_SYNCS_TA				32
#define SGX_MAX_DST_SYNCS_TA				1
/* note: only one dst sync is supported by the 2D paths */
#define SGX_MAX_SRC_SYNCS_TQ				6
#define SGX_MAX_DST_SYNCS_TQ				2

#define PVRSRV_MAX_DEVICES		16	/*!< Largest supported number of devices on the system */


typedef uint32_t IMG_UINT32;
typedef uint64_t IMG_UINT64;
typedef void IMG_VOID;
typedef void *IMG_PVOID;
typedef void *IMG_HANDLE;
typedef size_t IMG_SIZE_T;
typedef uintptr_t IMG_UINTPTR_T;
typedef char IMG_CHAR;

typedef	enum tag_img_bool
{
	IMG_FALSE		= 0,
	IMG_TRUE		= 1,
	IMG_FORCE_ALIGN = 0x7FFFFFFF
} IMG_BOOL, *IMG_PBOOL;

typedef enum _PVRSRV_ERROR_
{
	PVRSRV_OK = 0,
	PVRSRV_ERROR_OUT_OF_MEMORY,
	PVRSRV_ERROR_TOO_FEW_BUFFERS,
	PVRSRV_ERROR_INVALID_PARAMS,
	PVRSRV_ERROR_INIT_FAILURE,
	PVRSRV_ERROR_CANT_REGISTER_CALLBACK,
	PVRSRV_ERROR_INVALID_DEVICE,
	PVRSRV_ERROR_NOT_OWNER,
	PVRSRV_ERROR_BAD_MAPPING,
	PVRSRV_ERROR_TIMEOUT,
	PVRSRV_ERROR_FLIP_CHAIN_EXISTS,
	PVRSRV_ERROR_INVALID_SWAPINTERVAL,
	PVRSRV_ERROR_SCENE_INVALID,
	PVRSRV_ERROR_STREAM_ERROR,
	PVRSRV_ERROR_FAILED_DEPENDENCIES,
	PVRSRV_ERROR_CMD_NOT_PROCESSED,
	PVRSRV_ERROR_CMD_TOO_BIG,
	PVRSRV_ERROR_DEVICE_REGISTER_FAILED,
	PVRSRV_ERROR_TOOMANYBUFFERS,
	PVRSRV_ERROR_NOT_SUPPORTED,
	PVRSRV_ERROR_PROCESSING_BLOCKED,

	PVRSRV_ERROR_CANNOT_FLUSH_QUEUE,
	PVRSRV_ERROR_CANNOT_GET_QUEUE_SPACE,
	PVRSRV_ERROR_CANNOT_GET_RENDERDETAILS,
	PVRSRV_ERROR_RETRY,

	PVRSRV_ERROR_DDK_VERSION_MISMATCH,
	PVRSRV_ERROR_BUILD_MISMATCH,
	PVRSRV_ERROR_CORE_REVISION_MISMATCH,

	PVRSRV_ERROR_UPLOAD_TOO_BIG,

	PVRSRV_ERROR_INVALID_FLAGS,
	PVRSRV_ERROR_FAILED_TO_REGISTER_PROCESS,

	PVRSRV_ERROR_UNABLE_TO_LOAD_LIBRARY,
	PVRSRV_ERROR_UNABLE_GET_FUNC_ADDR,
	PVRSRV_ERROR_UNLOAD_LIBRARY_FAILED,

	PVRSRV_ERROR_BRIDGE_CALL_FAILED,
	PVRSRV_ERROR_IOCTL_CALL_FAILED,

    PVRSRV_ERROR_MMU_CONTEXT_NOT_FOUND,
	PVRSRV_ERROR_BUFFER_DEVICE_NOT_FOUND,
	PVRSRV_ERROR_BUFFER_DEVICE_ALREADY_PRESENT,

	PVRSRV_ERROR_PCI_DEVICE_NOT_FOUND,
	PVRSRV_ERROR_PCI_CALL_FAILED,
	PVRSRV_ERROR_PCI_REGION_TOO_SMALL,
	PVRSRV_ERROR_PCI_REGION_UNAVAILABLE,
	PVRSRV_ERROR_BAD_REGION_SIZE_MISMATCH,

	PVRSRV_ERROR_REGISTER_BASE_NOT_SET,

    PVRSRV_ERROR_BM_BAD_SHAREMEM_HANDLE,

	PVRSRV_ERROR_FAILED_TO_ALLOC_USER_MEM,
	PVRSRV_ERROR_FAILED_TO_ALLOC_VP_MEMORY,
	PVRSRV_ERROR_FAILED_TO_MAP_SHARED_PBDESC,
	PVRSRV_ERROR_FAILED_TO_GET_PHYS_ADDR,

	PVRSRV_ERROR_FAILED_TO_ALLOC_VIRT_MEMORY,
	PVRSRV_ERROR_FAILED_TO_COPY_VIRT_MEMORY,

	PVRSRV_ERROR_FAILED_TO_ALLOC_PAGES,
	PVRSRV_ERROR_FAILED_TO_FREE_PAGES,
	PVRSRV_ERROR_FAILED_TO_COPY_PAGES,
	PVRSRV_ERROR_UNABLE_TO_LOCK_PAGES,
	PVRSRV_ERROR_UNABLE_TO_UNLOCK_PAGES,
	PVRSRV_ERROR_STILL_MAPPED,
	PVRSRV_ERROR_MAPPING_NOT_FOUND,
	PVRSRV_ERROR_PHYS_ADDRESS_EXCEEDS_32BIT,
	PVRSRV_ERROR_FAILED_TO_MAP_PAGE_TABLE,

	PVRSRV_ERROR_INVALID_SEGMENT_BLOCK,
	PVRSRV_ERROR_INVALID_SGXDEVDATA,
	PVRSRV_ERROR_INVALID_DEVINFO,
	PVRSRV_ERROR_INVALID_MEMINFO,
	PVRSRV_ERROR_INVALID_MISCINFO,
	PVRSRV_ERROR_UNKNOWN_IOCTL,
	PVRSRV_ERROR_INVALID_CONTEXT,
	PVRSRV_ERROR_UNABLE_TO_DESTROY_CONTEXT,
	PVRSRV_ERROR_INVALID_HEAP,
	PVRSRV_ERROR_INVALID_KERNELINFO,
	PVRSRV_ERROR_UNKNOWN_POWER_STATE,
	PVRSRV_ERROR_INVALID_HANDLE_TYPE,
	PVRSRV_ERROR_INVALID_WRAP_TYPE,
	PVRSRV_ERROR_INVALID_PHYS_ADDR,
	PVRSRV_ERROR_INVALID_CPU_ADDR,
	PVRSRV_ERROR_INVALID_HEAPINFO,
	PVRSRV_ERROR_INVALID_PERPROC,
	PVRSRV_ERROR_FAILED_TO_RETRIEVE_HEAPINFO,
	PVRSRV_ERROR_INVALID_MAP_REQUEST,
	PVRSRV_ERROR_INVALID_UNMAP_REQUEST,
	PVRSRV_ERROR_UNABLE_TO_FIND_MAPPING_HEAP,
	PVRSRV_ERROR_MAPPING_STILL_IN_USE,

	PVRSRV_ERROR_EXCEEDED_HW_LIMITS,
	PVRSRV_ERROR_NO_STAGING_BUFFER_ALLOCATED,

	PVRSRV_ERROR_UNABLE_TO_CREATE_PERPROC_AREA,
	PVRSRV_ERROR_UNABLE_TO_CREATE_EVENT,
	PVRSRV_ERROR_UNABLE_TO_ENABLE_EVENT,
	PVRSRV_ERROR_UNABLE_TO_REGISTER_EVENT,
	PVRSRV_ERROR_UNABLE_TO_DESTROY_EVENT,
	PVRSRV_ERROR_UNABLE_TO_CREATE_THREAD,
	PVRSRV_ERROR_UNABLE_TO_CLOSE_THREAD,
	PVRSRV_ERROR_THREAD_READ_ERROR,
	PVRSRV_ERROR_UNABLE_TO_REGISTER_ISR_HANDLER,
	PVRSRV_ERROR_UNABLE_TO_INSTALL_ISR,
	PVRSRV_ERROR_UNABLE_TO_UNINSTALL_ISR,
	PVRSRV_ERROR_ISR_ALREADY_INSTALLED,
	PVRSRV_ERROR_ISR_NOT_INSTALLED,
	PVRSRV_ERROR_UNABLE_TO_INITIALISE_INTERRUPT,
	PVRSRV_ERROR_UNABLE_TO_RETRIEVE_INFO,
	PVRSRV_ERROR_UNABLE_TO_DO_BACKWARDS_BLIT,
	PVRSRV_ERROR_UNABLE_TO_CLOSE_SERVICES,
	PVRSRV_ERROR_UNABLE_TO_REGISTER_CONTEXT,
	PVRSRV_ERROR_UNABLE_TO_REGISTER_RESOURCE,
	PVRSRV_ERROR_UNABLE_TO_CLOSE_HANDLE,

	PVRSRV_ERROR_INVALID_CCB_COMMAND,

	PVRSRV_ERROR_UNABLE_TO_LOCK_RESOURCE,
	PVRSRV_ERROR_INVALID_LOCK_ID,
	PVRSRV_ERROR_RESOURCE_NOT_LOCKED,

	PVRSRV_ERROR_FLIP_FAILED,
	PVRSRV_ERROR_UNBLANK_DISPLAY_FAILED,

	PVRSRV_ERROR_TIMEOUT_POLLING_FOR_VALUE,

	PVRSRV_ERROR_CREATE_RENDER_CONTEXT_FAILED,
	PVRSRV_ERROR_UNKNOWN_PRIMARY_FRAG,
	PVRSRV_ERROR_UNEXPECTED_SECONDARY_FRAG,
	PVRSRV_ERROR_UNEXPECTED_PRIMARY_FRAG,

	PVRSRV_ERROR_UNABLE_TO_INSERT_FENCE_ID,

	PVRSRV_ERROR_BLIT_SETUP_FAILED,

	PVRSRV_ERROR_PDUMP_NOT_AVAILABLE,
	PVRSRV_ERROR_PDUMP_BUFFER_FULL,
	PVRSRV_ERROR_PDUMP_BUF_OVERFLOW,
	PVRSRV_ERROR_PDUMP_NOT_ACTIVE,
	PVRSRV_ERROR_INCOMPLETE_LINE_OVERLAPS_PAGES,

	PVRSRV_ERROR_MUTEX_DESTROY_FAILED,
	PVRSRV_ERROR_MUTEX_INTERRUPTIBLE_ERROR,

	PVRSRV_ERROR_INSUFFICIENT_SCRIPT_SPACE,
	PVRSRV_ERROR_INSUFFICIENT_SPACE_FOR_COMMAND,

	PVRSRV_ERROR_PROCESS_NOT_INITIALISED,
	PVRSRV_ERROR_PROCESS_NOT_FOUND,
	PVRSRV_ERROR_SRV_CONNECT_FAILED,
	PVRSRV_ERROR_SRV_DISCONNECT_FAILED,
	PVRSRV_ERROR_DEINT_PHASE_FAILED,
	PVRSRV_ERROR_INIT2_PHASE_FAILED,

	PVRSRV_ERROR_UNABLE_TO_FIND_RESOURCE,

	PVRSRV_ERROR_NO_DC_DEVICES_FOUND,
	PVRSRV_ERROR_UNABLE_TO_OPEN_DC_DEVICE,
	PVRSRV_ERROR_UNABLE_TO_REMOVE_DEVICE,
	PVRSRV_ERROR_NO_DEVICEDATA_FOUND,
	PVRSRV_ERROR_NO_DEVICENODE_FOUND,
	PVRSRV_ERROR_NO_CLIENTNODE_FOUND,
	PVRSRV_ERROR_FAILED_TO_PROCESS_QUEUE,

	PVRSRV_ERROR_UNABLE_TO_INIT_TASK,
	PVRSRV_ERROR_UNABLE_TO_SCHEDULE_TASK,
	PVRSRV_ERROR_UNABLE_TO_KILL_TASK,

	PVRSRV_ERROR_UNABLE_TO_ENABLE_TIMER,
	PVRSRV_ERROR_UNABLE_TO_DISABLE_TIMER,
	PVRSRV_ERROR_UNABLE_TO_REMOVE_TIMER,

	PVRSRV_ERROR_UNKNOWN_PIXEL_FORMAT,
	PVRSRV_ERROR_UNKNOWN_SCRIPT_OPERATION,

	PVRSRV_ERROR_HANDLE_INDEX_OUT_OF_RANGE,
	PVRSRV_ERROR_HANDLE_NOT_ALLOCATED,
	PVRSRV_ERROR_HANDLE_TYPE_MISMATCH,
	PVRSRV_ERROR_UNABLE_TO_ADD_HANDLE,
	PVRSRV_ERROR_HANDLE_NOT_SHAREABLE,
	PVRSRV_ERROR_HANDLE_NOT_FOUND,
	PVRSRV_ERROR_INVALID_SUBHANDLE,
	PVRSRV_ERROR_HANDLE_BATCH_IN_USE,
	PVRSRV_ERROR_HANDLE_BATCH_COMMIT_FAILURE,

	PVRSRV_ERROR_UNABLE_TO_CREATE_HASH_TABLE,
	PVRSRV_ERROR_INSERT_HASH_TABLE_DATA_FAILED,

	PVRSRV_ERROR_UNSUPPORTED_BACKING_STORE,
	PVRSRV_ERROR_UNABLE_TO_DESTROY_BM_HEAP,

	PVRSRV_ERROR_UNKNOWN_INIT_SERVER_STATE,

	PVRSRV_ERROR_NO_FREE_DEVICEIDS_AVALIABLE,
	PVRSRV_ERROR_INVALID_DEVICEID,
	PVRSRV_ERROR_DEVICEID_NOT_FOUND,

	PVRSRV_ERROR_MEMORY_TEST_FAILED,
	PVRSRV_ERROR_CPUPADDR_TEST_FAILED,
	PVRSRV_ERROR_COPY_TEST_FAILED,

	PVRSRV_ERROR_SEMAPHORE_NOT_INITIALISED,

	PVRSRV_ERROR_UNABLE_TO_RELEASE_CLOCK,
	PVRSRV_ERROR_CLOCK_REQUEST_FAILED,
	PVRSRV_ERROR_DISABLE_CLOCK_FAILURE,
	PVRSRV_ERROR_UNABLE_TO_SET_CLOCK_RATE,
	PVRSRV_ERROR_UNABLE_TO_ROUND_CLOCK_RATE,
	PVRSRV_ERROR_UNABLE_TO_ENABLE_CLOCK,
	PVRSRV_ERROR_UNABLE_TO_GET_CLOCK,
	PVRSRV_ERROR_UNABLE_TO_GET_PARENT_CLOCK,
	PVRSRV_ERROR_UNABLE_TO_GET_SYSTEM_CLOCK,

	PVRSRV_ERROR_UNKNOWN_SGL_ERROR,

	PVRSRV_ERROR_SYSTEM_POWER_CHANGE_FAILURE,
	PVRSRV_ERROR_DEVICE_POWER_CHANGE_FAILURE,

	PVRSRV_ERROR_BAD_SYNC_STATE,

	PVRSRV_ERROR_CACHEOP_FAILED,

	PVRSRV_ERROR_CACHE_INVALIDATE_FAILED,

	PVRSRV_ERROR_FORCE_I32 = 0x7fffffff

} PVRSRV_ERROR;

typedef enum _PVRSRV_DEVICE_TYPE_
{
	PVRSRV_DEVICE_TYPE_UNKNOWN			= 0 ,
	PVRSRV_DEVICE_TYPE_MBX1				= 1 ,
	PVRSRV_DEVICE_TYPE_MBX1_LITE		= 2 ,

	PVRSRV_DEVICE_TYPE_M24VA			= 3,
	PVRSRV_DEVICE_TYPE_MVDA2			= 4,
	PVRSRV_DEVICE_TYPE_MVED1			= 5,
	PVRSRV_DEVICE_TYPE_MSVDX			= 6,

	PVRSRV_DEVICE_TYPE_SGX				= 7,

	PVRSRV_DEVICE_TYPE_VGX				= 8,

	/* 3rd party devices take ext type */
	PVRSRV_DEVICE_TYPE_EXT				= 9,

    PVRSRV_DEVICE_TYPE_LAST             = 9,

	PVRSRV_DEVICE_TYPE_FORCE_I32		= 0x7fffffff

} PVRSRV_DEVICE_TYPE;

typedef enum _PVRSRV_DEVICE_CLASS_
{
	PVRSRV_DEVICE_CLASS_3D				= 0 ,
	PVRSRV_DEVICE_CLASS_DISPLAY			= 1 ,
	PVRSRV_DEVICE_CLASS_BUFFER			= 2 ,
	PVRSRV_DEVICE_CLASS_VIDEO			= 3 ,

	PVRSRV_DEVICE_CLASS_FORCE_I32 		= 0x7fffffff

} PVRSRV_DEVICE_CLASS;

typedef enum _PVRSRV_MEMTYPE_
{
	PVRSRV_MEMTYPE_UNKNOWN		= 0,
	PVRSRV_MEMTYPE_DEVICE		= 1,
	PVRSRV_MEMTYPE_DEVICECLASS	= 2,
	PVRSRV_MEMTYPE_WRAPPED		= 3,
	PVRSRV_MEMTYPE_MAPPED		= 4,
	PVRSRV_MEMTYPE_ION			= 5,
	PVRSRV_MEMTYPE_ALLOC		= 6,
	PVRSRV_MEMTYPE_FREE	    	= 7
} PVRSRV_MEMTYPE;

/* device virtual address */
typedef struct _IMG_DEV_VIRTADDR
{
	/* device virtual addresses are 32bit for now */
	IMG_UINT32  uiAddr;
#define IMG_CAST_TO_DEVVADDR_UINT(var)		(IMG_UINT32)(var)
	
} IMG_DEV_VIRTADDR;

/* system physical address */
typedef struct _IMG_SYS_PHYADDR
{
	/* variable sized type (32,64) */
#if IMG_ADDRSPACE_PHYSADDR_BITS == 32
	/* variable sized type (32,64) */
	IMG_UINT32 uiAddr;
#else
	IMG_UINT64 uiAddr;
#endif
} IMG_SYS_PHYADDR;

typedef struct _PVRSRV_DEVICE_IDENTIFIER_
{
	PVRSRV_DEVICE_TYPE		eDeviceType;		/*!< Identifies the type of the device */
	PVRSRV_DEVICE_CLASS		eDeviceClass;		/*!< Identifies more general class of device - display/3d/mpeg etc */
	IMG_UINT32				ui32DeviceIndex;	/*!< Index of the device within the system */
	IMG_CHAR				*pszPDumpDevName;	/*!< Pdump memory bank name */
	IMG_CHAR				*pszPDumpRegName;	/*!< Pdump register bank name */

} PVRSRV_DEVICE_IDENTIFIER;

typedef struct _PVRSRV_SYNC_DATA_
{
	/* CPU accessible WriteOp Info */
	IMG_UINT32					ui32WriteOpsPending;
	volatile IMG_UINT32			ui32WriteOpsComplete;

	/* CPU accessible ReadOp Info */
	IMG_UINT32					ui32ReadOpsPending;
	volatile IMG_UINT32			ui32ReadOpsComplete;

	/* CPU accessible ReadOp2 Info */
	IMG_UINT32					ui32ReadOps2Pending;
	volatile IMG_UINT32			ui32ReadOps2Complete;

	/* pdump specific value */
	IMG_UINT32					ui32LastOpDumpVal;
	IMG_UINT32					ui32LastReadOpDumpVal;

	/* Last write oprtation on this sync */
	IMG_UINT64					ui64LastWrite;

} PVRSRV_SYNC_DATA;

/*
	Client Sync Info structure
*/
typedef struct _PVRSRV_CLIENT_SYNC_INFO_
{
	/* mapping of the kernel sync data */
	PVRSRV_SYNC_DATA		*psSyncData;

	/* Device accessible WriteOp Info */
	IMG_DEV_VIRTADDR		sWriteOpsCompleteDevVAddr;

	/* Device accessible ReadOp Info */
	IMG_DEV_VIRTADDR		sReadOpsCompleteDevVAddr;

	/* Device accessible ReadOp2 Info */
	IMG_DEV_VIRTADDR		sReadOps2CompleteDevVAddr;

	/* handle to client mapping data (OS specific) */
	IMG_HANDLE					hMappingInfo;

	/* handle to kernel sync info */
	IMG_HANDLE					hKernelSyncInfo;

} PVRSRV_CLIENT_SYNC_INFO, *PPVRSRV_CLIENT_SYNC_INFO;

typedef struct _PVRSRV_MEMBLK_
{
	IMG_DEV_VIRTADDR	sDevVirtAddr;			/*!< Address of the memory in the IMG MMUs address space */
	IMG_HANDLE			hOSMemHandle;			/*!< Stores the underlying memory allocation handle */
	IMG_HANDLE			hOSWrapMem;				/*!< FIXME: better way to solve this problem */
	IMG_HANDLE			hBuffer;				/*!< Stores the BM_HANDLE for the underlying memory management */
	IMG_HANDLE			hResItem;				/*!< handle to resource item for allocate */
	IMG_SYS_PHYADDR	 	*psIntSysPAddr;

} PVRSRV_MEMBLK;

typedef struct _PVRSRV_KERNEL_MEM_INFO_
{
	/* Kernel Mode CPU Virtual address */
	IMG_PVOID				pvLinAddrKM;

	/* Device Virtual Address */
	IMG_DEV_VIRTADDR		sDevVAddr;

	/* allocation flags */
	IMG_UINT32				ui32Flags;

	/* Size of the allocated buffer in bytes */
	IMG_SIZE_T				uAllocSize;

	/* Internal implementation details. Do not use outside services code. */
	PVRSRV_MEMBLK			sMemBlk;

	/* Address of the backup buffer used in a save/restore of the surface */
	IMG_PVOID				pvSysBackupBuffer;

	/* refcount for allocation, wrapping and mapping */
	IMG_UINT32				ui32RefCount;

	/* Set when free call ocured and a mapping was still open */
	IMG_BOOL				bPendingFree;


#if defined(SUPPORT_MEMINFO_IDS)
	#if !defined(USE_CODE)
	/* Globally unique "stamp" for allocation (not re-used until wrap) */
	IMG_UINT64				ui64Stamp;
	#else /* !defined(USE_CODE) */
	IMG_UINT32				dummy1;
	IMG_UINT32				dummy2;
	#endif /* !defined(USE_CODE) */
#endif /* defined(SUPPORT_MEMINFO_IDS) */

	/* ptr to associated kernel sync info - NULL if no sync */
	struct _PVRSRV_KERNEL_SYNC_INFO_	*psKernelSyncInfo;

	IMG_HANDLE				hIonSyncInfo;

	PVRSRV_MEMTYPE				memType;

    /*
      To activate the "share mem workaround", add PVRSRV_MEM_XPROC to
      the flags for the allocation.  This will cause the "map" API to
      call use Alloc Device Mem but will share the underlying memory
      block and sync data.
    */
	struct {
        /* Record whether the workaround is active for this
           allocation.  The rest of the fields in this struct are
           undefined unless this is true */
		IMG_BOOL bInUse;

        /* Store the device cookie handle from the original
           allocation, as it is not present on the "Map" API. */
		IMG_HANDLE hDevCookieInt;

        /* This is an index into a static array which store
           information about the underlying allocation */
		IMG_UINT32 ui32ShareIndex;

        /* Original arguments as supplied to original
           "PVRSRVAllocDeviceMem" call, such that a new call to this
           function can be later constructed */
		IMG_UINT32 ui32OrigReqAttribs;
		IMG_UINT32 ui32OrigReqSize;
		IMG_UINT32 ui32OrigReqAlignment;
	} sShareMemWorkaround;
#if defined (MEM_TRACK_INFO_DEBUG)
	IMG_CHAR heapId[128];
#endif
} PVRSRV_KERNEL_MEM_INFO;



typedef struct _PVRSRV_CLIENT_MEM_INFO_
{
	/* CPU Virtual Address */
	IMG_PVOID				pvLinAddr;

	/* CPU Virtual Address (for kernel mode) */
	IMG_PVOID				pvLinAddrKM;

	/* Device Virtual Address */
	IMG_DEV_VIRTADDR		sDevVAddr;

	/* allocation flags */
	IMG_UINT32				ui32Flags;

	/* client allocation flags */
	IMG_UINT32				ui32ClientFlags;

	/* allocation size in bytes */
	IMG_SIZE_T				uAllocSize;


	/* ptr to associated client sync info - NULL if no sync */
	struct _PVRSRV_CLIENT_SYNC_INFO_	*psClientSyncInfo;

	/* handle to client mapping data (OS specific) */
	IMG_HANDLE							hMappingInfo;

	/* handle to kernel mem info */
	IMG_HANDLE							hKernelMemInfo;

	/* resman handle for UM mapping clean-up */
	IMG_HANDLE							hResItem;

#if defined(SUPPORT_MEMINFO_IDS)
	#if !defined(USE_CODE)
	/* Globally unique "stamp" for allocation (not re-used until wrap) */
	IMG_UINT64							ui64Stamp;
	#else /* !defined(USE_CODE) */
	IMG_UINT32							dummy1;
	IMG_UINT32							dummy2;
	#endif /* !defined(USE_CODE) */
#endif /* defined(SUPPORT_MEMINFO_IDS) */
#if defined(SUPPORT_ION)
	IMG_SIZE_T							uiIonBufferSize;
#endif /* defined(SUPPORT_ION) */

	/*
		ptr to next mem info
		D3D uses psNext for mid-scene texture reload.
	*/
	struct _PVRSRV_CLIENT_MEM_INFO_		*psNext;

} PVRSRV_CLIENT_MEM_INFO, *PPVRSRV_CLIENT_MEM_INFO;



typedef struct PVRSRV_BRIDGE_PACKAGE_TAG
{
	IMG_UINT32				ui32BridgeID;			/*!< ioctl/drvesc index */
	IMG_UINT32				ui32Size;				/*!< size of structure */
	IMG_VOID				*pvParamIn;				/*!< input data buffer */
	IMG_UINT32				ui32InBufferSize;		/*!< size of input data buffer */
	IMG_VOID				*pvParamOut;			/*!< output data buffer */
	IMG_UINT32				ui32OutBufferSize;		/*!< size of output data buffer */

	IMG_HANDLE				hKernelServices;		/*!< kernel servcies handle */
} PVRSRV_BRIDGE_PACKAGE;

typedef struct PVRSRV_BRIDGE_OUT_MAP_DEVICECLASS_MEMORY_TAG
{
	PVRSRV_ERROR            eError;
	PVRSRV_CLIENT_MEM_INFO  sClientMemInfo;
	PVRSRV_CLIENT_SYNC_INFO sClientSyncInfo;
	PVRSRV_KERNEL_MEM_INFO	*psKernelMemInfo;
	IMG_HANDLE				hMappingInfo;

} PVRSRV_BRIDGE_OUT_MAP_DEVICECLASS_MEMORY;

typedef struct PVRSRV_BRIDGE_OUT_ALLOCDEVICEMEM_TAG
{
	PVRSRV_ERROR            eError;
	PVRSRV_KERNEL_MEM_INFO	*psKernelMemInfo;
	PVRSRV_CLIENT_MEM_INFO  sClientMemInfo;
	PVRSRV_CLIENT_SYNC_INFO sClientSyncInfo;

} PVRSRV_BRIDGE_OUT_ALLOCDEVICEMEM;

typedef struct PVRSRV_BRIDGE_IN_MHANDLE_TO_MMAP_DATA_TAG
{
	IMG_UINT32			ui32BridgeFlags; /* Must be first member of structure */
	IMG_HANDLE			hMHandle;	 /* Handle associated with the memory that needs to be mapped */
} PVRSRV_BRIDGE_IN_MHANDLE_TO_MMAP_DATA;

typedef struct PVRSRV_BRIDGE_OUT_MHANDLE_TO_MMAP_DATA_TAG
{
    PVRSRV_ERROR		eError;

    /* This is a the offset you should pass to mmap(2) so that
     * the driver can look up the full details for the mapping
     * request. */
     IMG_UINTPTR_T		uiMMapOffset;

    /* This is the byte offset you should add to the mapping you
     * get from mmap */
    IMG_UINTPTR_T		uiByteOffset;

    /* This is the real size of the mapping that will be created
     * which should be passed to mmap _and_ munmap. */
    IMG_SIZE_T 			uiRealByteSize;

    /* User mode address associated with mapping */
    IMG_UINTPTR_T       uiUserVAddr;

} PVRSRV_BRIDGE_OUT_MHANDLE_TO_MMAP_DATA;

typedef struct PVRSRV_BRIDGE_OUT_MAP_DEV_MEMORY_TAG
{
	PVRSRV_ERROR            eError;
	PVRSRV_KERNEL_MEM_INFO	*psDstKernelMemInfo;
	PVRSRV_CLIENT_MEM_INFO  sDstClientMemInfo;
	PVRSRV_CLIENT_SYNC_INFO sDstClientSyncInfo;

}PVRSRV_BRIDGE_OUT_MAP_DEV_MEMORY;

typedef enum _SGX_MISC_INFO_REQUEST_
{
	SGX_MISC_INFO_REQUEST_CLOCKSPEED = 0,
	SGX_MISC_INFO_REQUEST_CLOCKSPEED_SLCSIZE,
	SGX_MISC_INFO_REQUEST_SGXREV,
	SGX_MISC_INFO_REQUEST_DRIVER_SGXREV,
#if defined(SUPPORT_SGX_EDM_MEMORY_DEBUG)
	SGX_MISC_INFO_REQUEST_MEMREAD,
	SGX_MISC_INFO_REQUEST_MEMCOPY,
#endif /* SUPPORT_SGX_EDM_MEMORY_DEBUG */
	SGX_MISC_INFO_REQUEST_SET_HWPERF_STATUS,
#if defined(SGX_FEATURE_DATA_BREAKPOINTS)
	SGX_MISC_INFO_REQUEST_SET_BREAKPOINT,
	SGX_MISC_INFO_REQUEST_POLL_BREAKPOINT,
	SGX_MISC_INFO_REQUEST_RESUME_BREAKPOINT,
#endif /* SGX_FEATURE_DATA_BREAKPOINTS */
	SGX_MISC_INFO_DUMP_DEBUG_INFO,
	SGX_MISC_INFO_DUMP_DEBUG_INFO_FORCE_REGS,
	SGX_MISC_INFO_PANIC,
	SGX_MISC_INFO_REQUEST_SPM,
	SGX_MISC_INFO_REQUEST_ACTIVEPOWER,
	SGX_MISC_INFO_REQUEST_LOCKUPS,
#if defined(PVRSRV_USSE_EDM_STATUS_DEBUG)
	SGX_MISC_INFO_REQUEST_EDM_STATUS_BUFFER_INFO,
#endif
	SGX_MISC_INFO_REQUEST_FORCE_I16 				=  0x7fff
} SGX_MISC_INFO_REQUEST;

typedef enum _SGXMKIF_CMD_TYPE_
{
	SGXMKIF_CMD_TA				= 0,
	SGXMKIF_CMD_TRANSFER		= 1,
	SGXMKIF_CMD_2D				= 2,
	SGXMKIF_CMD_POWER			= 3,
	SGXMKIF_CMD_CONTEXTSUSPEND	= 4,
	SGXMKIF_CMD_CLEANUP			= 5,
	SGXMKIF_CMD_GETMISCINFO		= 6,
	SGXMKIF_CMD_PROCESS_QUEUES	= 7,
	SGXMKIF_CMD_DATABREAKPOINT	= 8,
	SGXMKIF_CMD_SETHWPERFSTATUS	= 9,
 	SGXMKIF_CMD_FLUSHPDCACHE	= 10,
 	SGXMKIF_CMD_MAX				= 11,

	SGXMKIF_CMD_FORCE_I32   	= -1,

} SGXMKIF_CMD_TYPE;

typedef struct _SGXMKIF_COMMAND_
{
	IMG_UINT32				ui32ServiceAddress;		/*!< address of the USE command handler */
	IMG_UINT32				ui32CacheControl;		/*!< See SGXMKIF_CC_INVAL_* */
	IMG_UINT32				ui32Data[6];			/*!< array of other command control words */
} SGXMKIF_COMMAND;

typedef struct _CTL_STATUS_
{
	IMG_DEV_VIRTADDR	sStatusDevAddr;
	IMG_UINT32			ui32StatusValue;
} CTL_STATUS;

typedef struct _SGX_INTERNEL_STATUS_UPDATE_
{
	CTL_STATUS				sCtlStatus;
	IMG_HANDLE				hKernelMemInfo;
} SGX_INTERNEL_STATUS_UPDATE;

typedef struct PVRSRV_BRIDGE_RETURN_TAG
{
	PVRSRV_ERROR eError;
	IMG_VOID *pvData;

}PVRSRV_BRIDGE_RETURN;

typedef struct _SGX_CCB_KICK_
{
	SGXMKIF_COMMAND		sCommand;
	IMG_HANDLE	hCCBKernelMemInfo;

	IMG_UINT32	ui32NumDstSyncObjects;
	IMG_HANDLE	hKernelHWSyncListMemInfo;

	/* DST syncs */
	IMG_HANDLE	*pahDstSyncHandles;

	IMG_UINT32	ui32NumTAStatusVals;
	IMG_UINT32	ui32Num3DStatusVals;

#if defined(SUPPORT_SGX_NEW_STATUS_VALS)
	SGX_INTERNEL_STATUS_UPDATE	asTAStatusUpdate[SGX_MAX_TA_STATUS_VALS];
	SGX_INTERNEL_STATUS_UPDATE	as3DStatusUpdate[SGX_MAX_3D_STATUS_VALS];
#else
	IMG_HANDLE	ahTAStatusSyncInfo[SGX_MAX_TA_STATUS_VALS];
	IMG_HANDLE	ah3DStatusSyncInfo[SGX_MAX_3D_STATUS_VALS];
#endif

	IMG_BOOL	bFirstKickOrResume;
#if defined(NO_HARDWARE) || defined(PDUMP)
	IMG_BOOL	bTerminateOrAbort;
#endif
	IMG_BOOL	bLastInScene;

	/* CCB offset of data structure associated with this kick */
	IMG_UINT32	ui32CCBOffset;

#if defined(SUPPORT_SGX_GENERALISED_SYNCOBJECTS)
	/* SRC and DST syncs */
	IMG_UINT32	ui32NumTASrcSyncs;
	IMG_HANDLE	ahTASrcKernelSyncInfo[SGX_MAX_TA_SRC_SYNCS];
	IMG_UINT32	ui32NumTADstSyncs;
	IMG_HANDLE	ahTADstKernelSyncInfo[SGX_MAX_TA_DST_SYNCS];
	IMG_UINT32	ui32Num3DSrcSyncs;
	IMG_HANDLE	ah3DSrcKernelSyncInfo[SGX_MAX_3D_SRC_SYNCS];
#else
	/* SRC syncs */
	IMG_UINT32	ui32NumSrcSyncs;
	IMG_HANDLE	ahSrcKernelSyncInfo[SGX_MAX_SRC_SYNCS_TA];
#endif

	/* TA/3D dependency data */
	IMG_BOOL	bTADependency;
	IMG_HANDLE	hTA3DSyncInfo;

	IMG_HANDLE	hTASyncInfo;
	IMG_HANDLE	h3DSyncInfo;
#if defined(PDUMP)
	IMG_UINT32	ui32CCBDumpWOff;
#endif
#if defined(NO_HARDWARE)
	IMG_UINT32	ui32WriteOpsPendingVal;
#endif
	IMG_HANDLE	hDevMemContext;
	IMG_DEV_VIRTADDR	sHWRTDataSetDevAddr;
	IMG_DEV_VIRTADDR	sHWRTDataDevAddr;
	IMG_UINT32			ui32FrameNum;
#if defined(SUPPORT_PVRSRV_ANDROID_SYSTRACE)	
	IMG_BOOL	bIsFirstKick;
#endif
} SGX_CCB_KICK;

typedef struct PVRSRV_BRIDGE_IN_DOKICK_TAG
{
	IMG_UINT32				ui32BridgeFlags; /* Must be first member of structure */
	IMG_HANDLE				hDevCookie;
	SGX_CCB_KICK			sCCBKick;
}PVRSRV_BRIDGE_IN_DOKICK;

typedef struct PVRSRV_BRIDGE_IN_CONNECT_SERVICES_TAG
{
	IMG_UINT32		ui32BridgeFlags; /* Must be first member of structure */
	IMG_UINT32		ui32Flags;
} PVRSRV_BRIDGE_IN_CONNECT_SERVICES;

typedef struct PVRSRV_BRIDGE_OUT_CONNECT_SERVICES_TAG
{
	PVRSRV_ERROR    eError;
	IMG_HANDLE		hKernelServices;
}PVRSRV_BRIDGE_OUT_CONNECT_SERVICES;

typedef struct PVRSRV_BRIDGE_IN_ACQUIRE_DEVICEINFO_TAG
{
	IMG_UINT32			ui32BridgeFlags; /* Must be first member of structure */
	IMG_UINT32			uiDevIndex;
	PVRSRV_DEVICE_TYPE	eDeviceType;

} PVRSRV_BRIDGE_IN_ACQUIRE_DEVICEINFO;

typedef struct PVRSRV_BRIDGE_OUT_ACQUIRE_DEVICEINFO_TAG
{

	PVRSRV_ERROR		eError;
	IMG_HANDLE			hDevCookie;

} PVRSRV_BRIDGE_OUT_ACQUIRE_DEVICEINFO;

typedef struct _PVRSRV_SGX_MISCINFO_FEATURES
{
	IMG_UINT32			ui32CoreRev;	/*!< SGX Core revision from HW register */
	IMG_UINT32			ui32CoreID;		/*!< SGX Core ID from HW register */
	IMG_UINT32			ui32DDKVersion;	/*!< software DDK version */
	IMG_UINT32			ui32DDKBuild;	/*!< software DDK build no. */
	IMG_UINT32			ui32CoreIdSW;	/*!< software core version (ID), e.g. SGX535, SGX540 */
	IMG_UINT32			ui32CoreRevSW;	/*!< software core revision */
	IMG_UINT32			ui32BuildOptions;	/*!< build options bit-field */
#if defined(SUPPORT_SGX_EDM_MEMORY_DEBUG)
	IMG_UINT32			ui32DeviceMemValue;		/*!< device mem value read from ukernel */
#endif
} PVRSRV_SGX_MISCINFO_FEATURES;

typedef struct _PVRSRV_SGX_MISCINFO_QUERY_CLOCKSPEED_SLCSIZE
{
	IMG_UINT32                      ui32SGXClockSpeed;
	IMG_UINT32                      ui32SGXSLCSize;
} PVRSRV_SGX_MISCINFO_QUERY_CLOCKSPEED_SLCSIZE;

typedef struct _PVRSRV_SGX_MISCINFO_ACTIVEPOWER
{
	IMG_UINT32			ui32NumActivePowerEvents; /*!< active power events */
} PVRSRV_SGX_MISCINFO_ACTIVEPOWER;

typedef struct _PVRSRV_SGX_MISCINFO_LOCKUPS
{
	IMG_UINT32			ui32HostDetectedLockups; /*!< Host timer detected lockups */
	IMG_UINT32			ui32uKernelDetectedLockups; /*!< Microkernel detected lockups */
} PVRSRV_SGX_MISCINFO_LOCKUPS;

typedef struct _PVRSRV_SGX_MISCINFO_SPM
{
	IMG_HANDLE			hRTDataSet;				/*!< render target data set handle returned from SGXAddRenderTarget */
	IMG_UINT32			ui32NumOutOfMemSignals; /*!< Number of Out of Mem Signals */
	IMG_UINT32			ui32NumSPMRenders;	/*!< Number of SPM renders */
} PVRSRV_SGX_MISCINFO_SPM;

typedef struct _PVRSRV_SGX_MISCINFO_SET_HWPERF_STATUS
{
	/* See PVRSRV_SGX_HWPERF_STATUS_* */
	IMG_UINT32	ui32NewHWPerfStatus;
	
	#if defined(SGX_FEATURE_EXTENDED_PERF_COUNTERS)
	/* Specifies the HW's active group selectors */
	IMG_UINT32	aui32PerfGroup[PVRSRV_SGX_HWPERF_NUM_COUNTERS];
	/* Specifies the HW's active bit selectors */
	IMG_UINT32	aui32PerfBit[PVRSRV_SGX_HWPERF_NUM_COUNTERS];
	/* Specifies the HW's counter bit selectors */
	IMG_UINT32	ui32PerfCounterBitSelect;
	/* Specifies the HW's sum_mux selectors */
	IMG_UINT32	ui32PerfSumMux;
	#else
	/* Specifies the HW's active group */
	IMG_UINT32	ui32PerfGroup;
	#endif /* SGX_FEATURE_EXTENDED_PERF_COUNTERS */
} PVRSRV_SGX_MISCINFO_SET_HWPERF_STATUS;

typedef struct _SGX_MISC_INFO_
{
	SGX_MISC_INFO_REQUEST	eRequest;	/*!< Command request to SGXGetMiscInfo() */
	IMG_UINT32				ui32Padding;
#if defined(SUPPORT_SGX_EDM_MEMORY_DEBUG)
	IMG_DEV_VIRTADDR			sDevVAddrSrc;		/*!< dev virtual addr for mem read */
	IMG_DEV_VIRTADDR			sDevVAddrDest;		/*!< dev virtual addr for mem write */
	IMG_HANDLE					hDevMemContext;		/*!< device memory context for mem debug */
#endif
	union
	{
		IMG_UINT32	reserved;	/*!< Unused: ensures valid code in the case everything else is compiled out */
		PVRSRV_SGX_MISCINFO_FEATURES						sSGXFeatures;
		IMG_UINT32											ui32SGXClockSpeed;
		PVRSRV_SGX_MISCINFO_QUERY_CLOCKSPEED_SLCSIZE				sQueryClockSpeedSLCSize;
		PVRSRV_SGX_MISCINFO_ACTIVEPOWER						sActivePower;
		PVRSRV_SGX_MISCINFO_LOCKUPS							sLockups;
		PVRSRV_SGX_MISCINFO_SPM								sSPM;
#if defined(SGX_FEATURE_DATA_BREAKPOINTS)
		SGX_BREAKPOINT_INFO									sSGXBreakpointInfo;
#endif
		PVRSRV_SGX_MISCINFO_SET_HWPERF_STATUS				sSetHWPerfStatus;

#if defined(PVRSRV_USSE_EDM_STATUS_DEBUG)
		PVRSRV_SGX_MISCINFO_EDM_STATUS_BUFFER_INFO			sEDMStatusBufferInfo;
#endif
	} uData;
} SGX_MISC_INFO;

typedef struct PVRSRV_BRIDGE_IN_SGXGETMISCINFO_TAG
{
	IMG_UINT32		ui32BridgeFlags; /* Must be first member of structure */
	IMG_HANDLE		hDevCookie;
	SGX_MISC_INFO	*psMiscInfo;
}PVRSRV_BRIDGE_IN_SGXGETMISCINFO;

typedef struct PVRSRV_BRIDGE_OUT_ENUMDEVICE_TAG
{
	PVRSRV_ERROR eError;
	IMG_UINT32 ui32NumDevices;
	PVRSRV_DEVICE_IDENTIFIER asDeviceIdentifier[PVRSRV_MAX_DEVICES];

}PVRSRV_BRIDGE_OUT_ENUMDEVICE;

typedef struct PVRSRV_BRIDGE_IN_CREATE_DEVMEMCONTEXT_TAG
{
	IMG_UINT32			ui32BridgeFlags; /* Must be first member of structure */
	IMG_HANDLE			hDevCookie;

}PVRSRV_BRIDGE_IN_CREATE_DEVMEMCONTEXT;

typedef struct PVRSRV_BRIDGE_IN_GETCLIENTINFO_TAG
{
	IMG_UINT32					ui32BridgeFlags; /* Must be first member of structure */
	IMG_HANDLE					hDevCookie;
}PVRSRV_BRIDGE_IN_GETCLIENTINFO;

#endif
