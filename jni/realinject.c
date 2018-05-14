#include "hook.h"
#include <android/log.h>

static struct hook_t eph;

int hook_battleEnd(int p1, int p2)
{
	int (*orig_func)(int p1, int p2);
	orig_func = (void*)eph.proto;
    
    LOGD("hooking: 0x%x\n", (unsigned int)(eph.target_addr-eph.module_base));

    p2 = 1;
	int ret = orig_func(p1, p2);

    return ret;
}

static void real_init_func() __attribute__((constructor));

void real_init_func() 
{
#if 0
    LOGD("real_init_func called\n");

    long target_addr = 0;

    target_addr = 0x590f0;//sendto

    /*Bluestacks*/
    //hook_by_addr(&eph_sendto, "arm/libc.so", target_addr, hook_sendto);
    //hook_by_name(&eph_recvfrom, "arm/libc.so", "recvfrom", hook_recvfrom);

    /*Other emulators*/
    hook_by_addr(&eph_sendto, "nb/libc.so", target_addr, hook_sendto);
    hook_by_name(&eph_recvfrom, "nb/libc.so", "recvfrom", hook_recvfrom);

    LOGD("real_init_func ended\n");
#endif
    LOGD("real_init_func called\n");

    long target_addr = 0;

    target_addr = 0x177335;

    hook_by_addr(&eph, "libcocos2dcpp.so", target_addr, hook_battleEnd);

    LOGD("real_init_func ended\n");
}

