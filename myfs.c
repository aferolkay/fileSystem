#include "fileSystem.h"
#include <stdio.h>
#include <string.h>

/*
*
* FOR COMPILATION : gcc -o myfs myfs.c fileSystem.c  -lm 
*
*/

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Insufficient arguments.\n");
        return 1;
    }

    char* command = argv[2];
    // printf("1:%s, 2:%s, 3:%s",argv[0],argv[1],argv[2]); --> 1:./myfs, 2:disk, 3:-readRead
    if (strcmp(command, "-format") == 0) {
        format();
    } else if (strcmp(command, "-read") == 0) {
        if (argc < 5) {
            printf("Insufficient arguments for read command.\n");
            return 1;
        }
        char* srcFileName = argv[3];
        char* destFileName = argv[4];
        read(srcFileName, destFileName);
    } else if (strcmp(command, "-write") == 0) {
        if (argc < 5) {
            printf("Insufficient arguments for write command.\n");
            return 1;
        }
        char* srcFileName = argv[3];
        char* destFileName = argv[4];
        write(srcFileName,destFileName);
    } else if (strcmp(command, "-delete") == 0) {
        if (argc < 4) {
            printf("Insufficient arguments for delete command.\n");
            return 1;
        }
        char* fileName = argv[3];
        delete(fileName);
    }
    else if (strcmp(command, "-list") == 0) {
        list();
    } else if (strcmp(command, "-sorta") == 0) {
        sortA();
    } else if (strcmp(command, "-sortd") == 0) {
        sortD();
    } else if (strcmp(command, "-printfilelist") == 0) {
        printFileList();
    } else if (strcmp(command, "-printfat") == 0) {
        printFAT();
    } else if (strcmp(command, "-defragment") == 0) {
        defragment();
    } else if (strcmp(command, "-create") == 0) {
        create();
    } else {
        printf("Invalid command.\n");
        return 1;
    }

    return 0;
}
