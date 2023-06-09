#define PVRSRV_BRIDGE_UMKM_CMD_FIRST			0UL
PVRSRV_IOCTL(PVRSRV_BRIDGE_UM_KM_COMPAT_CHECK	, 0)
#define PVRSRV_BRIDGE_UMKM_CMD_LAST			(0)

#define PVRSRV_BRIDGE_CORE_CMD_FIRST			(PVRSRV_BRIDGE_UMKM_CMD_LAST + 1)
PVRSRV_IOCTL(PVRSRV_BRIDGE_ENUM_DEVICES			, PVRSRV_BRIDGE_CORE_CMD_FIRST+0)	/*!< enumerate device bridge index */
PVRSRV_IOCTL(PVRSRV_BRIDGE_ACQUIRE_DEVICEINFO	, PVRSRV_BRIDGE_CORE_CMD_FIRST+1)	/*!< acquire device data bridge index */
PVRSRV_IOCTL(PVRSRV_BRIDGE_RELEASE_DEVICEINFO	, PVRSRV_BRIDGE_CORE_CMD_FIRST+2)	/*!< release device data bridge index */
PVRSRV_IOCTL(PVRSRV_BRIDGE_CREATE_DEVMEMCONTEXT	, PVRSRV_BRIDGE_CORE_CMD_FIRST+3)	/*!< create device addressable memory context */
PVRSRV_IOCTL(PVRSRV_BRIDGE_DESTROY_DEVMEMCONTEXT	, PVRSRV_BRIDGE_CORE_CMD_FIRST+4)	/*!< destroy device addressable memory context */
PVRSRV_IOCTL(PVRSRV_BRIDGE_GET_DEVMEM_HEAPINFO	, PVRSRV_BRIDGE_CORE_CMD_FIRST+5)	/*!< get device memory heap info */
PVRSRV_IOCTL(PVRSRV_BRIDGE_ALLOC_DEVICEMEM		, PVRSRV_BRIDGE_CORE_CMD_FIRST+6)	/*!< alloc device memory bridge index */
PVRSRV_IOCTL(PVRSRV_BRIDGE_FREE_DEVICEMEM		, PVRSRV_BRIDGE_CORE_CMD_FIRST+7)	/*!< free device memory bridge index */
PVRSRV_IOCTL(PVRSRV_BRIDGE_GETFREE_DEVICEMEM		, PVRSRV_BRIDGE_CORE_CMD_FIRST+8)	/*!< get free device memory bridge index */
PVRSRV_IOCTL(PVRSRV_BRIDGE_CREATE_COMMANDQUEUE	, PVRSRV_BRIDGE_CORE_CMD_FIRST+9)	/*!< create Cmd Q bridge index */
PVRSRV_IOCTL(PVRSRV_BRIDGE_DESTROY_COMMANDQUEUE	, PVRSRV_BRIDGE_CORE_CMD_FIRST+10)	/*!< destroy Cmd Q bridge index */
PVRSRV_IOCTL(PVRSRV_BRIDGE_MHANDLE_TO_MMAP_DATA,		PVRSRV_BRIDGE_CORE_CMD_FIRST+11)   /*!< generate mmap data from a memory handle */
PVRSRV_IOCTL(PVRSRV_BRIDGE_CONNECT_SERVICES		, PVRSRV_BRIDGE_CORE_CMD_FIRST+12)	/*!< services connect bridge index */
PVRSRV_IOCTL(PVRSRV_BRIDGE_DISCONNECT_SERVICES	, PVRSRV_BRIDGE_CORE_CMD_FIRST+13)	/*!< services disconnect bridge index */
PVRSRV_IOCTL(PVRSRV_BRIDGE_WRAP_DEVICE_MEM		, PVRSRV_BRIDGE_CORE_CMD_FIRST+14)	/*!< wrap device memory bridge index */
PVRSRV_IOCTL(PVRSRV_BRIDGE_GET_DEVICEMEMINFO		, PVRSRV_BRIDGE_CORE_CMD_FIRST+15)	/*!< read the kernel meminfo record */
PVRSRV_IOCTL(PVRSRV_BRIDGE_RESERVE_DEV_VIRTMEM	, PVRSRV_BRIDGE_CORE_CMD_FIRST+16)
PVRSRV_IOCTL(PVRSRV_BRIDGE_FREE_DEV_VIRTMEM		, PVRSRV_BRIDGE_CORE_CMD_FIRST+17)
PVRSRV_IOCTL(PVRSRV_BRIDGE_MAP_EXT_MEMORY		, PVRSRV_BRIDGE_CORE_CMD_FIRST+18)
PVRSRV_IOCTL(PVRSRV_BRIDGE_UNMAP_EXT_MEMORY		, PVRSRV_BRIDGE_CORE_CMD_FIRST+19)
PVRSRV_IOCTL(PVRSRV_BRIDGE_MAP_DEV_MEMORY		, PVRSRV_BRIDGE_CORE_CMD_FIRST+20)
PVRSRV_IOCTL(PVRSRV_BRIDGE_UNMAP_DEV_MEMORY		, PVRSRV_BRIDGE_CORE_CMD_FIRST+21)
PVRSRV_IOCTL(PVRSRV_BRIDGE_MAP_MEM_INFO_TO_USER	, PVRSRV_BRIDGE_CORE_CMD_FIRST+22)
PVRSRV_IOCTL(PVRSRV_BRIDGE_UNMAP_MEM_INFO_FROM_USER, PVRSRV_BRIDGE_CORE_CMD_FIRST+23)
PVRSRV_IOCTL(PVRSRV_BRIDGE_EXPORT_DEVICEMEM		, PVRSRV_BRIDGE_CORE_CMD_FIRST+24)
PVRSRV_IOCTL(PVRSRV_BRIDGE_RELEASE_MMAP_DATA		, PVRSRV_BRIDGE_CORE_CMD_FIRST+25)
PVRSRV_IOCTL(PVRSRV_BRIDGE_CHG_DEV_MEM_ATTRIBS	, PVRSRV_BRIDGE_CORE_CMD_FIRST+26)
PVRSRV_IOCTL(PVRSRV_BRIDGE_MAP_DEV_MEMORY_2		, PVRSRV_BRIDGE_CORE_CMD_FIRST+27)
PVRSRV_IOCTL(PVRSRV_BRIDGE_EXPORT_DEVICEMEM_2	, PVRSRV_BRIDGE_CORE_CMD_FIRST+28)
#if defined (SUPPORT_ION)
PVRSRV_IOCTL(PVRSRV_BRIDGE_MAP_ION_HANDLE		, PVRSRV_BRIDGE_CORE_CMD_FIRST+29)
PVRSRV_IOCTL(PVRSRV_BRIDGE_UNMAP_ION_HANDLE		, PVRSRV_BRIDGE_CORE_CMD_FIRST+30)
#define PVRSRV_BRIDGE_CORE_CMD_LAST				(PVRSRV_BRIDGE_CORE_CMD_FIRST+30)
#else
#define PVRSRV_BRIDGE_CORE_CMD_LAST				(PVRSRV_BRIDGE_CORE_CMD_FIRST+28)
#endif
/* SIM */
#define PVRSRV_BRIDGE_SIM_CMD_FIRST				(PVRSRV_BRIDGE_CORE_CMD_LAST+1)
PVRSRV_IOCTL(PVRSRV_BRIDGE_PROCESS_SIMISR_EVENT	, PVRSRV_BRIDGE_SIM_CMD_FIRST+0)	/*!< RTSIM pseudo ISR */
PVRSRV_IOCTL(PVRSRV_BRIDGE_REGISTER_SIM_PROCESS	, PVRSRV_BRIDGE_SIM_CMD_FIRST+1)	/*!< Register RTSIM process thread */
PVRSRV_IOCTL(PVRSRV_BRIDGE_UNREGISTER_SIM_PROCESS, PVRSRV_BRIDGE_SIM_CMD_FIRST+2)	/*!< Unregister RTSIM process thread */
#define PVRSRV_BRIDGE_SIM_CMD_LAST				(PVRSRV_BRIDGE_SIM_CMD_FIRST+2)

