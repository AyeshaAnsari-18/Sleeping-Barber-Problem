#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_CUSTOMERS 10

sem_t waitingRoom;
sem_t barberChair;
sem_t barberPillow;
sem_t seatBelt;
int flag = 0;
int temp = 5;

void *customer (void *num) {
    int c = *(int *)num;
    printf("Customer %d leaving for barber shop.\n", c);
    sleep(5);
    printf("Customer %d reached at barber shop.\n", c);
    sem_wait(&waitingRoom);
    printf("Customer %d entered waiting room.\n", c);

    sem_wait(&barberChair);
    sem_post(&waitingRoom);
    printf("\n\t\tCustomer %d waking the barber.\n", c);
    sem_post(&barberPillow);
    sem_wait(&seatBelt);
    sem_post(&barberChair);
    printf("Customer %d leaving barber shop.\n", c);
    return NULL;
}

void *barber(void *data) {
    while (!flag) {
        printf("\n\t\tBarber is sleeping\n");
        sem_wait(&barberPillow);
        if (!flag) {
            printf("\t\tBarber is cutting hair\n");
            sleep(5);
            printf("\t\tBarber has finished cutting hair.\n");
            temp--;
            sem_post(&seatBelt);
        } else {
            printf("Barber is closing shop and going home.\n");
        }
    }
    return NULL;
}

int main(void) {
    pthread_t barber_id;
    pthread_t customer_id[MAX_CUSTOMERS];
    int numCustomers = 5;
    int numChairs = 3;
    int i;
    int cus[MAX_CUSTOMERS];

    printf("Sleeping Barber Problem Solution using Semaphores and Threads.\n");

    sem_init(&waitingRoom, 0, numChairs);
    sem_init(&barberChair, 0, 1);
    sem_init(&barberPillow, 0, 0);
    sem_init(&seatBelt, 0, 0);

    pthread_create(&barber_id, NULL, barber, NULL);

    for (i = 0; i < numCustomers; i++) {
        cus[i] = i + 1;
        pthread_create(&customer_id[i], NULL, customer, (void *)&cus[i]);
    }

    for (i = 0; i < numCustomers; i++) {
        pthread_join(customer_id[i], NULL);
    }

    if(temp == 0) {
        flag = 1;
        sem_post(&barberPillow);
    }

    pthread_join(barber_id, NULL);

    sem_destroy(&waitingRoom);
    sem_destroy(&barberChair);
    sem_destroy(&barberPillow);
    sem_destroy(&seatBelt);

    return 0;
}
