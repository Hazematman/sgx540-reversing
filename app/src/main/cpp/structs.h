#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*************************************************************************/ /*!
@Title          PVR Bridge Functionality
@Copyright      Copyright (c) Imagination Technologies Ltd. All Rights Reserved
@Description    Header for the PVR Bridge code
@License        Dual MIT/GPLv2

The contents of this file are subject to the MIT license as set out below.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

Alternatively, the contents of this file may be used under the terms of
the GNU General Public License Version 2 ("GPL") in which case the provisions
of GPL are applicable instead of those above.

If you wish to allow use of your version of this file only under the terms of
GPL, and not to allow others to use your version of this file under the terms
of the MIT license, indicate your decision by deleting the provisions above
and replace them with the notice and other provisions required by GPL as set
out in the file called "GPL-COPYING" included in this distribution. If you do
not delete the provisions above, a recipient may use your version of this file
under the terms of either the MIT license or GPL.

This License is also included in this distribution in the file called
"MIT-COPYING".

EXCEPT AS OTHERWISE STATED IN A NEGOTIATED AGREEMENT: (A) THE SOFTWARE IS
PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE AND NONINFRINGEMENT; AND (B) IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/ /**************************************************************************/

typedef struct PVRSRV_BRIDGE_IN_ALLOCDEVICEMEM_TAG {
	uint32_t			ui32BridgeFlags; /* Must be first member of structure */
	void*    			hDevCookie;
	void*    			hDevMemHeap;
	uint32_t			ui32Attribs;
	size_t   			uSize;
	size_t  			uAlignment;
	void*   			pvPrivData;
	uint32_t			ui32PrivDataLength;

	uint32_t			ui32ChunkSize;
	uint32_t			ui32NumVirtChunks;
	uint32_t			ui32NumPhysChunks;
	bool    			*pabMapChunk;
} PVRSRV_BRIDGE_IN_ALLOCDEVICEMEM;

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

typedef struct _PVRSRV_KERNEL_MEM_INFO_ *PPVRSRV_KERNEL_MEM_INFO;

typedef struct _PVRSRV_CLIENT_MEM_INFO_
{
	/* CPU Virtual Address */
	void*   				pvLinAddr;

	/* CPU Virtual Address (for kernel mode) */
	void*   				pvLinAddrKM;

	/* Device Virtual Address */
	uint32_t        		sDevVAddr;

	/* allocation flags */
	uint32_t				ui32Flags;

	/* client allocation flags */
	uint32_t				ui32ClientFlags;

	/* allocation size in bytes */
	size_t  				uAllocSize;


	/* ptr to associated client sync info - NULL if no sync */
	struct _PVRSRV_CLIENT_SYNC_INFO_	*psClientSyncInfo;

	/* handle to client mapping data (OS specific) */
	void*   							hMappingInfo;

	/* handle to kernel mem info */
	void*   							hKernelMemInfo;

	/* resman handle for UM mapping clean-up */
	void*   							hResItem;

#if defined(SUPPORT_MEMINFO_IDS)
	#if !defined(USE_CODE)
	/* Globally unique "stamp" for allocation (not re-used until wrap) */
	IMG_UINT64							ui64Stamp;
	#else /* !defined(USE_CODE) */
	uint32_t							dummy1;
	uint32_t							dummy2;
	#endif /* !defined(USE_CODE) */
#endif /* defined(SUPPORT_MEMINFO_IDS) */
#if defined(SUPPORT_ION)
	size_t  							uiIonBufferSize;
#endif /* defined(SUPPORT_ION) */

	/*
		ptr to next mem info
		D3D uses psNext for mid-scene texture reload.
	*/
	struct _PVRSRV_CLIENT_MEM_INFO_		*psNext;

} PVRSRV_CLIENT_MEM_INFO, *PPVRSRV_CLIENT_MEM_INFO;

typedef struct _PVRSRV_MEMBLK_
{
	uint32_t        	sDevVirtAddr;			/*!< Address of the memory in the IMG MMUs address space */
	void*   			hOSMemHandle;			/*!< Stores the underlying memory allocation handle */
	void*   			hOSWrapMem;				/*!< FIXME: better way to solve this problem */
	void*   			hBuffer;				/*!< Stores the BM_HANDLE for the underlying memory management */
	void*   			hResItem;				/*!< handle to resource item for allocate */
	uint32_t    	 	*psIntSysPAddr;
} PVRSRV_MEMBLK;

typedef enum _PVRSRV_MEMTYPE_ {
	PVRSRV_MEMTYPE_UNKNOWN		= 0,
	PVRSRV_MEMTYPE_DEVICE		= 1,
	PVRSRV_MEMTYPE_DEVICECLASS	= 2,
	PVRSRV_MEMTYPE_WRAPPED		= 3,
	PVRSRV_MEMTYPE_MAPPED		= 4,
	PVRSRV_MEMTYPE_ION			= 5,
	PVRSRV_MEMTYPE_ALLOC		= 6,
	PVRSRV_MEMTYPE_FREE	    	= 7
} PVRSRV_MEMTYPE;