/* User Mapping */
#define PVRSRV_BRIDGE_MAPPING_CMD_FIRST			(PVRSRV_BRIDGE_SIM_CMD_LAST+1)
PVRSRV_IOCTL(PVRSRV_BRIDGE_MAPPHYSTOUSERSPACE	, PVRSRV_BRIDGE_MAPPING_CMD_FIRST+0)	/*!< map CPU phys to user space */
PVRSRV_IOCTL(PVRSRV_BRIDGE_UNMAPPHYSTOUSERSPACE	, PVRSRV_BRIDGE_MAPPING_CMD_FIRST+1)	/*!< unmap CPU phys to user space */
PVRSRV_IOCTL(PVRSRV_BRIDGE_GETPHYSTOUSERSPACEMAP	, PVRSRV_BRIDGE_MAPPING_CMD_FIRST+2)	/*!< get user copy of Phys to Lin loopup table */
#define PVRSRV_BRIDGE_MAPPING_CMD_LAST			(PVRSRV_BRIDGE_MAPPING_CMD_FIRST+2)

#define PVRSRV_BRIDGE_STATS_CMD_FIRST			(PVRSRV_BRIDGE_MAPPING_CMD_LAST+1)
PVRSRV_IOCTL(PVRSRV_BRIDGE_GET_FB_STATS	, PVRSRV_BRIDGE_STATS_CMD_FIRST+0)	/*!< Get FB memory stats */
#define PVRSRV_BRIDGE_STATS_CMD_LAST			(PVRSRV_BRIDGE_STATS_CMD_FIRST+0)

