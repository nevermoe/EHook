#include "hook.h"
#include <android/log.h>

typedef void *native_load_library(void *handle, const char *symbol);
typedef void *native_bridge_get_trampoline(void *handle, const char *symbol_name, const char* shorty, uint32_t length);

typedef void* dvm2_hd_dlopen(const char* filename, int flag);

typedef int real_init_func(char * str);


void load_real_lib_bluestacks()
{
    void* dvm2hdDlopen = 0, *librealinject = 0, *lib3btrans = 0;

    lib3btrans = dlopen("/system/lib/lib3btrans.so", RTLD_NOW);
    LOGD("lib3btrans.so handler addr is %p\n", lib3btrans);

    dvm2hdDlopen = dlsym(lib3btrans, "dvm2hdDlopen");
    LOGD("dvm2hdDlopen addr is %p\n", dvm2hdDlopen);

    librealinject = ((dvm2_hd_dlopen*)dvm2hdDlopen)("/data/local/tmp/librealinject.so", RTLD_NOW);
    LOGD("librealinject.so handler addr is %p\n", librealinject);

}

void load_real_lib_4_4_2()
{
    void* dvm2hdDlopen = 0, *librealinject = 0, *libhoudini = 0;

    libhoudini = dlopen("/system/lib/libhoudini.so", RTLD_NOW);
    LOGD("libhoudini.so handler addr is %p\n", libhoudini);

    dvm2hdDlopen = dlsym(libhoudini, "dvm2hdDlopen");
    LOGD("dvm2hdDlopen addr is %p\n", dvm2hdDlopen);

    librealinject = ((dvm2_hd_dlopen*)dvm2hdDlopen)("/data/local/tmp/librealinject.so", RTLD_NOW);
    LOGD("librealinject.so handler addr is %p\n", librealinject);
}

void load_real_lib_5_5_1()
{
    void* libnativebridge = 0, *NativeBridgeLoadLibrary = 0, 
        *librealinject = 0;
    libnativebridge = dlopen("/system/lib/libnativebridge.so", RTLD_NOW);
    LOGD("libnativebridge.so handler addr is %p\n", libnativebridge);

    NativeBridgeLoadLibrary = dlsym(libnativebridge, "_ZN7android23NativeBridgeLoadLibraryEPKci");
    LOGD("NativeBridgeLoadLibrary addr is %p\n", NativeBridgeLoadLibrary);

    librealinject = ((native_load_library*)NativeBridgeLoadLibrary)("/data/local/tmp/librealinject.so", RTLD_NOW);
    LOGD("librealinject.so handler addr is %p\n", librealinject);

#if 0
    void *NativeBridgeGetTrampoline = 0, *RealInitFunc = 0;
    NativeBridgeGetTrampoline = dlsym(libnativebridge, "_ZN7android25NativeBridgeGetTrampolineEPvPKcS2_j");
    LOGD("NativeBridgeGetTrampoline addr is %p\n", NativeBridgeGetTrampoline);

  	RealInitFunc = ((native_bridge_get_trampoline*)NativeBridgeGetTrampoline)(librealinject, "real_init_func", 0, 0);
    LOGD("RealInitFunc addr is %p\n", RealInitFunc);

    ((real_init_func*)RealInitFunc)("HelloWorld");
#endif
}


int init_func(char * str){
    LOGD("%s, hook in pid = %d\n", str, getpid());

    char* nb_file = "/system/lib/libnativebridge.so";
    char* dvm_file = "/system/lib/libdvm.so";
    char* btrans_file = "/system/lib/lib3btrans.so";
    if( access(nb_file, F_OK) != -1 ) {
        // native bridge exists, Android 5.5.1
        load_real_lib_5_5_1();
    } else if ( access(btrans_file, F_OK) != -1){
        // BlueStacks
        load_real_lib_bluestacks();
    } else { //dvm_file exsits
        //Android 4.4.2, maybe NOX
        load_real_lib_4_4_2();
    }


    LOGD("init_func ended");

    return 0;
}
