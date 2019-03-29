#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include <ucontext.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/time.h>

#define LEN 		1000
#define MAX_PRINT 	20
#define MAX_PRINT_RES 	1000

#define STACK_SIZE 1024 * 1024

ucontext_t main_context;
ucontext_t * contexts;
double * t, * t_start, * t_end;
int * a_size, * c_finish;
int argc_c = 0, finished = 0, tmp = 0;

int cmp(int a, int b) {return a - b;}
void swap(int * a, int * b) {int buf = * a; *a = *b; *b = buf; return;}

void print_array(int * a, int size_of_array)
{
    size_of_array = (size_of_array > MAX_PRINT) ? MAX_PRINT : size_of_array;
    for(int i = 0; i < size_of_array; i++)
    	printf("%d ", a[i]);
    printf("\n");
}

void merge(int ** a, int * a_size, int total_size, FILE * fp)
{
	int * curr = (int *)malloc(argc_c * sizeof(int));
	for(int i = 0; i < argc_c; i++) curr[i] = 0;
	int min = 0, tmp_m = 0;
	for(int k = 0; k < total_size; k++)
	{	
		int j = 0;
		while(j <= argc_c && curr[j] >= a_size[j]) j++;
			if(j == argc_c) {free(curr); return;}
			else {min = a[j][curr[j]]; tmp_m = j;}
		for(int i = 0; i < argc_c; i++)
		{
			if(curr[i] < a_size[i])
				if(a[i][curr[i]] < min)
					{min = a[i][curr[i]]; tmp_m = i;}
		}
		fprintf(fp, "%d ", min);
		curr[tmp_m]++;
	}
	printf("\n");
	free(curr);
}

void * allocate_stack_mprot()
{
	void * stack = malloc(STACK_SIZE);
	mprotect(stack, STACK_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC);
	return stack;
}
					
#define go_next() \
do{ \
	tmp = id; do{ \
		tmp++; \
	}while(c_finish[(tmp) % argc_c]); \
	tmp %= argc_c; \
}while(0) 

#define switch_coroutine() \
do{ \
	t[id] += (t_end[id] - t_start[id]); \
	go_next(); \
	if(id != tmp) {swapcontext(&contexts[id], &contexts[tmp]);} \
}while(0)			

#define coroutine_finish() \
do{ \
	if(a_size[id] == size_of_array && c_finish[id] == 0) \
	{ \
		finished++; c_finish[id] = 1; \
		if(finished == argc_c) \
			{swapcontext(&contexts[id], &main_context);} \
	} \
	switch_coroutine(); \
}while(0)																											

void sort(int * a, int size_of_array, int id)
{
	t_end[id] = clock(); switch_coroutine(); t_start[id] = clock();
	if(size_of_array < 2) {coroutine_finish(); return;}
	int x = a[size_of_array >> 1], i, j; t_end[id] = clock(); switch_coroutine(); t_start[id] = clock();
	for(i = 0, j = size_of_array - 1;; i++, j--) 
	{
		while(a[i] < x) {i++; t_end[id] = clock(); switch_coroutine(); t_start[id] = clock();} 
		while(a[j] > x) {j--; t_end[id] = clock(); switch_coroutine(); t_start[id] = clock();}
		if(i >= j) break; t_end[id] = clock(); switch_coroutine(); t_start[id] = clock();
		swap(a + i, a + j); t_end[id] = clock(); switch_coroutine(); t_start[id] = clock();
	}
	t_end[id] = clock(); switch_coroutine(); t_start[id] = clock();
	sort(a, i, id);
	t_end[id] = clock(); switch_coroutine(); t_start[id] = clock();
	sort(a + i, size_of_array - i, id);
	coroutine_finish();
}

int main (int argc, char * argv[])
{	
	int ** a = 0; 
	int n = 0, tmp = 0, total_size = 0; 
	argc_c = argc - 1;
	a = (int **)malloc(argc_c * sizeof(int *));
	a_size = (int *)malloc((argc_c) * sizeof(int));
	c_finish = (int *)malloc((argc_c) * sizeof(int));;

	for(int i = 0; i < argc_c; i++) 
	{
		c_finish[i] = 0;
		FILE * fp;
		if(!(fp = fopen(argv[i + 1], "r"))) {printf("Can not open file %s!\n", argv[i + 1]);}
		while(fscanf(fp, "%d", &tmp) == 1) n++; rewind(fp);
		a[i] = (int *)malloc(n * sizeof(int));
		total_size += a_size[i] = n; n = 0;
		for(int j = 0; j < a_size[i]; j++)
		{if(fscanf(fp, "%d", a[i] + j) != 1) {printf("Error read!\n"); break;}}
		fclose(fp);
	}
	

	for(int i = 0; i < argc - 1; i++)
		{printf("%s: ", argv[i + 1]); print_array(a[i], a_size[i]);}

	void ** stack_tmp = malloc(argc_c * sizeof(stack_tmp[0]));

	contexts = malloc(argc_c * sizeof(ucontext_t));

	t = (double *)malloc((argc - 1) * sizeof(double));
	t_start = (double *)malloc((argc - 1) * sizeof(double));
	t_end = (double *)malloc((argc - 1) * sizeof(double));

	for(int i = 0; i < argc_c; i++) 
	{
		t_start[i] = t_end[i] = t[i] = 0;

		getcontext(&contexts[i]);
		contexts[i].uc_stack.ss_sp = allocate_stack_mprot();
		stack_tmp[i] = contexts[i].uc_stack.ss_sp;
		contexts[i].uc_stack.ss_size = STACK_SIZE;
		if(i < argc_c) contexts[i].uc_link = &main_context;
		makecontext(&contexts[i], (void (*)(void))sort, 3, a[i], a_size[i], i);
	}
	
	printf("Sorting...\n"); 
	t_start[0] = clock(); 
	double time = clock(); swapcontext(&main_context, &contexts[0]); time = (clock() - time) / CLOCKS_PER_SEC;
	printf("End sorting...\n"); 

	for(int i = 0; i < argc_c; i++)
		{printf("%s: ", argv[i + 1]); print_array(a[i], a_size[i]);}

	FILE * fp = fopen("res.txt", "w");
	merge(a, a_size, total_size, fp);
	fclose(fp);

	for(int i = 0; i < argc_c; i++) 
	{
		free(a[i]);
		free(stack_tmp[i]);
		printf("test%d time: %.2lf\n", i + 1, t[i] / CLOCKS_PER_SEC);
	}
	printf("Total time of sorting: %.2lf\n", time);
	free(contexts);
	free(t); free(t_start); free(t_end);
	free(c_finish); free(a); free(a_size);
	return 0;
}