/* API to retrieve misc. info. from services */
#define PVRSRV_BRIDGE_MISC_CMD_FIRST			(PVRSRV_BRIDGE_STATS_CMD_LAST+1)
PVRSRV_IOCTL(PVRSRV_BRIDGE_GET_MISC_INFO			, PVRSRV_BRIDGE_MISC_CMD_FIRST+0)	/*!< misc. info. */
PVRSRV_IOCTL(PVRSRV_BRIDGE_RELEASE_MISC_INFO		, PVRSRV_BRIDGE_MISC_CMD_FIRST+1)	/*!< misc. info. */
#define PVRSRV_BRIDGE_MISC_CMD_LAST				(PVRSRV_BRIDGE_MISC_CMD_FIRST+1)

/* Overlay ioctls */

#if defined (SUPPORT_OVERLAY_ROTATE_BLIT)
#define PVRSRV_BRIDGE_OVERLAY_CMD_FIRST			(PVRSRV_BRIDGE_MISC_CMD_LAST+1)
PVRSRV_IOCTL(PVRSRV_BRIDGE_INIT_3D_OVL_BLT_RES	, PVRSRV_BRIDGE_OVERLAY_CMD_FIRST+0)	/*!< 3D Overlay rotate blit init */
PVRSRV_IOCTL(PVRSRV_BRIDGE_DEINIT_3D_OVL_BLT_RES	, PVRSRV_BRIDGE_OVERLAY_CMD_FIRST+1)	/*!< 3D Overlay rotate blit deinit */
#define PVRSRV_BRIDGE_OVERLAY_CMD_LAST			(PVRSRV_BRIDGE_OVERLAY_CMD_FIRST+1)
#else
#define PVRSRV_BRIDGE_OVERLAY_CMD_LAST			PVRSRV_BRIDGE_MISC_CMD_LAST
#endif

