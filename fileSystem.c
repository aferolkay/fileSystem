#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "fileSystem.h"



DISK disk;
block* blocks;
fileItem* fileItems;
FAT* table;

/* works as intented*/
void create(){
    FILE *file;
    file = fopen("disk","w+");
    
    // init File Allocation Table
    table = malloc( sizeof(FAT) );
    memset( table, 0, sizeof(FAT) );
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
    free(blocks);
    free(fileItems);
    free(table);
    printf("A new disk with size %d bytes is created successfully! \n",FILE_END);
}

/* works as intented*/
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

/* works as intented*/
void demount(){
    FILE *file;
    file = fopen("disk","w");
    // store File Allocation Table
    fwrite( table , sizeof(FAT) , 1 , file);
    // store Items List
    for(int i=0 ; i < NUM_OF_ITEMS ; i++ ){
        fwrite( &(fileItems[i]) , sizeof(fileItem) , 1 , file);
    }
    // store Data Blocks
    for(int i=0 ; i < NUM_OF_BLOCKS ; i++ ){
        fwrite( &(blocks[i]) , sizeof(block) , 1 , file);
    }
    fclose(file);
    
    free(blocks);
    free(fileItems);
    free(table);
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

/* works as intented*/
void format(){
    FILE* file_check = fopen("disk", "r");
    if (file_check == NULL) {
        printf("Disk not found!");
        fclose(file_check);
        return;
    }
    fseek(file_check, DATA_START, SEEK_SET);
    blocks = malloc( sizeof(block) * NUM_OF_BLOCKS );
    for(int i=0 ; i < NUM_OF_BLOCKS ; i++ ){
        fread( &(blocks[i]) , sizeof(block) , 1 , file_check);
    }


    FILE* file = fopen("disk", "w");

    // format File Allocation Table
    table = malloc( sizeof(FAT) );
    memset( table, 0, sizeof(FAT) );
    memset( table, 0xFFFFFFFF , sizeof(int32_t));      
    fwrite( table , sizeof(FAT) , 1 , file);

    // format Items List
    fileItems = malloc( sizeof(fileItem) * NUM_OF_ITEMS );
    for(int i=0 ; i < NUM_OF_ITEMS ; i++ ){
        strcpy( fileItems[i].fileName , "NULL");
        fileItems[i].fileSize = 0;
        fileItems[i].firstBlock = 0;
        fwrite( &(fileItems[i]) , sizeof(fileItem) , 1 , file);
    }
    fwrite(blocks , sizeof(block)* NUM_OF_BLOCKS , 1 , file);
    free(blocks);
    free(fileItems);
    free(table);
    fclose(file); // Close the file when finished
}






/* find the first empty slot on file allocation table*/
int32_t emptyAddress(){
    for ( uint32_t i=1 ; i<NUM_OF_ADDRESSES ; i++){
        if( table->address[i] == 0x0 ){
            printf("Empty address space found in slot %d!\n",i);
            return i;
            break;
        }
    }
    printf("No empty space in address table!\n");
    return 0;
}

/* find the first empty slot on file list*/
int32_t emptyFileSlot(){
    for ( uint32_t i=0 ; i<NUM_OF_ITEMS ; i++){
        if( fileItems[i].firstBlock == 0 ){
            printf("Empty space found in file list at slot %d!\n",i);
            return i;
            break;
        }
    }
    printf("No empty space in file list!\n");
    return 0;
}

/* how many bytes is the indicated file*/
int32_t returnFileSize(char* filename){

    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Error opening the file. File may not exist or cannot be accessed.\n");
        return -1;
    }

    // Move the file pointer to the end of the file
    fseek(file, 0, SEEK_END);

    // Get the current position of the file pointer, which corresponds to the file size
    int32_t size = ftell(file);
    if (size == -1) {
        printf("Error getting the file size.\n");
        fclose(file);
        return -1;
    }
    fclose(file);
    return size;

}

void write(char *srcFileName, char *destFileName){
    mount();

    /* calculate how much space the file will occupy*/
    uint32_t sourceFileSize = returnFileSize(srcFileName);
    int numOfBlocksNecessary = (int)ceil ( (double)sourceFileSize / 512.0  );
    FILE* srcFile = fopen(srcFileName,"r");
    if (srcFile == NULL) {
        printf("File with name <%s> not found!\n",srcFileName);
        fclose(srcFile);
        return;
    }

    /* place the necessary file information on the file list*/
    uint32_t fileNumber = emptyFileSlot();
    strcpy( fileItems[fileNumber].fileName , "NEW_FILE"); //destFileName);
    fileItems[fileNumber].fileSize = sourceFileSize;
    int firstBlockAddress=emptyAddress();
    fileItems[fileNumber].firstBlock = firstBlockAddress;
    

    int tempBlockAddress=firstBlockAddress;
    for(int i = 0 ; i<numOfBlocksNecessary ; i++){
        
        fread( &blocks[tempBlockAddress] , sizeof(block) , 1 , srcFile); /* bu son block'ta size tam uyumlu olmadığı için sıkıntı çıkarabilir*/

        if ( i < numOfBlocksNecessary-1 ){
            table->address[tempBlockAddress] = 0xAFEAFEAF; /* dummy variable as a placeholder*/
            table->address[tempBlockAddress] = emptyAddress() ; /* now the true value*/
            tempBlockAddress = table->address[tempBlockAddress];
        }
        else {
            table->address[tempBlockAddress] = 0xFFFFFFFF ; /* last block is set to FFs*/
        }
        //printf( "Block #%d has been placed into address %d with %d bytes. i: %d | numOfBlocksNecessary: %d\n", i, tempBlockAddress, sizeof(block), i, numOfBlocksNecessary );
    }
    printf("The file<%s> has been successfully saved to disk as <%s>\n", srcFileName,destFileName );
    fclose(srcFile);
    
    printFAT();
    printFileList();
    
    demount();
}


int main(int argc , char* arg[]){
    //format();
    //create();
    //write("schedule.jpeg", "dersProgrami");
  
     
  mount();
  printFAT();
  printFileList();
  demount();

    return 0; 
}
