//Allocate a 10GB binary file with sequencial write before disk benchmark experience

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main()
	{
    
//create a new file
    
    FILE *new_file;
    
    
//open a binary file for writhing in binary mode, if file exists, rewrite it; else create new file.
    new_file = fopen("new_file.bin", "wb");

//disable the buffer so that following process are executed directly to disk
    setbuf(new_file, NULL);

    if(new_file == NULL){
          printf("Cannot open a binary file!!");
          exit(1);
   
}

    int block_size = 8000;

//create a string with size of block_size and fill out memory
    char * str = (char *)malloc(block_size);
    memset(str, 0, sizeof(str));

//write the string to the new file in multiple times; and the number of times of writing inversely propotional o the block size.
//This file size is 10GB, and we will do read+write, sequencial read and random read within the file

int i;
    for (i = 0; i < 10000000000 / block_size; i++) {
       fwrite(str, 1, block_size, new_file);
    }

    fclose(new_file);	//close the new_file
    free(str);     //free cache
    
    return (0);
}