/* PDUMP */
#if defined(PDUMP)
#define PVRSRV_BRIDGE_PDUMP_CMD_FIRST			(PVRSRV_BRIDGE_OVERLAY_CMD_LAST+1)
PVRSRV_IOCTL(PVRSRV_BRIDGE_PDUMP_INIT		, PVRSRV_BRIDGE_PDUMP_CMD_FIRST+0)	/*!< pdump command structure */
PVRSRV_IOCTL(PVRSRV_BRIDGE_PDUMP_MEMPOL		, PVRSRV_BRIDGE_PDUMP_CMD_FIRST+1)	/*!< pdump command structure */
PVRSRV_IOCTL(PVRSRV_BRIDGE_PDUMP_DUMPMEM		, PVRSRV_BRIDGE_PDUMP_CMD_FIRST+2)	/*!< pdump command structure */
PVRSRV_IOCTL(PVRSRV_BRIDGE_PDUMP_REG			, PVRSRV_BRIDGE_PDUMP_CMD_FIRST+3)	/*!< pdump command structure */
PVRSRV_IOCTL(PVRSRV_BRIDGE_PDUMP_REGPOL		, PVRSRV_BRIDGE_PDUMP_CMD_FIRST+4)	/*!< pdump command structure */
PVRSRV_IOCTL(PVRSRV_BRIDGE_PDUMP_COMMENT		, PVRSRV_BRIDGE_PDUMP_CMD_FIRST+5)	/*!< pdump command structure */
PVRSRV_IOCTL(PVRSRV_BRIDGE_PDUMP_SETFRAME		, PVRSRV_BRIDGE_PDUMP_CMD_FIRST+6)	/*!< pdump command structure */
PVRSRV_IOCTL(PVRSRV_BRIDGE_PDUMP_ISCAPTURING		, PVRSRV_BRIDGE_PDUMP_CMD_FIRST+7)	/*!< pdump command structure */
PVRSRV_IOCTL(PVRSRV_BRIDGE_PDUMP_DUMPBITMAP		, PVRSRV_BRIDGE_PDUMP_CMD_FIRST+8)	/*!< pdump command structure */
PVRSRV_IOCTL(PVRSRV_BRIDGE_PDUMP_DUMPREADREG		, PVRSRV_BRIDGE_PDUMP_CMD_FIRST+9)	/*!< pdump command structure */
PVRSRV_IOCTL(PVRSRV_BRIDGE_PDUMP_SYNCPOL		, PVRSRV_BRIDGE_PDUMP_CMD_FIRST+10)	/*!< pdump command structure */
PVRSRV_IOCTL(PVRSRV_BRIDGE_PDUMP_DUMPSYNC		, PVRSRV_BRIDGE_PDUMP_CMD_FIRST+11)	/*!< pdump command structure */
PVRSRV_IOCTL(PVRSRV_BRIDGE_PDUMP_MEMPAGES		, PVRSRV_BRIDGE_PDUMP_CMD_FIRST+12)	/*!< pdump command structure */
PVRSRV_IOCTL(PVRSRV_BRIDGE_PDUMP_DRIVERINFO		, PVRSRV_BRIDGE_PDUMP_CMD_FIRST+13)	/*!< pdump command structure */
PVRSRV_IOCTL(PVRSRV_BRIDGE_PDUMP_DUMPPDDEVPADDR	, PVRSRV_BRIDGE_PDUMP_CMD_FIRST+15)	/*!< pdump command structure */
PVRSRV_IOCTL(PVRSRV_BRIDGE_PDUMP_CYCLE_COUNT_REG_READ, PVRSRV_BRIDGE_PDUMP_CMD_FIRST+16)
PVRSRV_IOCTL(PVRSRV_BRIDGE_PDUMP_STARTINITPHASE		, PVRSRV_BRIDGE_PDUMP_CMD_FIRST+17)
PVRSRV_IOCTL(PVRSRV_BRIDGE_PDUMP_STOPINITPHASE		, PVRSRV_BRIDGE_PDUMP_CMD_FIRST+18)
#define PVRSRV_BRIDGE_PDUMP_CMD_LAST			(PVRSRV_BRIDGE_PDUMP_CMD_FIRST+18)
#else
/* Note we are carefull here not to leave a large gap in the ioctl numbers.
 * (Some ports may use these values to index into an array where large gaps can
 * waste memory) */
#define PVRSRV_BRIDGE_PDUMP_CMD_LAST			PVRSRV_BRIDGE_OVERLAY_CMD_LAST
#endif

/* DisplayClass APIs */
#define PVRSRV_BRIDGE_OEM_CMD_FIRST				(PVRSRV_BRIDGE_PDUMP_CMD_LAST+1)
PVRSRV_IOCTL(PVRSRV_BRIDGE_GET_OEMJTABLE			, PVRSRV_BRIDGE_OEM_CMD_FIRST+0)	/*!< Get OEM Jtable */
#define PVRSRV_BRIDGE_OEM_CMD_LAST				(PVRSRV_BRIDGE_OEM_CMD_FIRST+0)

/* device class enum */
#define PVRSRV_BRIDGE_DEVCLASS_CMD_FIRST		(PVRSRV_BRIDGE_OEM_CMD_LAST+1)
PVRSRV_IOCTL(PVRSRV_BRIDGE_MAP_DEVICECLASS_MEMORY, PVRSRV_BRIDGE_DEVCLASS_CMD_FIRST+0)
PVRSRV_IOCTL(PVRSRV_BRIDGE_UNMAP_DEVICECLASS_MEMORY, PVRSRV_BRIDGE_DEVCLASS_CMD_FIRST+1)
PVRSRV_IOCTL(PVRSRV_BRIDGE_ENUM_CLASS			, PVRSRV_BRIDGE_DEVCLASS_CMD_FIRST+2)
#define PVRSRV_BRIDGE_DEVCLASS_CMD_LAST			(PVRSRV_BRIDGE_DEVCLASS_CMD_FIRST+2)

