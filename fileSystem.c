#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "fileSystem.h"

/*
struct block{
    char data[512];
};
typedef struct block block;

struct fileItem{
    char fileName[248];
    uint32_t firstBlock;
    uint32_t fileSize;
};
typedef struct fileItem fileItem;

struct FAT{
    int32_t address[4096];
};
typedef struct FAT FAT;

struct DISK{
    int num_fileItem;
    int num_dataBlocks;
};
typedef struct DISK DISK;
*/

DISK disk;
block* blocks;
fileItem* fileItems;
FAT* table;

void create(){
    FILE *file;
    file = fopen("disk","w+");

    // init File Allocation Table
    table = malloc( sizeof(FAT) );
    memset( table, 0, sizeof(FAT) );
    memset( table, 0xFFFFFFFF , sizeof(int32_t));      
    fwrite( table , sizeof(FAT) , 1 , file);

    // init Items List
    fileItems = malloc( sizeof(fileItem) * NUM_OF_ITEMS );
    for(int i=0 ; i < NUM_OF_ITEMS ; i++ ){
        strcpy( fileItems[i].fileName , "NULL");
        fileItems[i].fileSize = 0;
        fileItems[i].firstBlock = 0;
        fwrite( &(fileItems[i]) , sizeof(fileItem) , 1 , file);
    }

    // init Data Blocks
    blocks = malloc( sizeof(block) * NUM_OF_BLOCKS );
    memset( blocks, 0, sizeof(sizeof(block) * NUM_OF_BLOCKS) );
    for(int i=0 ; i < NUM_OF_BLOCKS ; i++ ){
        fwrite( &(blocks[i]) , sizeof(block) , 1 , file);
    }

    fclose(file);
}

void mount(){
    FILE *file;
    file = fopen("disk","r");

    // init File Allocation Table
    table = malloc( sizeof(FAT) );
    fread( table , sizeof(FAT) , 1 , file);

    // init Items List
    fileItems = malloc( sizeof(fileItem) * NUM_OF_ITEMS );
    for(int i=0 ; i < NUM_OF_ITEMS ; i++ ){
        fread( &(fileItems[i]) , sizeof(fileItem) , 1 , file);
    }

    // init Data Blocks
    blocks = malloc( sizeof(block) * NUM_OF_BLOCKS );
    for(int i=0 ; i < NUM_OF_BLOCKS ; i++ ){
        fread( &(blocks[i]) , sizeof(block) , 1 , file);
    }

    fclose(file);

}



void printFAT(){
    printf("Entry\t\tValue\t\tEntry\t\tValue\t\tEntry\t\tValue\t\tEntry\t\tValue\n");
    for( int i = 0 ; i < 512 ; i++){
        (i%4==3) ?  printf("%d\t\t%X\t\t\n",i, table->address[i] ) : printf("%d\t\t%X\t\t",i, table->address[i] ) ;  
    }
}
 
void printFileList(){
    printf("Item\t\tFile name\t\tFirst block\t\tFile size (Bytes)\n");
    for(int i=0 ; i < NUM_OF_ITEMS ; i++ ){
        printf("%d\t\t%s\t\t%d\t\t%d\n" , i , fileItems[i].fileName , fileItems[i].firstBlock , fileItems[i].fileSize );
    }
}



int main(int argc , char* arg[]){
    create();
    printFAT();
    printFileList();
    
    free(blocks);
    free(fileItems);
    free(table);
    return 0;
}
