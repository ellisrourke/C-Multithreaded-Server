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
    int* answer;
    int threadID;
};


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

    printf("Thread ID: %d  Number: %d\n",arg_struct->threadID,arg_struct->number);
    
    for(int f=1;f<arg_struct->number;f++){
        if(arg_struct->number % f == 0){
            printf("++ A factor of %d = %d\n",arg_struct->number,f);
        }
    }
    
    
    
    pthread_exit(0);
}




int main(int argc, char *argv[]){
    key_t ShmKEY;
    int ShmID;
    struct Memory *ShmPTR;
    int binaryShifts[32];
    int numberOfThreads = 32;
    struct factorRunner args[numberOfThreads];
    
    ShmKEY = ftok(".", 'x');
    ShmID = shmget(ShmKEY, sizeof(struct Memory), 0666);
    if (ShmID < 0){
        printf("[x] shmget error on server side...\n");
        exit(1);
    }
    printf("[+] Shared Memory Recieved Sucesfully...\n");

    ShmPTR = (struct Memory *) shmat(ShmID, NULL, 0);
    if ((int) ShmPTR == -1){
        printf("[x] shmat error on server side...\n");
        exit(1);
    }
    printf("[+] Shared Memory Sucesfully Attached...\n");
    
    printf("Number from client: %d\n",ShmPTR->number);
    int* num;
    num = convertToBinary(ShmPTR->number);
    
    for(int i=0; i<numberOfThreads;i++){
        int current = binaryToDecimal(num);
        binaryShifts[i] = current;
        num = arrayShift(num);
    }
    
    pthread_t tids[numberOfThreads];
    
    for(int i=0;i<numberOfThreads;i++){
        args[i].number = binaryShifts[i];
        args[i].threadID = i;
        pthread_create(&tids[i],NULL,factorise,&args[i]);
    }

    for(int i=0;i<numberOfThreads;i++){
        pthread_join(tids[i],NULL);
    }

    
    //EXIT
    ShmPTR->status = -2;

    exit(0);
}

