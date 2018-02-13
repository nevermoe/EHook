#include "hook.h"
#include <android/log.h>

//static struct hook_t eph1;

typedef void *native_load_library(void *handle, const char *symbol);
typedef void *native_bridge_get_trampoline(void *handle, const char *symbol_name, const char* shorty, uint32_t length);
typedef int real_init_func(char * str);

#if 0
int hook_func1()
{
	int (*orig_func)();
	orig_func = (void*)eph1.proto;
    //LOGD("proto: 0x%x \n", (unsigned int)(eph1.proto));
    //sleep(10);
    
	LOGD("func 0x%x call begin.\n", (unsigned int)(eph1.target_addr-eph1.module_base));

	int ret = orig_func();

	LOGD("func 0x%x call end.\n", (unsigned int)(eph1.target_addr-eph1.module_base));

    return ret;
}

uint32_t get_entry_addr(const char* lib_name, uint32_t entry_addr)
{
    uint32_t module_start_addr = 0, module_end_addr = 0;
    get_module_range(0, lib_name, &module_start_addr, &module_end_addr);
    int ret = mprotect((void*)module_start_addr, module_end_addr - module_start_addr, 
            PROT_READ|PROT_WRITE|PROT_EXEC);
    if (ret) {
        LOGD("mprotect failed\n");
        exit(-1);
    }
    uint32_t target_addr = module_start_addr + entry_addr;
    return target_addr;
}

uint32_t get_hook_func()
{
    void* hook_func = mmap(0, 0x4000, 
            PROT_READ | PROT_WRITE | PROT_EXEC /*WRX*/,
            MAP_ANONYMOUS | MAP_PRIVATE/*flag*/, 0, 0);

    uint32_t module_start_addr = 0, module_end_addr = 0;
    get_module_range(0, "nb/libc.so", &module_start_addr, &module_end_addr);

    uint32_t str_offset = 0x1000;
    char *str = "/data/local/tmp/libarm.so\n";
    strcpy(((char*)hook_func)+str_offset, str);

    uint32_t flag_offset = 0x1100;
    ((uint32_t*)hook_func)[flag_offset] = RTLD_NOW;


    uint32_t dlopen_offset = 0x1200;
    ((uint32_t*)hook_func)[dlopen_offset] = 0xFC90/*dlopen.plt*/ + module_start_addr;

    
    ((uint32_t*)hook_func)[0] = 0xe92d4001;//STMFD SP!, {R0,LR}
    //((uint32_t*)hook_func)[1] = 0xe92d0003;//PUSH {R0, R1}
    ((uint32_t*)hook_func)[1] = 0xe28f0eff;//ADD R0, PC, #0x1000-#0x0C (=#0x)
    ((uint32_t*)hook_func)[2] = ;//LDR R1, PC, #0x1100-#0x10 (=#0x)
    ((uint32_t*)hook_func)[3] = ;//LDR R2, PC, #0x1200-#0x14 (=#0x)
    ((uint32_t*)hook_func)[4] = ;//BLX R2   (Call dlopen)
    ((uint32_t*)hook_func)[5] = ;//LDMFD SP!, {R0,PC}

    
    return 0;
}
#endif

int init_func(char * str){
    LOGD("%s, hook in pid = %d\n", str, getpid());

    long target_addr = 0;

    //target_addr = 0x400a0;//lrand48
    //target_addr = 0x590f0;//sendto
    //hook_by_addr(&eph1, "nb/libc.so", target_addr, hook_func1);

#if 0
    uint32_t entry_addr = get_entry_addr("nb/libc.so", 0x11640);//malloc
    uint32_t hook_func = get_hook_func();
    _hook(&eph1, entry_addr, (void*)hook_func);
#endif
    
    void* libnativebridge = 0, *NativeBridgeLoadLibrary = 0, 
        *NativeBridgeGetTrampoline = 0, 
        *librealinject = 0, *RealInitFunc= 0;
    libnativebridge = dlopen("/system/lib/libnativebridge.so", RTLD_NOW);
    LOGD("libnativebridge.so handler addr is %p\n", libnativebridge);


    NativeBridgeLoadLibrary = dlsym(libnativebridge, "_ZN7android23NativeBridgeLoadLibraryEPKci");
    LOGD("NativeBridgeLoadLibrary addr is %p\n", NativeBridgeLoadLibrary);

    librealinject = ((native_load_library*)NativeBridgeLoadLibrary)("/data/local/tmp/librealinject.so", RTLD_NOW);
    LOGD("librealinject.so handler addr is %p\n", librealinject);

    NativeBridgeGetTrampoline = dlsym(libnativebridge, "_ZN7android25NativeBridgeGetTrampolineEPvPKcS2_j");
    LOGD("NativeBridgeGetTrampoline addr is %p\n", NativeBridgeGetTrampoline);

#if 0
    //RealInitFunc = ((native_bridge_get_trampoline*)NativeBridgeGetTrampoline)(librealinject, "real_init_func", 0, 0);
    RealInitFunc = dlsym(librealinject, "real_init_func");
    LOGD("RealInitFunc addr is %p\n", RealInitFunc);

    ((real_init_func*)RealInitFunc)("HelloWorld");
#endif

    LOGD("init_func ended");

    return 0;
}
