#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 3
/* the available amount of each resource */
int available[NUMBER_OF_RESOURCES];
/*the maximum demand of each customer */
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
/* the amount currently allocated to each customer */
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
/* the remaining need of each customer */
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
//threads:
pthread_t tid[NUMBER_OF_CUSTOMERS];
pthread_mutex_t lock;

bool isSafe(){
    int work[NUMBER_OF_RESOURCES];
    for(int i=0; i<NUMBER_OF_RESOURCES; i++){
        work[i] = available[i];
    }

    bool finish[NUMBER_OF_CUSTOMERS];
    for(int i=0; i<NUMBER_OF_CUSTOMERS; i++){
        finish[i] = false;
    }

    int customerCtr = 1;
    for(int k=0 ; k<NUMBER_OF_RESOURCES; k++){
        work[k] += allocation[customerCtr][k];
    }
    int ctr = 0;
    bool flagCan;
    bool flag;
    while(ctr<NUMBER_OF_RESOURCES){

        if(finish[customerCtr] == false){
            for(int j=0 ; j<NUMBER_OF_RESOURCES;j++){
                if(need[customerCtr][j]>work[j])
                    flagCan = false;
            }
            if(flagCan){
                for(int k=0 ; k<NUMBER_OF_RESOURCES; k++){
                    work[k] += allocation[customerCtr][k];
                }
                finish[customerCtr] = true;
                ctr++;
                customerCtr = (customerCtr + 1) % NUMBER_OF_CUSTOMERS;
                break;
            }
        }

        for(int i=0 ; i<NUMBER_OF_CUSTOMERS; i++){
            if(finish[i] == false){
                for(int j=0 ; j<NUMBER_OF_RESOURCES; j++){
                    if(work[j]>need[customerCtr][j])
                        flag = false;
                break;
                }
            }
        }

        if(flag)
            return false;
    }

    return true;
}

bool request_resources(int request[], int customer_num) {

    printf("Customer %d is Requesting Resources:\n", customer_num);
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        printf("Request for R%d = %d\n", i, request[i]);
    }


    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        printf("Available R%d = %d\n", i, available[i]);
    }

    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        printf("Need for R%d = %d\n", i, need[customer_num][i]);
    }

    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        //first check 2 constraints:
        if (request[i] <= need[customer_num][i]) {
            if(request[i] <= available[i]){
                if( isSafe() ){
                    printf("Safe! Request is granted!\n");

                    for(int j=0 ; j<NUMBER_OF_RESOURCES ; j++){
                        available[j] -= request[j];
                        allocation[customer_num][j] += request[j];
                        need[customer_num][j] -= request[j];
                    }

                    return true;
                }else{
                    printf("Not safe! Can't grant request!\n");
                    return false;
                }
            }else{
                printf("Request is more than available! ABORT!\n");
                return false;
            }
        }else{
            printf("Request is more than need! ABORT!\n");
            return false;
        }


    }
}

bool request_resources_control(int request[],int customer_num){
    //CRITICAL SECTION //
    bool released = false;
    pthread_mutex_lock(&lock);
    released=request_resources(request,customer_num);
    pthread_mutex_unlock(&lock);

    return released;
}

bool release_resources(int release[], int customer_num){
    //give back resources:
    for(int i=0 ; i<NUMBER_OF_RESOURCES ; i++){
        available[i] += release[i];
    }
    return true;gi
}
void release_resources_control(int customer_num){
    pthread_mutex_lock(&lock);

    for(int i=0;i<NUMBER_OF_RESOURCES;i++)
        release_resources(maximum[customer_num],customer_num);
    pthread_mutex_unlock(&lock);
    printf("Thread %d finished execution \n",customer_num);
}

void* getResources(void *arg){
    int customerNum = *(int *)arg;
    bool released = false;

    //a random request
    int request_one[] = {6,7,8};     //prototype for a request
    while(request_resources_control(request_one,customerNum)==false);
    release_resources_control(customerNum);
    return 0;
}



int main(int argc, char *argv[]) {
    //check input:
    if (argc < NUMBER_OF_RESOURCES + 1) {
        printf("not enough arguments!\n");
        exit(1);
    }

    //initialization of our data structures:
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        available[i] = strtol(argv[i + 1], NULL, 10);
        for (int j = 0; j < NUMBER_OF_CUSTOMERS; j++) {
            maximum[j][i] = 5; //random initialization
            allocation[j][i] = 0;//default
            need[j][i] = 5;//t first, need is same as max
        }
    }

    //create the threads:
    int pid[] = {0,1,2,3,4};
    for (int i=0; i<NUMBER_OF_CUSTOMERS ; i++){
        pthread_create(&(tid[i]),NULL,getResources,&pid[i])
    }
    return 0;
}

