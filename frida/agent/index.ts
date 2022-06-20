import { Console } from "console" 
import * as funs from './offsets';
import { log, in_range, align } from "./util"

var glES2Lib = Process.getModuleByName("libGLESv2_POWERVR_SGX540_120.so")
var umLib    = Process.getModuleByName("libsrv_um_SGX540_120.so")

log("gles2 base: " + glES2Lib.base)

//Handling of glDrawArrays tracing

Interceptor.attach(glES2Lib.getExportByName("glDrawArrays"), {
    onEnter : function(args) {
        log("glDrawArrays()")
    }
})

var DrawArrayFPs_Loaded = funs.DrawArrayFPs.map(x => glES2Lib.base.add(x))

DrawArrayFPs_Loaded.map(
    (fnp, index) => {
        log("Attaching to DrawArrays" + (index + 1) + " at " + fnp + " rel: " + fnp.sub(glES2Lib.base))
        Interceptor.attach(fnp, {
            onEnter : function(args) {
                log("Drawarrays"  + (index + 1) + " called")
                log("relative ip: " + fnp.sub(glES2Lib.base))
                log("Primitive type: " + args[1])
                log("first: " + args[2])
                log("count: " + args[3])
                log("arg5:  " + args[4])
            }
        })
    }
)

Interceptor.attach(glES2Lib.base.add(funs.DrawArrayBackend),{
    onEnter : function (args) {
        log("Drawarrays backend")
        log("relative ip: " + this.context.pc.sub(glES2Lib.base))
        log("Primitive type: " + args[1])
        log("\n")
    }
})

function addr_to_name(addr: number): String {
    if (in_range(addr, 0x1000, 0xb7fe000)) {
        return "General";
    } else if (in_range(addr, 0xc800000, 0xfff000)){
        return "TAData";
    } else if (in_range(addr, 0xe400000, 0x7f000)) {
        return "KernelCode";
    } else if (in_range(addr, 0xf000000, 0x3ff000)) {
        return "KernelData";
    } else if (in_range(addr, 0xf400000, 0x4ff000)) {
        return "PixelShaderCode";
    } else if (in_range(addr, 0xfc00000, 0x1ff000)) {
        return "VertexShaderCode";
    } else if (in_range(addr, 0xdc00000, 0x7ff000)) {
        return "PDSPPixelCodeData";
    } else if (in_range(addr, 0xe800000, 0x7ff000)) {
        return "PDSPVertexCodeData";
    } else if (in_range(addr, 0xd800000, 0x3ff000)) {
        return "CacheCoherent";
    } else if (in_range(addr, 0xb800000, 0)) { //seemingly unused?
        return "Shared3DParameters";
    } else if (in_range(addr, 0xb800000, 0xfff000)) {
        return "PerContext3DParameters";
    } else {
        return "Unknown";
    }
}

function is_code(addr: number) : boolean {
    return in_range(addr, 0xf400000, 0x4ff000) ||
           in_range(addr, 0xfc00000, 0x1ff000) ||
           in_range(addr, 0xdc00000, 0x7ff000) ||
           in_range(addr, 0xe800000, 0x7ff000)
}

Interceptor.attach(umLib.getExportByName("PVRSRVAllocDeviceMem"), {
    onEnter : function(args) {
        this.outputAlloc = args[5]
        this.size = args[3]

        if (in_range(this.returnAddress.toUInt32(), glES2Lib.base.toUInt32(), glES2Lib.size)) {
            log("called from gles2: " + this.returnAddress.sub(glES2Lib.base))
        } else {
            log("called from gles2: " + this.returnAddress)
        }

    },

    onLeave : function (retval) {
        var ptr = new NativePointer(this.outputAlloc.readU32());
        var mmaped_addr = new NativePointer(ptr.readU32())
        var gpu_ptr = ptr.add(0x8).readU32()
        var sz = align(this.size.toUInt32(), 0x1000)

        log("gpu addr: " + addr_to_name(gpu_ptr) +
            "(" + gpu_ptr.toString(16) + "h)" +
            " len: " + sz.toString(16))
        
        var zeros = new Array(this.size.toUInt32()).fill(0);
        mmaped_addr.writeByteArray(zeros)
    }
})