#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "fileSystem.h"


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

/* reads the disk into memory */
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

/* saves the memory to the disk */
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

/* printf FAT into fat.txt */
void printFAT(){
    FILE* file = fopen("fat.txt","w");
    char buffer[80];

    sprintf(buffer,"Entry\t\tValue\t\tEntry\t\tValue\t\tEntry\t\tValue\t\tEntry\t\tValue\n");
    fprintf(file,"%s",buffer);
    //printf("Entry\t\tValue\t\tEntry\t\tValue\t\tEntry\t\tValue\t\tEntry\t\tValue\n");
    for( int i = 0 ; i < 4096 ; i++){
        uint8_t* bytePtr = (uint8_t*) &(table->address[i]) ;
        (i%4==3) ?  sprintf(buffer,"%d\t\t0x%02X%02X%02X%02X\t\n",i, bytePtr[0],bytePtr[1],bytePtr[2],bytePtr[3]) : sprintf(buffer,"%d\t\t0x%02X%02X%02X%02X\t",i, bytePtr[0],bytePtr[1],bytePtr[2],bytePtr[3] ) ;
        fprintf(file,"%s",buffer);
    }
    fclose(file);
    printf(" File Allocation Table of the disk has been successfully written to the file <fat.txt>\n");
}

/* printf file list into filelist.txt */
void printFileList(){
    FILE* file = fopen("filelist.txt","w");
    char buffer[300];

    sprintf(buffer,"Item\t\tFile name\tFirst block\tFile size (Bytes)\n");
    fprintf(file,"%s",buffer);
    for(int i=0 ; i < NUM_OF_ITEMS ; i++ ){
        sprintf(buffer,"%d\t\t%s\t\t%d\t\t%d\n" , i , fileItems[i].fileName , fileItems[i].firstBlock , fileItems[i].fileSize );
        fprintf(file,"%s",buffer);
    }
    fclose(file);
    printf(" File List informaton of the disk has been successfully written to the file <filelist.txt>\n");
}

