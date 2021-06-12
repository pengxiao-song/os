#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define N 500

float worker_output;

void *compute()
{
	for (int i = 1; i < N / 2; i++){
		int denominator = i * 2 - 1;
		int sign = i % 2 ? 1 : -1;
		worker_output += 1 / (float)denominator * sign;
	}
	return NULL;
}

float master_output;

void *master()
{
	for (int i = N / 2; i < N;i+ +){
		int denominator = i * 2 - 1;
		int sign = i % 2 ? 1 : -1;
		master_output += 1 / (float)denominator * sign;
	}
	return NULL;
}

int main()
{
	pthread_t worker_tid;
	pthread_create(&worker_tid, NULL, compute, NULL);
	master();
	pthread_join(worker_tid, NULL);
	float total = (worker_output + master_output) * 4;
	printf("master_output = %f, worker_output = %f, total = %f\n", master_output, worker_output, total);
	return 0;
}