/* display class API */
#define PVRSRV_BRIDGE_DISPCLASS_CMD_FIRST		(PVRSRV_BRIDGE_DEVCLASS_CMD_LAST+1)
PVRSRV_IOCTL(PVRSRV_BRIDGE_OPEN_DISPCLASS_DEVICE	, PVRSRV_BRIDGE_DISPCLASS_CMD_FIRST+0)
PVRSRV_IOCTL(PVRSRV_BRIDGE_CLOSE_DISPCLASS_DEVICE, PVRSRV_BRIDGE_DISPCLASS_CMD_FIRST+1)
PVRSRV_IOCTL(PVRSRV_BRIDGE_ENUM_DISPCLASS_FORMATS, PVRSRV_BRIDGE_DISPCLASS_CMD_FIRST+2)
PVRSRV_IOCTL(PVRSRV_BRIDGE_ENUM_DISPCLASS_DIMS	, PVRSRV_BRIDGE_DISPCLASS_CMD_FIRST+3)
PVRSRV_IOCTL(PVRSRV_BRIDGE_GET_DISPCLASS_SYSBUFFER, PVRSRV_BRIDGE_DISPCLASS_CMD_FIRST+4)
PVRSRV_IOCTL(PVRSRV_BRIDGE_GET_DISPCLASS_INFO	, PVRSRV_BRIDGE_DISPCLASS_CMD_FIRST+5)
PVRSRV_IOCTL(PVRSRV_BRIDGE_CREATE_DISPCLASS_SWAPCHAIN	, PVRSRV_BRIDGE_DISPCLASS_CMD_FIRST+6)
PVRSRV_IOCTL(PVRSRV_BRIDGE_DESTROY_DISPCLASS_SWAPCHAIN	, PVRSRV_BRIDGE_DISPCLASS_CMD_FIRST+7)
PVRSRV_IOCTL(PVRSRV_BRIDGE_SET_DISPCLASS_DSTRECT	, PVRSRV_BRIDGE_DISPCLASS_CMD_FIRST+8)
PVRSRV_IOCTL(PVRSRV_BRIDGE_SET_DISPCLASS_SRCRECT	, PVRSRV_BRIDGE_DISPCLASS_CMD_FIRST+9)
PVRSRV_IOCTL(PVRSRV_BRIDGE_SET_DISPCLASS_DSTCOLOURKEY	, PVRSRV_BRIDGE_DISPCLASS_CMD_FIRST+10)
PVRSRV_IOCTL(PVRSRV_BRIDGE_SET_DISPCLASS_SRCCOLOURKEY	, PVRSRV_BRIDGE_DISPCLASS_CMD_FIRST+11)
PVRSRV_IOCTL(PVRSRV_BRIDGE_GET_DISPCLASS_BUFFERS	, PVRSRV_BRIDGE_DISPCLASS_CMD_FIRST+12)
PVRSRV_IOCTL(PVRSRV_BRIDGE_SWAP_DISPCLASS_TO_BUFFER, PVRSRV_BRIDGE_DISPCLASS_CMD_FIRST+13)
PVRSRV_IOCTL(PVRSRV_BRIDGE_SWAP_DISPCLASS_TO_BUFFER2, PVRSRV_BRIDGE_DISPCLASS_CMD_FIRST+14)
PVRSRV_IOCTL(PVRSRV_BRIDGE_SWAP_DISPCLASS_TO_SYSTEM, PVRSRV_BRIDGE_DISPCLASS_CMD_FIRST+15)
#define PVRSRV_BRIDGE_DISPCLASS_CMD_LAST		(PVRSRV_BRIDGE_DISPCLASS_CMD_FIRST+15)

/* buffer class API */
#define PVRSRV_BRIDGE_BUFCLASS_CMD_FIRST		(PVRSRV_BRIDGE_DISPCLASS_CMD_LAST+1)
PVRSRV_IOCTL(PVRSRV_BRIDGE_OPEN_BUFFERCLASS_DEVICE, PVRSRV_BRIDGE_BUFCLASS_CMD_FIRST+0)
PVRSRV_IOCTL(PVRSRV_BRIDGE_CLOSE_BUFFERCLASS_DEVICE, PVRSRV_BRIDGE_BUFCLASS_CMD_FIRST+1)
PVRSRV_IOCTL(PVRSRV_BRIDGE_GET_BUFFERCLASS_INFO	, PVRSRV_BRIDGE_BUFCLASS_CMD_FIRST+2)
PVRSRV_IOCTL(PVRSRV_BRIDGE_GET_BUFFERCLASS_BUFFER, PVRSRV_BRIDGE_BUFCLASS_CMD_FIRST+3)
#define PVRSRV_BRIDGE_BUFCLASS_CMD_LAST			(PVRSRV_BRIDGE_BUFCLASS_CMD_FIRST+3)

