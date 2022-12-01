#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(){


	int fd;
	int fd2;
	char str[100];
	char str1[100];
	int n;

	/*while((fd = open("/tmp/fifo_teste", O_WRONLY))== -1){
	  if(mkfifo("/tmp/fifo_teste", 0666)!=0){
			printf("problem creating the fifo\n");
			exit(-1);
	  }else{
		  printf("fifo created\n");
	  }
	}
	while((fd2 = open("/tmp/fifo_teste2", O_RDONLY))== -1){
			if(mkfifo("/tmp/fifo_teste2", 0666)!=0){
					printf("problem creating the fifo\n");
					exit(-1);
			}else{
				printf("fifo created\n");
			}
		}*/
	

    while(1){
		while((fd=open("/tmp/c2s",O_WRONLY))==-1){
		}
		printf("write a string:");
		fgets(str, 100, stdin);
		write(fd, str, 100);
		fd2 = open("/tmp/s2c", O_RDONLY);
		n=read(fd2,str1,100);
		if(n<=0){
			perror("read ");
			exit(-1);
		}
		else{
			printf("%s\n",str1);
		}
		close(fd2);

		/*printf("write a number:");
		fgets(str, 100, stdin);
		sscanf(str, "%d", &n);
		write(fd, &n, sizeof(n));*/

	}
}