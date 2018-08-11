# EHook

A hooking framework for commerical Android emulators ([雷电模拟器](http://www.ldmnq.com/), [NOX](https://jp.bignox.com/), [BlueStacks](https://www.bluestacks.com/ja/index.html#gref)).

As you may know, these commercial emulators use x86 (kernel) and arm (user land) mixed-mode method to accelerate the emulation and this makes it difficult to hook the native arm libraries. This emulator hooking framework is intented to solve this problem.


# Usage

1. Modify the jni/realinject.c file. You can hook functions both by address or by the function's symbol name:

```
    static struct hook_t eph1;
    static struct hook_t eph_sendto;
    ...
    ...
    
    /*On BlueStacks*/
    //hook_by_addr(&eph1, "arm/libc.so", target_addr, hook_func1);
    
    /*Other emulators*/
    hook_by_name(&eph_sendto, "nb/libc.so", "sendto", hook_sendto);
```
**Notice:** If you are hooking system libraries like "libc.so", please make sure you are giving the right path of the arm version binary.
    
2. Run `ndk-build`

3. Push the following 3 binary to android /data/local/tmp directory: 
    `libs/x86/stalker` 
    `libs/x86/libinject.so`
    `libs/armeabi/librealinject.so`

Or just run `push.cmd`.

4. On android, type `./stalker [pid]`.

# Notice
1. For NOX emulator, you should use the Android 5.1.1 instead of the default 4.4.2.
2. Also see this repo: [XEHook](https://github.com/nevermoe/XEHook).

# acknowledgement

Thanks to these 大神s' work. They made my life much easier.
https://github.com/zhengmin1989/TheSevenWeapons/tree/master/LiBieGou

https://github.com/ele7enxxh/Android-Inline-Hook
