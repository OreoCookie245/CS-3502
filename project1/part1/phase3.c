#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#define NUM_THREADS 2
#define NUM_ACCOUNTS 2
#define INITIAL_BALANCE 1000.00

typedef struct {
	int account_id;
	double balance;
	pthread_mutex_t lock;
} Account;

Account accounts[NUM_ACCOUNTS];
int progress[NUM_THREADS] = {0};

void transfer(int from_id, int to_id, double amount, int thread_id){
	printf("Thread %ld: Attempting transfer from %d to %d\n", pthread_self(), 
from_id, to_id);
	pthread_mutex_lock(&accounts[from_id].lock);
	printf("Thread %ld: Locked account %d\n", pthread_self(), from_id);

	usleep(100000);

	printf("Thread %ld: Waiting for account %d\n", pthread_self(), from_id);
	pthread_mutex_lock(&accounts[to_id].lock);

	accounts[from_id].balance -= amount;
	accounts[to_id].balance += amount;

	pthread_mutex_unlock(&accounts[to_id].lock);
	pthread_mutex_unlock(&accounts[from_id].lock);

	progress[thread_id] = 1;
}

void* transfer_thread(void* arg){
	int id = *(int*)arg;

	if (id == 0){
		transfer(0, 1, 100.0, id);
	}
	else{
		transfer(1, 0, 50.0, id);
	}

	return NULL;
}

void* progress_thread(void* arg){
	double timeout = 5.0;
	double elapsed = 0.0;

	while(elapsed < timeout){
		bool done = true;
		for (int i = 0; i < NUM_THREADS; i++){
			if (progress[i] == 0) {
				done = false;
			}
		}
		if (done){
                	printf("Transfers successfully completed\n");
			return NULL;
        	}

		usleep(100000);
		elapsed += 0.1;
	}

	printf("Error: Deadlock\n");

	return NULL;
}

int main() {
	pthread_t threads[NUM_THREADS];
	int thread_ids[NUM_THREADS];
	pthread_t progress_t;

	for ( int i = 0; i < NUM_ACCOUNTS ; i ++){
		pthread_mutex_init(&accounts[i].lock, NULL);
		accounts[i].balance = INITIAL_BALANCE;
		accounts[i].account_id = i;
	}

	printf("Initial Balance:\n A = %.2f\n B = %.2f\n", accounts[0].balance, 
accounts[1].balance);

	for (int i =0; i < NUM_THREADS; i++){
		thread_ids[i] = i;
		pthread_create(&threads[i], NULL, transfer_thread, &thread_ids[i]);
	}
	pthread_create(&progress_t, NULL, progress_thread, NULL);

	pthread_join(progress_t, NULL);

    printf("Final balance:\n A = %.2f\n B = %.2f\n", accounts[0].balance, 
accounts[1].balance);
    return 0;
}
