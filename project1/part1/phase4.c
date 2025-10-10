#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <errno.h>

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

void safe_transfer(int from_id, int to_id, double amount){
	int first = (from_id < to_id) ? from_id : to_id;
	int second = (from_id < to_id) ? to_id : from_id;

	pthread_mutex_lock(&accounts[first].lock);
	printf("Thread %ld: Locked account %d\n", pthread_self(), first);

	usleep(100000);

	pthread_mutex_lock(&accounts[second].lock);
        printf("Thread %ld: Locked account %d\n", pthread_self(), second);

	accounts[from_id].balance -= amount;
	accounts[to_id].balance += amount;

	pthread_mutex_unlock(&accounts[second].lock);
	pthread_mutex_unlock(&accounts[first].lock);

	printf("Thread %ld: Transfer complete\n", pthread_self());
}

void* transfer_thread(void* arg){
	int id = *(int*)arg;

	if (id == 0){
		safe_transfer(0, 1, 100.0);
	}
	else{
		safe_transfer(1, 0, 50.0);
	}

	return NULL;
}

int main() {
	pthread_t threads[NUM_THREADS];
	int thread_ids[NUM_THREADS];

	for ( int i = 0; i < NUM_ACCOUNTS ; i ++){
		pthread_mutex_init(&accounts[i].lock, NULL);
		accounts[i].balance = INITIAL_BALANCE;
		accounts[i].account_id = i;
	}

	printf("Initial Balance:\n A = %.2f\n B = %.2f\n", accounts[0].balance, 
accounts[1].balance);

	for (int i = 0; i < NUM_THREADS; i++){
		thread_ids[i] = i;
		pthread_create(&threads[i], NULL, transfer_thread, &thread_ids[i]);
	}

	for (int i = 0; i < NUM_THREADS; i++){
		pthread_join(threads[i], NULL);
	}

	printf("Final balance:\n A = %.2f\n B = %.2f\n", accounts[0].balance, 
accounts[1].balance);

	for (int i = 0; i < NUM_ACCOUNTS; i++){
		pthread_mutex_destroy(&accounts[i].lock);
	}

	return 0;
}
