#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 4
#define NUM_ACCOUNTS 1
#define TRANSACTIONS_PER_TELLER 1

typedef struct {
	int account_id;
	double balance;
	int transaction_count;
} Account;

Account accounts[NUM_ACCOUNTS];

void* teller_thread(void* arg){
	int teller_id = *(int*)arg;
	unsigned int seed = time(NULL) ^ (teller_id * 0x9e3779b1);

	for (int i = 0; i < TRANSACTIONS_PER_TELLER; i++){
		int account_choice = rand_r(&seed) % NUM_ACCOUNTS;

		int dorw = rand_r(&seed) % 2; //dorw = deposit(1) or withdrawl(0)
		double amount = (rand_r(&seed) % 100) + 1;
		double old_balance = accounts[account_choice].balance;

	        if (dorw){
        	    	accounts[account_choice].balance = old_balance + amount;
			printf("Thread %d:Depositing %.2f\n", teller_id + 1, amount);
	        }
		else{
	            	accounts[account_choice].balance = old_balance - amount;
                        printf("Thread %d: Withdrawling %.2f\n", teller_id + 1, amount);
		}

	        accounts[account_choice].transaction_count++;
		usleep(rand_r(&seed) % 1000);
	}

	return NULL;
}

int main() {
	pthread_t threads[NUM_THREADS];
	int thread_ids[NUM_THREADS];

	accounts[0].account_id = 1;
	accounts[0].balance = 1000.00;
	accounts[0].transaction_count = 0;

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