/* works as intented*/
void format(){
    FILE* file_check = fopen("disk", "r");
    if (file_check == NULL) {
        printf("Disk not found!\n");
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
    printf("Disk has been formatted successfully! \n");
}

/* find the first empty slot on file allocation table*/
int32_t emptyAddress(){
    for ( uint32_t i=1 ; i<NUM_OF_ADDRESSES ; i++){
        if( table->address[i] == 0x0 ){
            //printf("Empty address space found in slot %d!\n",i);
            return i;
            break;
        }
    }
    printf("No empty space in address table!\n");
    return 0;
}

/* find the first empty slot on a give FAT , intented as an function overload to emptyAdress()*/
int32_t emptyAddressNew(FAT* tableNew){
    for ( uint32_t i=1 ; i<NUM_OF_ADDRESSES ; i++){
        if( tableNew->address[i] == 0x0 ){
            //printf("Empty address space found in slot %d!\n",i);
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
            //printf("Empty space found in file list at slot %d!\n",i);
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

/* copies a file to the disk */
void write(char *srcFileName, char *destFileName){
    mount();

    /* calculate how much space the file will occupy*/
    uint32_t sourceFileSize = returnFileSize(srcFileName);
    int numOfBlocksNecessary = (int) ceil( (double)sourceFileSize / 512.0  );
    FILE* srcFile = fopen(srcFileName,"r");
    if (srcFile == NULL) {
        printf("File with name <%s> not found!\n",srcFileName);
        fclose(srcFile);
        return;
    }

    /* place the necessary file information on the file list*/
    uint32_t fileNumber = emptyFileSlot();
    strcpy( fileItems[fileNumber].fileName , destFileName); //destFileName);
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

    demount();
}

/* copies a file from the disk */
void read(char *srcFileName, char *destPath){
    int isSameArray=-1;
    int fileNo=-1;
    uint32_t tempBlockAddress = 0;

    mount();

    /* find where does the srcFile reside in disk */
    for( int i=0 ; i < NUM_OF_ITEMS ; i++){
        isSameArray = strcmp(srcFileName , fileItems[i].fileName );
        if (isSameArray == 0){
            fileNo = i;
            break;
        }
    }
    /* if file do not exist, return error */
    if (fileNo == -1){
        printf(" <%s> can't be found in the disk! \n",srcFileName);
        demount();
        return;
    }

    /* collect all necessary data blocks*/
    block* fileData;
    int numOfBlocksNecessary = (int) ceil( (double)(fileItems[fileNo].fileSize) / 512.0  );
    fileData = malloc( sizeof(block) * numOfBlocksNecessary );

    tempBlockAddress = fileItems[fileNo].firstBlock;
    for( int blockNo=0 ; blockNo<numOfBlocksNecessary ; blockNo++) {
        // printf("blockNo is from tempBlockAddress : %d <--- %d \n", blockNo , tempBlockAddress);
        fileData[blockNo] = blocks[tempBlockAddress];
        tempBlockAddress = table->address[tempBlockAddress];
    }

    FILE* outputFile = fopen(destPath,"w");
    fwrite( fileData , fileItems[fileNo].fileSize , 1 , outputFile);
    printf("The file <%s> is read from disk as <%s> to the current directory successfully! \n",srcFileName,destPath);
    demount();

}

/* deletes a file if exists on the disk */
void delete(char* filename){
    mount();

    int isSameArray=-1;
    int fileNo=-1;
    /* find where does the srcFile reside in disk */
    for( int i=0 ; i < NUM_OF_ITEMS ; i++){
        isSameArray = strcmp(filename , fileItems[i].fileName );
        if (isSameArray == 0){
            fileNo = i;
            break;
        }
    }
    /* if file do not exist, return error */
    if (fileNo == -1){
        printf(" <%s> can't be found in the disk! \n",filename);
        demount();
        return;
    }

    /* locate all necessary addresses to be cleared*/

    int numOfBlocksNecessary = (int) ceil( (double)(fileItems[fileNo].fileSize) / 512.0  );
    uint32_t tempBlockAddress = 0;
    uint32_t storeBlockAddress = 0;

    tempBlockAddress = fileItems[fileNo].firstBlock;
    for( int blockNo=0 ; blockNo<numOfBlocksNecessary ; blockNo++) {
        storeBlockAddress = table->address[tempBlockAddress];
        table->address[tempBlockAddress] = 0 ;
        tempBlockAddress = storeBlockAddress;
    }



    strcpy(fileItems[fileNo].fileName , "NULL");
    fileItems[fileNo].fileSize = 0 ;
    fileItems[fileNo].firstBlock = 0;
    printf("File <%s> is sucessfully deleted! \n" , filename);

    demount();
}

void list(){
    mount();
    int isSameArray=-1;
    for( int i=0 ; i < NUM_OF_ITEMS ; i++){
        isSameArray = strcmp("NULL" , fileItems[i].fileName );
        if (isSameArray != 0){
            printf(" %s     |    %d bytes \n",fileItems[i].fileName, fileItems[i].fileSize  );
        }
    }
    demount();
};

/* an helper function to sort the file list easier*/
int compareItemSizeAscending (const void* a , const void* b){
    const fileItem* item1 = (const fileItem*)a;
    const fileItem* item2 = (const fileItem*)b;

    return (int)(item1->fileSize) - (int)(item2->fileSize);
}
int compareItemSizeDescending (const void* a , const void* b){
    const fileItem* item1 = (const fileItem*)a;
    const fileItem* item2 = (const fileItem*)b;

    return (int)(item2->fileSize) - (int)(item1->fileSize);
}

/* 0 for ascending , 1 for descending */
void sort(int direction){
    mount();

    fileItem* realItems;
    int itemCount =0;
    int isSameArray=-1;

    /* find how many actual objects there are */
    for( int i=0 ; i < NUM_OF_ITEMS ; i++){
        isSameArray = strcmp("NULL" , fileItems[i].fileName );
        if (isSameArray != 0){
            itemCount++;
        }
    }

    /* create an array of list of actual existing files */
    realItems = malloc(itemCount*sizeof(fileItem));
    itemCount =0;
    for( int i=0 ; i < NUM_OF_ITEMS ; i++){
        isSameArray = strcmp("NULL" , fileItems[i].fileName );
        if (isSameArray != 0){
            strcpy(realItems[itemCount].fileName , fileItems[i].fileName);
            realItems[itemCount].fileSize = fileItems[i].fileSize;
            realItems[itemCount].firstBlock = fileItems[i].firstBlock;
            itemCount++;
        }
    }
    /* now sort the files in ascending order of size */
    if (direction)
        qsort( realItems , itemCount , sizeof(fileItem) , compareItemSizeDescending );
    else
        qsort( realItems , itemCount , sizeof(fileItem) , compareItemSizeAscending );

    /* and finally write back the sorted files to the disk */
    for ( int i=0 ; i < itemCount ; i++){
        //printf( "File Name: %s , File Size: %d \n",realItems[i].fileName , realItems[i].fileSize );
        strcpy(fileItems[i].fileName ,realItems[i].fileName );
        fileItems[i].fileSize = realItems[i].fileSize ;
        fileItems[i].firstBlock = realItems[i].firstBlock ;
    }
    for ( int i=itemCount ; i < NUM_OF_ITEMS ; i++){
        strcpy(fileItems[i].fileName ,"NULL" );
        fileItems[i].fileSize = 0 ;
        fileItems[i].firstBlock = 0 ;
    }
    demount();
}

void sortA(){
    sort(0);
    printf("Item List is sorted in ascending order as requested. \n");
}

void sortD(){
    sort(1);
    printf("Item List is sorted in descending order as requested. \n");
}

void defragment(){
    mount();

    /* create an clear disk on memory to be edited */
    block* blocksNew;
    fileItem* fileItemsNew;
    FAT* tableNew;
    tableNew = malloc( sizeof(FAT) );
    memset( tableNew, 0, sizeof(FAT) );
    memset( tableNew, 0xFFFFFFFF, sizeof(uint32_t) );
    fileItemsNew = malloc( sizeof(fileItem) * NUM_OF_ITEMS );
    for(int i=0 ; i < NUM_OF_ITEMS ; i++ ){
        strcpy( fileItemsNew[i].fileName , "NULL");
        fileItemsNew[i].fileSize = 0;
        fileItemsNew[i].firstBlock = 0;
    }
    blocksNew = malloc( sizeof(block) * NUM_OF_BLOCKS );
    memset( blocksNew, 0, sizeof(sizeof(block) * NUM_OF_BLOCKS) );


    int itemCount = 0;
    int isSameArray=-1;
    uint32_t tempBlockAddress=0;
    uint32_t tempBlockAddressNew=0;
    for( int i=0 ; i < NUM_OF_ITEMS ; i++){
        isSameArray = strcmp("NULL" , fileItems[i].fileName );
        if (isSameArray != 0){ /* an item on the disk has been found */
            /* lets start with item list */
            strcpy(fileItemsNew[itemCount].fileName , fileItems[i].fileName);
            fileItemsNew[itemCount].fileSize =  fileItems[i].fileSize;
            fileItemsNew[itemCount].firstBlock = emptyAddressNew(tableNew);

            /* continue with data blocks */
            tempBlockAddress = fileItems[i].firstBlock;
            tempBlockAddressNew = fileItemsNew[itemCount].firstBlock;
            while(1){
                blocksNew[tempBlockAddressNew] = blocks[tempBlockAddress]; /* data transfer one block at a time */

                if (table->address[tempBlockAddress] == 0xFFFFFFFF){ /* if it is the last block, end the data transfer*/
                    tableNew->address[tempBlockAddressNew] = 0xFFFFFFFF;
                    break;
                }
                /* update the temp variables and the FAT table*/
                tableNew->address[tempBlockAddressNew] = 0xAFEAFEAF; /* dummy variable as a placeholder */
                tableNew->address[tempBlockAddressNew] = emptyAddressNew(tableNew);
                tempBlockAddressNew = tableNew->address[tempBlockAddressNew];
                tempBlockAddress = table->address[tempBlockAddress];
            }
            itemCount ++;
        }
    }
    /* get rid of the fragmented version*/
    free(table);
    free(blocks);
    free(fileItems);

    /* update disk with the new defragmented version */
    table = tableNew;
    blocks = blocksNew;
    fileItems = fileItemsNew;

    demount();

    printf("Disk is defragmented successfully. \n");
}


