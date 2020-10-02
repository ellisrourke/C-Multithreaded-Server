/* CLIENT.C */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <time.h>
#include "header.h"



int main(int argc, char *argv[]){
    key_t ShmKEY;
    int ShmID;
    struct Memory *ShmPTR;
    
    if(argc < 2){
        printf("Please provide an integer...\n");
        exit(1);
    }
    
    //int number = atoi(argv[1]);
    //int num2 = atoi(argv[2]);
    
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
    
//    ShmPTR->status = NOT_READY;

    ShmPTR->number = ShmPTR->request[0];
    ShmPTR->status = 0;
    //ShmPTR->resultStatus = 0;
    

    ShmPTR->numRequests = argc-1;
    for(int i=0;i<argc-1;i++){
        ShmPTR->request[i] = atoi(argv[i+1]);
    }

    printf("[?] Please start the server...\n");
    clock_t begin = clock();

    while(1){
        if(ShmPTR->number == -1){
            printf("[+] Server completion detected...\n");
            shmdt((void *) ShmPTR);
            printf("[+] Shared Memory Sucesfully Detached...\n");
            shmctl(ShmID, IPC_RMID, NULL);
            printf("[+] Shared Memory sucesfully Removed...\n");
            
            clock_t end = clock();
            double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
            printf("[+] Execution time: %f\n",time_spent);
            printf("[+] Client Exit...\n");
            exit(0);
        }
        else if(ShmPTR->status == 1){
            printf("%d has a factor: %d\n",ShmPTR->current,ShmPTR->number);
            ShmPTR->status = 0;
        }

        else if(ShmPTR->status == -3){
            printf("-----------------------------------------------------\n");
            ShmPTR->status = 0;
        }
    }




     
}
