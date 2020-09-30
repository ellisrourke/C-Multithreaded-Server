/* CLIENT.C */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include "header.h"



int main(int argc, char *argv[]){
    key_t ShmKEY;
    int ShmID;
    struct Memory *ShmPTR;
    
    if(argc != 2){
        printf("Please provide an integer...\n");
        exit(1);
    }
    
    int number = atoi(argv[1]);
    
    printf("...\n");
    
    ShmKEY = ftok(".",'x');
    ShmID = shmget(ShmKEY, sizeof(struct Memory), IPC_CREAT | 0666);
    if(ShmID < 0){
        printf("[x] shmget error on client side...\n");
        exit(1);
    }
    printf("[+] Shared Memory Recieved Sucesfully...\n");
    
    ShmPTR = (struct Memory *) shmat(ShmID, NULL, 0);
    if((int) ShmPTR == -1){
        printf("[x] shmat error on client side...\n");
        exit(1);
    }
    printf("[+] Shared Memory Sucesfully Attached...\n");
    
    ShmPTR->status = NOT_READY;

    ShmPTR->number = number;
    ShmPTR->status = FILLED;
    
    printf("[?] Please start the server...\n");
    
    while(1){
        if(ShmPTR->status == -2){
            break;
        }
        sleep(1);

    }
    
    printf("[+] Server completion detected...\n");
    shmdt((void *) ShmPTR);
    printf("[+] Shared Memory Sucesfully Detached...\n");
    shmctl(ShmID, IPC_RMID, NULL);
    printf("[+] Shared Memory sucesfully Removed...\n");
    printf("[+] Client Exit...\n");
    exit(0);
}
