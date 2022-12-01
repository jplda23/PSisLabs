#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]){

int i;
char result_string[500];

for(int i = 0; i < argc; i++) {

   strcat(result_string, argv[i]);

}
    

printf("%s \n", result_string);

}