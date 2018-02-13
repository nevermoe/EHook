#ifndef __HOOK_H__
#define __HOOK_H__

#include <stdio.h>
#include <stdlib.h>
//#include <asm/user.h>
#include <sys/user.h>
#include <asm/ptrace.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <elf.h>
#include <android/log.h>


#if defined(__i386__)    
#define pt_regs         user_regs_struct    
#endif    

#define LOG_TAG "MY_HOOK"
#define LOGD(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, fmt, ##args)
#define LOGF(...) \
        {FILE *fp = fopen("/data/local/tmp/hook.log", "a+"); if (fp) {\
        fprintf(fp, __VA_ARGS__);\
        fclose(fp);}}


#define SET_BIT0(addr)      (addr | 1)
#define CLEAR_BIT0(addr)    (addr & 0xFFFFFFFE)
#define TEST_BIT0(addr)     (addr & 1)

#define ORIG_INS_SIZE           3
#define TRAMPOLINE_INS_SIZE     10
#define ORIG_BOUND_SIZE         4
#define TRAMPOLINE_BOUND_SIZE   20

struct hook_t {
    uint32_t target_addr;
    uint32_t new_addr;
    uint32_t orig_instructions[ORIG_INS_SIZE];
    uint32_t trampoline_instructions[TRAMPOLINE_INS_SIZE];
    int orig_boundaries[ORIG_BOUND_SIZE];
    int trampoline_boundaries[TRAMPOLINE_BOUND_SIZE];
    uint32_t *proto;
    int count;
    int length;
    unsigned int thumb;
    unsigned int module_base;
};

enum hook_status {
    HOOK_OK = 0,
    //HOOK_ERROR_NOT_EXECUTABLE,
    HOOK_ERROR_SO_NOT_FOUND,
    HOOK_ERROR_MPROTECT_FAILED,
    HOOK_ERROR_UNKNOWN
};

void get_module_range(pid_t pid, const char* module_name, uint32_t* start_addr, uint32_t* end_addr);

int hook_by_addr(struct hook_t *h, char* module_name, unsigned int addr, void *hook_func);
int hook_by_name(struct hook_t *h, char* module_name, const char* func_name, void *hook_func);

static int _hook(struct hook_t *h, unsigned int addr, void *hook_func);

void inline hook_cacheflush(unsigned int begin, unsigned int end);

void hook_unset_jump(struct hook_t *h);
void hook_set_jump(struct hook_t *h);

#endif
