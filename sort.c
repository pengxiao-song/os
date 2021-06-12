#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

int a[] = {1, 9, 2, 8, 7, 3, 6, 4, 0, 5};

#define N_ARR (sizeof(a) / sizeof(a[0]))
#define N_TID 2
#define N_CHILD (N_ARR / N_TID)

struct param
{
	int start;
	int end;
};

void *selsort(void * arg)
{
	struct param *p = (struct param *)arg;
	int st = p->start, ed = p->end;
	for (int i = st; i < ed; i++){
		int mininx = i;
		for (int j = i + 1; j < ed; j++){
			if (a[j] < a[mininx])
				mininx = j;
		}
		int temp = a[i];
		a[i] = a[mininx];
		a[mininx] = temp;
	}
	return NULL;
}

int tmp[N_ARR];

void mergesort(int q[], int l, int r)
{
	if (l >= r)	return ;

	int mid = l + r >> 1;

	mergesort(q, l, mid), mergesort(q, mid + 1, r);

	int k = 0, i = l, j = mid + 1;
	while (i <= mid && j <= r)
		if (q[i] <= q[j])	tmp[k++] = q[i++];
		else tmp[k++] = q[j++];
	while (i <= mid)	tmp[k++] = q[i++];
	while (j <= r)		tmp[k++] = q[j++];

	for (i = l, j = 0; i <= r; i++, j++)	q[i] = tmp[j];
}

int main()
{
	pthread_t tid[N_TID];
	struct param params[N_TID];

	for (int i = 0; i < N_TID; i++){
		struct param *p = params + i;
		p->start = i * N_CHILD;
		p->end	 = (i + 1) * N_CHILD;
		pthread_create(tid + i, NULL, selsort, p);
	}

	for (int i = 0; i < N_TID; i++){
		pthread_join(tid[i], NULL);
	}


	mergesort(a, 0, N_ARR - 1);

	for (int i = 0; i < N_ARR; i++)	printf("%d ", a[i]);
	
	return 0;
}