#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "funcs.h"

int main(){

    void *handle;
	int fd;
    int fd2;

    mkfifo("/tmp/s2c",0666);
    mkfifo("/tmp/c2s",0666);

	/*while((fd = open("/tmp/fifo_teste", O_RDONLY))== -1){
	  if(mkfifo("/tmp/fifo_teste", 0666)!=0){
			printf("problem creating the fifo\n");
			exit(-1);
	  }else{
		  printf("fifo created\n");
	  }
	}
    
    while((fd2 = open("/tmp/fifo_teste2", O_WRONLY))== -1){
                if(mkfifo("/tmp/fifo_teste2", 0666)!=0){
                        printf("problem creating the fifo\n");
                        exit(-1);
                }else{
                    printf("fifo created\n");
                }
            }*/
	printf("fifo just opened\n");

    char str[100];
    int n;
    int (*func)(void);
    handle =dlopen("./funcs.so", RTLD_LAZY);
    if (handle==NULL){
        printf("Library not loaded\n");
        exit(-1);
    }
    else {
        printf("Library was loaded\n");
    }

    while(1){
        fd=open("/tmp/c2s",O_RDONLY);
        
        n=read(fd,str,100);
        str[strlen(str)-1] = '\0';
        if(n<=0){
			perror("read ");
			exit(-1);
		}
        printf("Str %s\n", str);
        func=dlsym(handle, str);
        if (func == NULL) {
        printf("Func a NULL\n");
        fd2 = open("/tmp/s2c", O_WRONLY);
        write(fd2, "no function found", 100);
        close(fd2);
        }
        else{
            printf("Func a não NULL\n");
            printf("func returns %d\n",func());

            
            int result = func();
            char* str3 = malloc(100*sizeof(char));
            snprintf( str3, 3, "%d", result);
            printf("teste de str3: %s \n", str3);
            fd2 = open("/tmp/s2c", O_WRONLY);
            write(fd2, str3, 100);
            free(str3);
            close(fd2);
        }
        

        
    }
}