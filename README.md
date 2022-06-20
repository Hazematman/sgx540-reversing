PowerVR SGX540 dumping
=========
This repository contains a set of tools and reverse engineered information on the PowerVR SGX540 gpu found in the Galaxy Tab 2 7.0 P3110 by samsung


All of the reverse engineering is done out on the Cyanogenmod-based Beanstalck android 6.0 rom version `6.15-20160430-espressowifi`

The tools in this repository can be build using the android studio IDE, after building the project the testing apk will be located under `app/build/outputs/apk/debug/app-debug.apk` and the intercept tool will be under `app/build/intermediates/cmake/debug/obj/armeabi-v7a/libdumper.so`

In order to use the dumper you have to first `setenforce 0` on your tablet and then `setprop wrap.com.android.gl2jni LD_PRELOAD=/path/to/libdumper.so`

There's now a frida-based replacement for libdumper, mainly intended to be used together with an SRE tool perform dynamic analysis, for now it does not have all the features of libdumper, to use it
patch the app apk manually or using a tool such as `objection` and then run `npm run build && frida -U Gadget -l _agent.js` in the frida subdirectory

There is now a disassembler under the directory `data`, the exact instruction encoding still isn't fully reversed yet.

Checklist:

- [x] Implement a disassembler
- [ ] Figure out how to run this on postmarketos instead
- [ ] Reverse the instruction decoding fully
- [ ] Determine instruction semantics for the instructions that are commonly used in simple shaders
- [ ] Implement an assembler (an assembler is most likely embedded in the x86 binary)
- [ ] Implement a way of replacing shaders generated by the proprietary driver
- [ ] Reverse the command stream
- [ ] Perform some basic rendering independently of the userspace libgl