typedef struct _PVRSRV_KERNEL_MEM_INFO_
{
	/* Kernel Mode CPU Virtual address */
	void*				pvLinAddrKM;

	/* Device Virtual Address */
	uint32_t		sDevVAddr;

	/* allocation flags */
	uint32_t				ui32Flags;

	/* Size of the allocated buffer in bytes */
	size_t  				uAllocSize;

	/* Internal implementation details. Do not use outside services code. */
	PVRSRV_MEMBLK			sMemBlk;

	/* Address of the backup buffer used in a save/restore of the surface */
	void*				pvSysBackupBuffer;

	/* refcount for allocation, wrapping and mapping */
	uint32_t				ui32RefCount;

	/* Set when free call ocured and a mapping was still open */
	bool				bPendingFree;


#if defined(SUPPORT_MEMINFO_IDS)
	#if !defined(USE_CODE)
	/* Globally unique "stamp" for allocation (not re-used until wrap) */
	uint64_t				ui64Stamp;
	#else /* !defined(USE_CODE) */
	uint32_t				dummy1;
	uint32_t				dummy2;
	#endif /* !defined(USE_CODE) */
#endif /* defined(SUPPORT_MEMINFO_IDS) */

	/* ptr to associated kernel sync info - NULL if no sync */
	struct _PVRSRV_KERNEL_SYNC_INFO_	*psKernelSyncInfo;

	void*   				hIonSyncInfo;

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
		bool bInUse;

        /* Store the device cookie handle from the original
           allocation, as it is not present on the "Map" API. */
		void*    hDevCookieInt;

        /* This is an index into a static array which store
           information about the underlying allocation */
		uint32_t ui32ShareIndex;

        /* Original arguments as supplied to original
           "PVRSRVAllocDeviceMem" call, such that a new call to this
           function can be later constructed */
		uint32_t ui32OrigReqAttribs;
		uint32_t ui32OrigReqSize;
		uint32_t ui32OrigReqAlignment;
	} sShareMemWorkaround;
#if defined (MEM_TRACK_INFO_DEBUG)
	IMG_CHAR heapId[128];
#endif
} PVRSRV_KERNEL_MEM_INFO;


typedef struct PVRSRV_BRIDGE_OUT_ALLOCDEVICEMEM_TAG {
	PVRSRV_ERROR            eError;
	PVRSRV_KERNEL_MEM_INFO	*psKernelMemInfo;
	PVRSRV_CLIENT_MEM_INFO  sClientMemInfo;
    //PURPOSEFULLY LEFT OUT, this information doesn't seem necessary at the moment
//	PVRSRV_CLIENT_SYNC_INFO sClientSyncInfo;

} PVRSRV_BRIDGE_OUT_ALLOCDEVICEMEM;

typedef struct PVRSRV_BRIDGE_PACKAGE_TAG {
	uint32_t 				ui32BridgeID;			/*!< ioctl/drvesc index */
	uint32_t 				ui32Size;				/*!< size of structure */
	void    				*pvParamIn;				/*!< input data buffer */
	uint32_t 				ui32InBufferSize;		/*!< size of input data buffer */
	void    				*pvParamOut;			/*!< output data buffer */
	uint32_t 				ui32OutBufferSize;		/*!< size of output data buffer */

	void*    				hKernelServices;		/*!< kernel servcies handle */
} PVRSRV_BRIDGE_PACKAGE;

typedef struct PVRSRV_BRIDGE_OUT_MHANDLE_TO_MMAP_DATA_TAG {
    PVRSRV_ERROR		eError;

    /* This is a the offset you should pass to mmap(2) so that
     * the driver can look up the full details for the mapping
     * request. */
     uintptr_t		uiMMapOffset;

    /* This is the byte offset you should add to the mapping you
     * get from mmap */
    uintptr_t		uiByteOffset;

    /* This is the real size of the mapping that will be created
     * which should be passed to mmap _and_ munmap. */
    size_t 			uiRealByteSize;

    /* User mode address associated with mapping */
    uintptr_t       uiUserVAddr;

} PVRSRV_BRIDGE_OUT_MHANDLE_TO_MMAP_DATA;

typedef struct PVRSRV_BRIDGE_IN_MHANDLE_TO_MMAP_DATA_TAG
{
	uint32_t			ui32BridgeFlags; /* Must be first member of structure */
	void*   			hMHandle;	 /* Handle associated with the memory that needs to be mapped */
} PVRSRV_BRIDGE_IN_MHANDLE_TO_MMAP_DATA;
