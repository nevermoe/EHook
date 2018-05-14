#include "hook.h"
#include "relocate.h"


void get_module_range(pid_t pid, const char* module_name, uint32_t* start_addr, uint32_t* end_addr)
{
    FILE *fp;
    char *pch;
    char filename[32];
    char line[1024];
    *start_addr = 0;
    if (end_addr) {
        *end_addr = 0;
    }

    if (pid == 0) {
        snprintf(filename, sizeof(filename), "/proc/self/maps");
    } else {
        snprintf(filename, sizeof(filename), "/proc/%d/maps", pid);
    }

    fp = fopen(filename, "r");

    if (fp != NULL) {
        while (fgets(line, sizeof(line), fp)) {
            //LOGD("line: %s", line);
            if (strstr(line, module_name)) {
                pch = strtok(line, "-");
                *start_addr = strtoul(pch, NULL, 16);
                //LOGD("pch start: %s start_addr: %x\n", pch, *start_addr);
                pch = strtok(NULL, " ");
                if (end_addr) {
                    *end_addr = strtoul(pch, NULL, 16);
                    //LOGD("pch end: %s end_addr: %x\n", pch, *end_addr);
                }

                /*
                if (*start_addr == 0x8000) {
                    *start_addr -= 0x8000;

                    if (end_addr)
                        *end_addr -= 0x8000;
                }
                */
                break;
            }
        }

        fclose(fp) ;
    }
}

int hook_by_name(struct hook_t *h, char* module_name, const char* func_name, void *hook_func)
{
    int ret = 0;
    void *handle = dlopen(module_name, RTLD_NOW);
    void *func_addr = dlsym(handle, func_name);

    //get module range for self process
    uint32_t module_start_addr = 0, module_end_addr = 0;
    get_module_range(0, module_name, &module_start_addr, &module_end_addr);
    if(module_start_addr == 0 || module_end_addr == 0) {
        LOGD("HOOK_ERROR_SO_NOT_FOUND: %s\n", module_name);
        return HOOK_ERROR_SO_NOT_FOUND;
    }

    //mprotect
    ret = mprotect((void*)module_start_addr, module_end_addr - module_start_addr, 
            PROT_READ|PROT_WRITE|PROT_EXEC);
    if (ret) {
        LOGD("mprotect failed\n");
        return HOOK_ERROR_MPROTECT_FAILED;
    }

    h->module_base = module_start_addr;

    return _hook(h, (unsigned int)func_addr, hook_func);
}

int hook_by_addr(struct hook_t *h, char* module_name, unsigned int addr, void *hook_func)
{
	int i;
	
    uint32_t module_start_addr = 0, module_end_addr = 0;
    get_module_range(0, module_name, &module_start_addr, &module_end_addr);
    if(module_start_addr == 0 || module_end_addr == 0) {
        LOGD("HOOK_ERROR_SO_NOT_FOUND: %s\n", module_name);
        return HOOK_ERROR_SO_NOT_FOUND;
    }
    unsigned int func_addr = module_start_addr + addr;
    
    //mprotect
    int ret = mprotect((void*)module_start_addr, module_end_addr - module_start_addr, 
            PROT_READ|PROT_WRITE|PROT_EXEC);
    if (ret) {
        LOGD("mprotect failed\n");
        return HOOK_ERROR_MPROTECT_FAILED;
    }

    h->module_base = module_start_addr;

    return _hook(h, (unsigned int)func_addr, hook_func);
}

