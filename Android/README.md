All of the Android reverse engineering is done out on the Cyanogenmod-based Beanstalck android 6.0 rom version `6.15-20160430-espressowifi`

The tools in this directory can be built using the android studio IDE, after building the project the testing apk will be located under `app/build/outputs/apk/debug/app-debug.apk` and the intercept tool will be under `app/build/intermediates/cmake/debug/obj/armeabi-v7a/libdumper.so`

In order to use the dumper you have to first `setenforce 0` on your tablet and then `setprop wrap.com.android.gl2jni LD_PRELOAD=/path/to/libdumper.so`

There's now a frida-based partial replacement for libdumper, mainly intended to be used together with an SRE tool perform dynamic analysis, for now it does not have all the features of libdumper, to use it
patch the app apk manually or using a tool such as `objection` and then run `npm run build && frida -U Gadget -l _agent.js` in the frida subdirectory