/* Wrap/Unwrap external memory */
#define PVRSRV_BRIDGE_WRAP_CMD_FIRST			(PVRSRV_BRIDGE_BUFCLASS_CMD_LAST+1)
PVRSRV_IOCTL(PVRSRV_BRIDGE_WRAP_EXT_MEMORY		, PVRSRV_BRIDGE_WRAP_CMD_FIRST+0)
PVRSRV_IOCTL(PVRSRV_BRIDGE_UNWRAP_EXT_MEMORY		, PVRSRV_BRIDGE_WRAP_CMD_FIRST+1)
#define PVRSRV_BRIDGE_WRAP_CMD_LAST				(PVRSRV_BRIDGE_WRAP_CMD_FIRST+1)

/* Shared memory */
#define PVRSRV_BRIDGE_SHAREDMEM_CMD_FIRST		(PVRSRV_BRIDGE_WRAP_CMD_LAST+1)
PVRSRV_IOCTL(PVRSRV_BRIDGE_ALLOC_SHARED_SYS_MEM	, PVRSRV_BRIDGE_SHAREDMEM_CMD_FIRST+0)
PVRSRV_IOCTL(PVRSRV_BRIDGE_FREE_SHARED_SYS_MEM	, PVRSRV_BRIDGE_SHAREDMEM_CMD_FIRST+1)
PVRSRV_IOCTL(PVRSRV_BRIDGE_MAP_MEMINFO_MEM		, PVRSRV_BRIDGE_SHAREDMEM_CMD_FIRST+2)
PVRSRV_IOCTL(PVRSRV_BRIDGE_UNMAP_MEMINFO_MEM		, PVRSRV_BRIDGE_SHAREDMEM_CMD_FIRST+3)
#define PVRSRV_BRIDGE_SHAREDMEM_CMD_LAST		(PVRSRV_BRIDGE_SHAREDMEM_CMD_FIRST+3)

/* Intialisation Service support */
#define PVRSRV_BRIDGE_INITSRV_CMD_FIRST			(PVRSRV_BRIDGE_SHAREDMEM_CMD_LAST+1)
PVRSRV_IOCTL(PVRSRV_BRIDGE_INITSRV_CONNECT		, PVRSRV_BRIDGE_INITSRV_CMD_FIRST+0)
PVRSRV_IOCTL(PVRSRV_BRIDGE_INITSRV_DISCONNECT	, PVRSRV_BRIDGE_INITSRV_CMD_FIRST+1)
#define PVRSRV_BRIDGE_INITSRV_CMD_LAST			(PVRSRV_BRIDGE_INITSRV_CMD_FIRST+1)

/* Event Objects */
#define PVRSRV_BRIDGE_EVENT_OBJECT_CMD_FIRST	(PVRSRV_BRIDGE_INITSRV_CMD_LAST+1)
PVRSRV_IOCTL(PVRSRV_BRIDGE_EVENT_OBJECT_WAIT		, PVRSRV_BRIDGE_EVENT_OBJECT_CMD_FIRST+0)
PVRSRV_IOCTL(PVRSRV_BRIDGE_EVENT_OBJECT_OPEN		, PVRSRV_BRIDGE_EVENT_OBJECT_CMD_FIRST+1)
PVRSRV_IOCTL(PVRSRV_BRIDGE_EVENT_OBJECT_CLOSE	, PVRSRV_BRIDGE_EVENT_OBJECT_CMD_FIRST+2)
#define PVRSRV_BRIDGE_EVENT_OBJECT_CMD_LAST		(PVRSRV_BRIDGE_EVENT_OBJECT_CMD_FIRST+2)

