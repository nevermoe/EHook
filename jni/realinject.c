#include "hook.h"
#include <android/log.h>

static struct hook_t eph_sendto;
static struct hook_t eph_recvfrom;

int hook_sendto(int p0,int p1,int p2,int p3,int p4,int p5,int p6,int p7,int p8,int p9,int p10,int p11,int p12,int p13,int p14,int p15,int p16,int p17,int p18,int p19)
{
	int (*orig_func)(int p0,int p1,int p2,int p3,int p4,int p5,int p6,int p7,int p8,int p9,int p10,int p11,int p12,int p13,int p14,int p15,int p16,int p17,int p18,int p19);
	orig_func = (void*)eph_sendto.proto;
    
    LOGD("hook_sendto 0x%x\n", (unsigned int)(eph_sendto.target_addr-eph_sendto.module_base));

	int ret = orig_func(p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19);

    return ret;
}

int hook_recvfrom(int p0,int p1,int p2,int p3,int p4,int p5,int p6,int p7,int p8,int p9,int p10,int p11,int p12,int p13,int p14,int p15,int p16,int p17,int p18,int p19)
{
	int (*orig_func)(int p0,int p1,int p2,int p3,int p4,int p5,int p6,int p7,int p8,int p9,int p10,int p11,int p12,int p13,int p14,int p15,int p16,int p17,int p18,int p19);
	orig_func = (void*)eph_recvfrom.proto;
    
    LOGD("hook_recvfrom %x\n", (unsigned int)(eph_recvfrom.target_addr-eph_recvfrom.module_base));

	int ret = orig_func(p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19);

    return ret;
}


static void real_init_func() __attribute__((constructor));

void real_init_func() 
{
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
}

