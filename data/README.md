`preload_hack.c` is compiled with `gcc -g -m32 preload_hack.c  -o preload_hack.so -fPIC -shared -ldl`, the intended usage is as a `LD_PRELOAD` binary for the official `GLSLESCompiler_Series5` that imgtech provides https://developer.imaginationtech.com/pvrshadereditor/, the first argument passed to it is the file you want to disassemble, the disassembly will be written to standard output.


This is possible because while imgtech claims to not provide the disassembler for Series5 gpus they just left out any calls to it.
