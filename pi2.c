#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#define NR_TOTAL 2000
#define NR_CPU 4
#define NR_CHILD (NR_TOTAL / NR_CPU)

struct param
{
	int start;
	int end;
};

void *compute(void *arg)
{
	struct param *p = (struct param *)arg;
	float sum = 0;
	for (int i = p->start + 1; i <= p->end; i++){
		int denominator = i * 2 - 1;
		int sign = i % 2 ? 1 : -1;
		sum += 1 / (float)denominator * sign;
	}

	float *res = malloc(sizeof(float));
	*res = sum;
	return (void *)res;
}

int main()
{
	pthread_t workers[NR_CPU];
	struct param params[NR_CPU];

	for (int i = 0; i < NR_CPU; i++){
		struct param *p = &params[i];
		p->start = i * NR_CHILD;
		p->end   = (i + 1) * NR_CHILD;
		pthread_create(&workers[i], NULL, compute, (void *)p);
	}

	float sum = 0;
	for (int i = 0; i < NR_CPU; i++){
		float *result;
		pthread_join(workers[i], (void **)&result);
		sum += *result;
		free(result);
	}

	sum *= 4;
	printf("sum = %f\n", sum);
	return 0;
}
