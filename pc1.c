#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define CAPACITY 8

int buffer1[CAPACITY], buffer2[CAPACITY];
int in1, in2;
int out1, out2;
int count1, count2;

pthread_mutex_t mutex1, mutex2;
pthread_cond_t cond1_empty, cond1_fill;
pthread_cond_t cond2_empty, cond2_fill;

int get1()
{
	int item = buffer1[out1];
	out1 = (out1 + 1) % CAPACITY;
	count1--;

	return item;
}

int get2()
{
	int item = buffer2[out2];
	out2 = (out2 + 1) % CAPACITY;
	count2--;

	return item;
}

void put1(int item)
{
	buffer1[in1] = item;
	in1 = (in1 + 1) % CAPACITY;
	count1++;
}

void put2(int item)
{
	buffer2[in2] = item;
	in2 = (in2 + 1) % CAPACITY;
	count2++;
}

void *produce(void *arg)
{
	for (int i = 0; i < CAPACITY; i++){
		pthread_mutex_lock(&mutex1);
		while (count1 == CAPACITY)
			pthread_cond_wait(&cond1_empty, &mutex1);

		int item = 'a' + i;
		put1(item);
		printf("procuce: %c\n", item);

		pthread_cond_signal(&cond1_fill);
		pthread_mutex_unlock(&mutex1);
	}
}

void *work(void *arg)
{
	for (int i = 0; i < CAPACITY; i++){
		pthread_mutex_lock(&mutex1);
		while (count1 == 0)
			pthread_cond_wait(&cond1_fill, &mutex1);

		int item = get1();
		printf("	work1: %c\n", item);
		item -= 32;

		pthread_cond_signal(&cond1_empty);
		pthread_mutex_unlock(&mutex1);

		pthread_mutex_lock(&mutex2);
		while (count2 == CAPACITY)
			pthread_cond_wait(&cond2_empty, &mutex2);

		put2(item);
		printf("	work1: %c\n", item);

		pthread_cond_signal(&cond2_fill);
		pthread_mutex_unlock(&mutex2);
	}
}

void *consume(void *arg)
{
	for (int i = 0; i < CAPACITY; i++){
		pthread_mutex_lock(&mutex2);
		while (count2 == 0)
			pthread_cond_wait(&cond2_fill, &mutex2);

		int item = get2();
		printf("		consume: %c\n", item);

		pthread_cond_signal(&cond2_empty);
		pthread_mutex_unlock(&mutex2);
	}
	return NULL;
}

int main()
{
	pthread_t consumer_tid, worker_tid;
	pthread_mutex_init(&mutex1, NULL);
	pthread_mutex_init(&mutex2, NULL);
	pthread_cond_init(&cond1_empty, NULL);
	pthread_cond_init(&cond1_fill, NULL);
	pthread_cond_init(&cond2_empty, NULL);
	pthread_cond_init(&cond2_fill, NULL);

	pthread_create(&consumer_tid, NULL, consume, NULL);
	pthread_create(&worker_tid, NULL, work, NULL);
	produce(NULL);

	pthread_join(consumer_tid, NULL);
	pthread_join(worker_tid, NULL);
	return 0;
}