/* Sync ops */
#define PVRSRV_BRIDGE_SYNC_OPS_CMD_FIRST		(PVRSRV_BRIDGE_EVENT_OBJECT_CMD_LAST+1)
PVRSRV_IOCTL(PVRSRV_BRIDGE_CREATE_SYNC_INFO_MOD_OBJ, PVRSRV_BRIDGE_SYNC_OPS_CMD_FIRST+0)
PVRSRV_IOCTL(PVRSRV_BRIDGE_DESTROY_SYNC_INFO_MOD_OBJ, PVRSRV_BRIDGE_SYNC_OPS_CMD_FIRST+1)
PVRSRV_IOCTL(PVRSRV_BRIDGE_MODIFY_PENDING_SYNC_OPS, PVRSRV_BRIDGE_SYNC_OPS_CMD_FIRST+2)
PVRSRV_IOCTL(PVRSRV_BRIDGE_MODIFY_COMPLETE_SYNC_OPS, PVRSRV_BRIDGE_SYNC_OPS_CMD_FIRST+3)
PVRSRV_IOCTL(PVRSRV_BRIDGE_SYNC_OPS_TAKE_TOKEN      , PVRSRV_BRIDGE_SYNC_OPS_CMD_FIRST+4)
PVRSRV_IOCTL(PVRSRV_BRIDGE_SYNC_OPS_FLUSH_TO_TOKEN  , PVRSRV_BRIDGE_SYNC_OPS_CMD_FIRST+5)
PVRSRV_IOCTL(PVRSRV_BRIDGE_SYNC_OPS_FLUSH_TO_MOD_OBJ, PVRSRV_BRIDGE_SYNC_OPS_CMD_FIRST+6)
PVRSRV_IOCTL(PVRSRV_BRIDGE_SYNC_OPS_FLUSH_TO_DELTA, PVRSRV_BRIDGE_SYNC_OPS_CMD_FIRST+7)
PVRSRV_IOCTL(PVRSRV_BRIDGE_ALLOC_SYNC_INFO          , PVRSRV_BRIDGE_SYNC_OPS_CMD_FIRST+8)
PVRSRV_IOCTL(PVRSRV_BRIDGE_FREE_SYNC_INFO           , PVRSRV_BRIDGE_SYNC_OPS_CMD_FIRST+9)
#define PVRSRV_BRIDGE_SYNC_OPS_CMD_LAST			(PVRSRV_BRIDGE_SYNC_OPS_CMD_FIRST+9)

/* For sgx_bridge.h (msvdx_bridge.h should probably use these defines too) */
#define PVRSRV_BRIDGE_LAST_NON_DEVICE_CMD		(PVRSRV_BRIDGE_SYNC_OPS_CMD_LAST+1)

#define PVRSRV_BRIDGE_SGX_CMD_BASE (PVRSRV_BRIDGE_LAST_NON_DEVICE_CMD+1)
PVRSRV_IOCTL(PVRSRV_BRIDGE_SGX_GETCLIENTINFO		, PVRSRV_BRIDGE_SGX_CMD_BASE+0)
PVRSRV_IOCTL(PVRSRV_BRIDGE_SGX_RELEASECLIENTINFO	, PVRSRV_BRIDGE_SGX_CMD_BASE+1)
PVRSRV_IOCTL(PVRSRV_BRIDGE_SGX_GETINTERNALDEVINFO, PVRSRV_BRIDGE_SGX_CMD_BASE+2)
PVRSRV_IOCTL(PVRSRV_BRIDGE_SGX_DOKICK			, PVRSRV_BRIDGE_SGX_CMD_BASE+3)
PVRSRV_IOCTL(PVRSRV_BRIDGE_SGX_GETPHYSPAGEADDR	, PVRSRV_BRIDGE_SGX_CMD_BASE+4)
PVRSRV_IOCTL(PVRSRV_BRIDGE_SGX_READREGISTRYDWORD	, PVRSRV_BRIDGE_SGX_CMD_BASE+5)

PVRSRV_IOCTL(PVRSRV_BRIDGE_SGX_2DQUERYBLTSCOMPLETE  , PVRSRV_BRIDGE_SGX_CMD_BASE+9)

#if defined(TRANSFER_QUEUE)
PVRSRV_IOCTL(PVRSRV_BRIDGE_SGX_SUBMITTRANSFER		, PVRSRV_BRIDGE_SGX_CMD_BASE+13)
#endif
PVRSRV_IOCTL(PVRSRV_BRIDGE_SGX_GETMISCINFO			, PVRSRV_BRIDGE_SGX_CMD_BASE+14)
PVRSRV_IOCTL(PVRSRV_BRIDGE_SGXINFO_FOR_SRVINIT		, PVRSRV_BRIDGE_SGX_CMD_BASE+15)
PVRSRV_IOCTL(PVRSRV_BRIDGE_SGX_DEVINITPART2			, PVRSRV_BRIDGE_SGX_CMD_BASE+16)

