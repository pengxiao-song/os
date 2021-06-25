#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define CAPACITY 8

int buffer1[CAPACITY], buffer2[CAPACITY];
int in1, in2;
int out1, out2;
int count1, count2;

typedef struct 
{
	int value;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
} sema_t;

void sema_init(sema_t *sema, int value)
{
	sema->value = value;
	pthread_mutex_init(&sema->mutex, NULL);
	pthread_cond_init(&sema->cond, NULL);
}

void sema_destroy(sema_t *sema)
{
	pthread_mutex_destroy(&sema->mutex);
	pthread_cond_destroy(&sema->cond);
}

void sema_wait(sema_t *sema)
{
	pthread_mutex_lock(&sema->mutex);
	while (sema->value <= 0)
		pthread_cond_wait(&sema->cond, &sema->mutex);
	sema->value--;
	pthread_mutex_unlock(&sema->mutex);
}

void sema_signal(sema_t *sema)
{
	pthread_mutex_lock(&sema->mutex);
	sema->value++;
	pthread_cond_signal(&sema->cond);
	pthread_mutex_unlock(&sema->mutex);
}

sema_t mutex1, mutex2;
sema_t cond1_empty, cond1_fill;
sema_t cond2_empty, cond2_fill;

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
		sema_wait(&cond1_empty);
		sema_wait(&mutex1);

		int item = 'a' + i;
		put1(item);
		printf("procuce: %c\n", item);

		sema_signal(&mutex1);
		sema_signal(&cond1_fill);
	}
}

void *work(void *arg)
{
	for (int i = 0; i < CAPACITY; i++){
		sema_wait(&cond1_fill);
		sema_wait(&mutex1);

		int item = get1();
		printf("	work1: %c\n", item);
		item -= 32;

		sema_signal(&mutex1);
		sema_signal(&cond1_empty);

		sema_wait(&cond2_empty);
		sema_wait(&mutex2);

		put2(item);
		printf("	work1: %c\n", item);

		sema_signal(&mutex2);
		sema_signal(&cond2_fill);
	}
}

void *consume(void *arg)
{
	for (int i = 0; i < CAPACITY; i++){
		sema_wait(&cond2_fill);
		sema_wait(&mutex2);

		int item = get2();
		printf("		consume: %c\n", item);

		sema_signal(&mutex2);
		sema_signal(&cond2_empty);
	}
	return NULL;
}

int main()
{
	pthread_t consumer_tid, worker_tid;
	
	sema_init(&mutex1, 1);
	sema_init(&mutex2, 1);

	sema_init(&cond1_empty, CAPACITY);
	sema_init(&cond1_fill, 0);
	sema_init(&cond2_empty, CAPACITY);
	sema_init(&cond2_fill, 0);

	pthread_create(&consumer_tid, NULL, consume, NULL);
	pthread_create(&worker_tid, NULL, work, NULL);
	produce(NULL);

	pthread_join(consumer_tid, NULL);
	pthread_join(worker_tid, NULL);
	return 0;
}
