#include <stdio.h>
#include <time.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(){
    srand(time(NULL));
    int j = 0;
    scanf("%d", &j);
    j = 0;
    //dlopen("/data/local/tmp/libinject.so", RTLD_NOW);
    while(1) {
        j++;
        sleep(1);
        printf("hello world: %d\n",rand()%100);

        if (j == 5) {
            void* p = malloc(100);
            int i = 0;
            for (i = 0 ; i < 100 ; i++) {
                ((int*)p)[i] = i*i;
                //printf("p[i]: %d\n", ((int*)p)[i]);
            }
        }
    }
    return 0;
}
