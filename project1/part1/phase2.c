#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 4
#define NUM_ACCOUNTS 1
#define TRANSACTIONS_PER_TELLER 1
#define INITIAL_BALANCE 1000.00

typedef struct {
	int account_id;
	double balance;
	int transaction_count;
	pthread_mutex_t lock;
} Account;

Account accounts[NUM_ACCOUNTS];

void deposit(int account_id, double amount) {
        if(pthread_mutex_lock(&accounts[account_id].lock) != 0){
                perror("Failed to acquire lock");
                return;
        }

        accounts[account_id].balance += amount;
        accounts[account_id].transaction_count++;
        pthread_mutex_unlock(&accounts[account_id].lock);
}

void* teller_thread(void* arg){
	int teller_id = *(int*)arg;
	unsigned int seed = time(NULL) ^ (teller_id * 0x9e3779b1);

	for (int i = 0; i < TRANSACTIONS_PER_TELLER; i++){
		int account_choice = rand_r(&seed) % NUM_ACCOUNTS;

		int dorw = rand_r(&seed) % 2; //dorw = deposit(1) or withdrawl(0)
		double amount = (rand_r(&seed) % 100) + 1;
		double old_balance = accounts[account_choice].balance;

	        if (dorw){
        	    	deposit(account_choice, amount);
			printf("Thread %d: Depositing %.2f\n", teller_id + 1, amount);
	        }
		else{
	            	deposit(account_choice, -amount);
                        printf("Thread %d: Withdrawing %.2f\n", teller_id + 1, amount);
		}

		usleep(rand_r(&seed) % 1000);
	}

	return NULL;
}

int main() {
	pthread_t threads[NUM_THREADS];
	int thread_ids[NUM_THREADS];

	for ( int i = 0; i < NUM_ACCOUNTS ; i ++){
		pthread_mutex_init(&accounts[i].lock, NULL);
		accounts[i].balance = INITIAL_BALANCE;
		accounts[i].transaction_count = 0;
	}

	printf("Initial Balance: %.2f\n", accounts[0].balance);

	for (int i =0; i < NUM_THREADS; i++){
		thread_ids[i] = i;
		pthread_create(&threads[i], NULL, teller_thread, &thread_ids[i]);
	}

	for (int i = 0; i < NUM_THREADS; i++) {
	        pthread_join(threads[i], NULL);
	}

    printf("Final balance: %.2f\n", accounts[0].balance);
    return 0;
}