PVRSRV_IOCTL(PVRSRV_BRIDGE_SGX_FINDSHAREDPBDESC		, PVRSRV_BRIDGE_SGX_CMD_BASE+17)
PVRSRV_IOCTL(PVRSRV_BRIDGE_SGX_UNREFSHAREDPBDESC		, PVRSRV_BRIDGE_SGX_CMD_BASE+18)
PVRSRV_IOCTL(PVRSRV_BRIDGE_SGX_ADDSHAREDPBDESC		, PVRSRV_BRIDGE_SGX_CMD_BASE+19)
PVRSRV_IOCTL(PVRSRV_BRIDGE_SGX_REGISTER_HW_RENDER_CONTEXT, PVRSRV_BRIDGE_SGX_CMD_BASE+20)
PVRSRV_IOCTL(PVRSRV_BRIDGE_SGX_FLUSH_HW_RENDER_TARGET, PVRSRV_BRIDGE_SGX_CMD_BASE+21)
PVRSRV_IOCTL(PVRSRV_BRIDGE_SGX_UNREGISTER_HW_RENDER_CONTEXT, PVRSRV_BRIDGE_SGX_CMD_BASE+22)
#if defined(SGX_FEATURE_2D_HARDWARE)
PVRSRV_IOCTL(PVRSRV_BRIDGE_SGX_SUBMIT2D				, PVRSRV_BRIDGE_SGX_CMD_BASE+23)
PVRSRV_IOCTL(PVRSRV_BRIDGE_SGX_REGISTER_HW_2D_CONTEXT, PVRSRV_BRIDGE_SGX_CMD_BASE+24)
PVRSRV_IOCTL(PVRSRV_BRIDGE_SGX_UNREGISTER_HW_2D_CONTEXT, PVRSRV_BRIDGE_SGX_CMD_BASE+25)
#endif
PVRSRV_IOCTL(PVRSRV_BRIDGE_SGX_REGISTER_HW_TRANSFER_CONTEXT, PVRSRV_BRIDGE_SGX_CMD_BASE+26)
PVRSRV_IOCTL(PVRSRV_BRIDGE_SGX_UNREGISTER_HW_TRANSFER_CONTEXT, PVRSRV_BRIDGE_SGX_CMD_BASE+27)

PVRSRV_IOCTL(PVRSRV_BRIDGE_SGX_SCHEDULE_PROCESS_QUEUES	, PVRSRV_BRIDGE_SGX_CMD_BASE+28)

PVRSRV_IOCTL(PVRSRV_BRIDGE_SGX_READ_HWPERF_CB		, PVRSRV_BRIDGE_SGX_CMD_BASE+29)
PVRSRV_IOCTL(PVRSRV_BRIDGE_SGX_SET_RENDER_CONTEXT_PRIORITY     , PVRSRV_BRIDGE_SGX_CMD_BASE+30)
PVRSRV_IOCTL(PVRSRV_BRIDGE_SGX_SET_TRANSFER_CONTEXT_PRIORITY     , PVRSRV_BRIDGE_SGX_CMD_BASE+31)

#if defined(PDUMP)
PVRSRV_IOCTL(PVRSRV_BRIDGE_SGX_PDUMP_BUFFER_ARRAY	, PVRSRV_BRIDGE_SGX_CMD_BASE+32)
PVRSRV_IOCTL(PVRSRV_BRIDGE_SGX_PDUMP_3D_SIGNATURE_REGISTERS, PVRSRV_BRIDGE_SGX_CMD_BASE+33)
PVRSRV_IOCTL(PVRSRV_BRIDGE_SGX_PDUMP_COUNTER_REGISTERS, PVRSRV_BRIDGE_SGX_CMD_BASE+34)
PVRSRV_IOCTL(PVRSRV_BRIDGE_SGX_PDUMP_TA_SIGNATURE_REGISTERS, PVRSRV_BRIDGE_SGX_CMD_BASE+35)
PVRSRV_IOCTL(PVRSRV_BRIDGE_SGX_PDUMP_HWPERFCB			, PVRSRV_BRIDGE_SGX_CMD_BASE+36)
PVRSRV_IOCTL(PVRSRV_BRIDGE_SGX_PDUMP_SAVEMEM				, PVRSRV_BRIDGE_SGX_CMD_BASE+37)
#endif

/* *REMEMBER* to update PVRSRV_BRIDGE_LAST_SGX_CMD if you add/remove a command! 
 * You need to ensure all PVRSRV_BRIDGE_SGX_CMD_BASE+ offsets are sequential!
 */
#define PVRSRV_BRIDGE_LAST_SGX_CMD (PVRSRV_BRIDGE_SGX_CMD_BASE+37)
