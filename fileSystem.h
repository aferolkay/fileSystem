#include <stdio.h>
#include <stdint.h>
#include <math.h>
#define NUM_OF_ITEMS 128
#define NUM_OF_ADDRESSES 4096
#define NUM_OF_BLOCKS 4096
#define FAT_START 0  
#define FILE_LIST_START 16384 
#define DATA_START 49152
#define FILE_END 2146304
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
    int32_t address[NUM_OF_ADDRESSES];
};
typedef struct FAT FAT;

struct DISK{
    int num_fileItem;
    int num_dataBlocks;
};
typedef struct DISK DISK;


extern DISK disk;
extern block* blocks;
extern fileItem* fileItems;
extern FAT* table;

void create();

void format();

void write(char *srcPath, char *destFileName);

void read(char *srcFileName, char *destPath);

//void delete(char* filename);

void list();

void sortA();

void sortD();

void printFileList();

void printFAT();

void defragment();

void mount();

void demount();