static int _hook(struct hook_t *h, unsigned int addr, void *hook_func)
{
	
    LOGD("[%d] hooking: 0x%x\n", getpid(), addr - h->module_base);
    h->new_addr = (unsigned int)hook_func;
    h->target_addr = addr;

    h->length = TEST_BIT0(h->target_addr) ? 12 : 8;
    memcpy(h->orig_instructions, (void *) CLEAR_BIT0(h->target_addr), h->length);
    
    //modify function entry point
    if (!TEST_BIT0(h->target_addr)) {
        //ARM mode
        LOGD("using ARM mode 0x%lx\n", (unsigned long)hook_func);
        h->thumb = 0;
        h->proto = h->trampoline_instructions;
        ((uint32_t *)h->target_addr)[0] = 0xe51ff004; // LDR pc, [pc, #-4]
        ((uint32_t *)h->target_addr)[1] = h->new_addr;
    }
    else {
        //thumb mode
        LOGD("using thumb mode 0x%lx\n", (unsigned long)hook_func);

        int i = 0;
        if (CLEAR_BIT0(h->target_addr) % 4 != 0) {
            ((uint16_t *) CLEAR_BIT0(h->target_addr))[i++] = 0xBF00;  // NOP
        }

        h->thumb = 1;
        h->proto = (uint32_t *)SET_BIT0((uint32_t)h->trampoline_instructions);
        ((uint16_t *) CLEAR_BIT0(h->target_addr))[i++] = 0xF8DF;
        ((uint16_t *) CLEAR_BIT0(h->target_addr))[i++] = 0xF000; // LDR.W PC, [PC]
        ((uint16_t *) CLEAR_BIT0(h->target_addr))[i++] = h->new_addr & 0xFFFF;
        ((uint16_t *) CLEAR_BIT0(h->target_addr))[i++] = h->new_addr >> 16;

    }

    hook_cacheflush(CLEAR_BIT0(h->target_addr), CLEAR_BIT0(h->target_addr)+h->length);

    relocateInstruction(h->target_addr, h->orig_instructions, h->length, h->trampoline_instructions, h->orig_boundaries, h->trampoline_boundaries, &h->count);

    //addr must align to page (4kb)
    int ret = mprotect((void*)((int)h->trampoline_instructions & 0xFFFFF000), 0x1000, 
            PROT_READ|PROT_WRITE|PROT_EXEC);

    if (ret != 0) {
        LOGD("mprotect failed: %d\ttrampoline_addr: 0x%x\n", ret, (uint32_t)h->trampoline_instructions);
        ret = HOOK_ERROR_MPROTECT_FAILED;
    }

	return ret;

}


void inline hook_cacheflush(unsigned int begin, unsigned int end)
{	
#if 0
#if defined(__arm__)    
	const int syscall = 0xf0002;

	__asm __volatile (
		"mov	 r0, %0\n"			
		"mov	 r1, %1\n"
		"mov	 r7, %2\n"
		"mov     r2, #0x0\n"
		"svc     0x00000000\n"
		:
		:	"r" (begin), "r" (end), "r" (syscall)
		:	"r0", "r1", "r7"
		);
#endif
#endif
}

/*
void hook_unset_jump(struct hook_t *h)
{
    int i;
    
    if (h->thumb) {
        unsigned int orig = h->orig - 1;
        for (i = 0; i < sizeof(h->storet); i++) {
            ((unsigned char*)orig)[i] = h->storet[i];
        }
    }
    else {
        for (i = 0; i < sizeof(h->store)/4; i++)
            ((unsigned int*)h->orig)[i] = h->store[i];
    }   
    hook_cacheflush((unsigned int)h->orig, (unsigned int)h->orig+sizeof(h->jumpt));
}
*/

/*
void hook_set_jump(struct hook_t *h)
{
    int i;

    if (h->thumb) {
        unsigned int orig = h->orig - 1;
        for (i = 0; i < sizeof(h->jumpt); i++)
            ((unsigned char*)orig)[i] = h->jumpt[i];
    }
    else {
        for (i = 0; i < sizeof(h->jump)/sizeof(unsigned int); i++)
            ((int*)h->orig)[i] = h->jump[i];
    }
    hook_cacheflush((unsigned int)h->orig, (unsigned int)h->orig+sizeof(h->jumpt));
}
*/

