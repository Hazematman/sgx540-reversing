import { Console } from "console" 
import * as funs from './offsets';
import { log, in_range, align } from "./util"


var androidLibgl = Process.getModuleByName("libGLESv2.so")
var glES2Lib = Process.getModuleByName("libGLESv2_POWERVR_SGX540_120.so")
var umLib    = Process.getModuleByName("libsrv_um_SGX540_120.so")
var libc     = Process.getModuleByName("libc.so")

var monitored_ranges: Map<NativePointer, [number, number]> = new Map()
var memory_backups: NativePointer[] = []

log("gles2 base: " + glES2Lib.base)

//Handling of glDrawArrays tracing

Interceptor.attach(androidLibgl.getExportByName("glDrawArrays"), {
    onEnter : function(args) {
        log("glDrawArrays()")
    }
})

Interceptor.attach(glES2Lib.getExportByName("glDrawArrays"), {
    onEnter : function(args) {
        log("glDrawArrays_SGX()")
        log("DrawArrays initiated by: " + Thread.backtrace(this.context, Backtracer.ACCURATE)
                    .map(DebugSymbol.fromAddress).join('\n') + '\n');
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

//Get all memcpy calls and check if they write to any area of interest
Interceptor.attach(libc.getExportByName("memcpy"), {
    onEnter : function(memcpy_args) {
        monitored_ranges.forEach((val: [number, number], key: NativePointer) => {
            if (in_range(memcpy_args[0].toUInt32(), key.toUInt32(), val[0])) {
                //setup data for onLeave
                this.to_track = true

                let gpu_addr = val[1]
                let caller = Process.findModuleByAddress(this.returnAddress)
                let offset = memcpy_args[0].sub(key)
                if (caller == undefined) {
                    log("undefined caller")
                    return
                }
                log("code memcpyd to in " + caller.name + " at " + (this.returnAddress.sub(caller.base)) +
                        " gpu addr: " + gpu_addr.toString(16) + " offset " + offset + " zone (" + addr_to_name(gpu_addr) + ")")
                log("memcpy initiated by: " + Thread.backtrace(this.context, Backtracer.ACCURATE)
                    .map(DebugSymbol.fromAddress).join('\n') + '\n');

                //if we're tracking this memory access it means we also want to know if it was modified, so let's take a backup
                log("allocating: " + memcpy_args[2])
                var mem = Memory.alloc(memcpy_args[2].toUInt32())
                Memory.copy(mem, memcpy_args[1], memcpy_args[2].toUInt32())
                memory_backups.push(mem)
            }
        })
    },
    onLeave : function(ret) {
    }
})

Interceptor.attach(glES2Lib.base.add(funs.DrawArrayBackend),{
    onEnter : function (args) {
        log("Drawarrays backend")
        log("relative ip: " + this.context.pc.sub(glES2Lib.base))
        log("Primitive type: " + args[1])

        //Stalker.follow(this.threadId, stalker_code)
    },
    onLeave : function(ret) {
        log("leaving the drawarrays backend")
        //Stalker.unfollow(this.threadId)
        log("\n")
    }
})

//Code to stalk every single memory access instruction in a certain range (best used on the smallest area possible)
//this may be useful in case writes to certain mapped areas are not done via memcpy
let stalker_code = {events: {
    call: false,
    ret: false,
    exec: false,
    block: false,
    compile: false 
  },
  onReceive(events: ArrayBuffer) {},
  transform(iterator_g: (StalkerArmIterator | StalkerX86Iterator | StalkerThumbIterator | StalkerArm64Iterator)) {
    let iterator = <StalkerArmIterator>iterator_g;
    let instruction = iterator.next()
    if (instruction == null) {
        iterator.keep()
        return
    }
    do {
        if (in_range(instruction.address.toUInt32(), glES2Lib.base.toUInt32(), glES2Lib.size)) {
            if (instruction.mnemonic.startsWith("st")) {
                //log(instruction.mnemonic + " " + instruction.opStr)
                iterator.putCallout(single_step_trace)
            }
        } else {
            //log("outside instruction: " + Process.findModuleByAddress(instruction.address)?.name)
        }
        iterator.keep()
    } while ((instruction = iterator.next()) !== null)
  }
}

//TODO: finish the instruction decoding portion
function single_step_trace(context_g: CpuContext) {
    var context = <ArmCpuContext>context_g
    var instruction = <ArmInstruction>Instruction.parse(context.pc)
    log(instruction.toString())
    let ops = instruction.operands

    let mem = <ArmMemOperand>ops[1]
    let base = mem.value.base
    switch(base) {
        case "sb":
            base = "r9"
        break;
        case "ip":
            base = "pc"
        break
    }
    let base_value = context[base as (keyof typeof context)]
    let index_value = undefined
    if (mem.value.index != undefined) {
        index_value = context[mem.value.index as (keyof typeof context)]
    } else {
        index_value = new NativePointer(0)
    }

    let addr = base_value.add(index_value).add(mem.value.disp);
}

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
        //get output of the memory allocation function
        var ptr = new NativePointer(this.outputAlloc.readU32());
        var mmaped_addr = new NativePointer(ptr.readU32())
        var gpu_ptr = ptr.add(0x8).readU32()
        var sz = align(this.size.toUInt32(), 0x1000)

        log("gpu addr: " + addr_to_name(gpu_ptr) +
            "(" + gpu_ptr.toString(16) + "h)" +
            " len: " + sz.toString(16))
        log("mapped to: " + mmaped_addr)
        
        //zero out the memory for easier analysis (the driver doesn't do this)
        var zeros = new Array(this.size.toUInt32()).fill(0);
        mmaped_addr.writeByteArray(zeros)

        //set a manual "watchpoint", Memory.Protect seems to behave wery weirdly on android and causes a full system crash
        if (is_code(gpu_ptr)) {
            log("monitoring new range: " + mmaped_addr)
            monitored_ranges.set(mmaped_addr, [sz, gpu_ptr])
        }
    }
})
