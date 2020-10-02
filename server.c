/* SERVER.C */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <pthread.h>
#include "header.h"

struct factorRunner {
    int number;
    //int* answer;
    int threadID;
};

struct Memory* sharedData;

int* convertToBinary(int number){
    int n, c, k;
    static int binary[32];

    for (c = 31; c >= 0; c--){
        k = number >> c;
          if (k & 1){
          binary[31-c] = 1;
          } else {
          binary[31-c] = 0;
          }
      }
    return binary;
}

int binaryToDecimal(int number[]){
    int value = 0;
    int multiplier = 1;
    
    for(int i =31; i>=0;i--){
        value += number[i]*multiplier;
        multiplier *= 2;
    }
    
    return value;
}

int* arrayShift(int array[]){
    /* shifting array elements */
        int temp = array[0];
        int i;
        for(i=0;i<31;i++)
        {
            array[i] = array[i+1];
        }
        array[31]=temp;
    return array;
}


void* factorise(void* arg){
    struct factorRunner *arg_struct = (struct factorRunner*) arg;
    int* binary = convertToBinary(arg_struct->number);

    if(arg_struct->number < 1){
        printf("Negative Thread..\n");
        pthread_exit(0);
    }
    
    printf("Thread ID: %d  Number: %d\n",arg_struct->threadID,arg_struct->number);
    for(int f=2;f<arg_struct->number;f++){
        if(arg_struct->number % f == 0){
            //printf("[>] A factor of %d = %d\n",arg_struct->number,f);
            while(1){
                if(sharedData->status == 1){
                    usleep( 500000 );
                } else {
                    sharedData->status = 1;
                    sharedData->number = f;
                    sharedData->current = arg_struct->number;
                    break;
                }
            }
        }
    }
    sleep(5);
    pthread_exit(0);
}




int main(int argc, char *argv[]){
    key_t ShmKEY;
    int ShmID;
    struct Memory *ShmPTR;
    int binaryShifts[32];
    
    ShmKEY = ftok(".", 'x');
    ShmID = shmget(ShmKEY, sizeof(struct Memory), IPC_CREAT | 0666);
    if(ShmID < 0){
        printf("[x] shmget error on client side...\n");
        exit(1);
    }
    printf("[+] Shared Memory Recieved Sucesfully...\n");

    ShmPTR = (struct Memory *) shmat(ShmID, NULL, 0);
    if ((int) ShmPTR == -1){
        printf("[x] shmat error on server side...\n");
        exit(1);
    }
    printf("[+] Shared Memory Sucesfully Attached...\n");
    
    printf("Requests from client: [ ");
    for(int i=0;i<ShmPTR->numRequests;i++){
        printf("%d ",ShmPTR->request[i]);
    } printf("]\nFactoring will start in 2 seconds...\n");
    sleep(2);
    
    
    
    sharedData = ShmPTR;

    
    
    
    int numberOfThreads = 32 * ShmPTR->numRequests;
    struct factorRunner args[numberOfThreads];
    
    //printf("Number from client: %d\n",ShmPTR->number);
    for(int c=0;c<ShmPTR->numRequests;c++){
        printf("----------- Request Number: %d -----------\n",c);
        int* num;
        //num = convertToBinary(ShmPTR->number);
        num = convertToBinary(ShmPTR->request[c]);
        for(int i=0; i<32;i++){
            int current = binaryToDecimal(num);
            binaryShifts[i] = current;
            num = arrayShift(num);
        }
        
        pthread_t tids[32];
        int threadDepth;

        for(int j=0;j<31;j++){
            args[j].number = binaryShifts[j];
            args[j].threadID = j;
            //args[i+j].ll = ((i+j)*threadDepth)+1;
            //args[i+j].ul = (i+j+1)*threadDepth;
            //args[i+j].ShmPTR = ShmPTR;
            pthread_create(&tids[j],NULL,factorise,&args[j]);
        }

        for(int i=0;i<numberOfThreads;i++){
            pthread_join(tids[i],NULL);
            //ShmPTR->result = t
        }
        ShmPTR->status = -3;
    }
    
    //EXIT
    printf("\n[+] Server Exiting...");
    ShmPTR->status = -2;
    ShmPTR->number = -1;


    exit(0);
}

