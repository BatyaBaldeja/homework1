#include <memory>
#include <algorithm>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include <ucontext.h>
#include <signal.h>
#include <sys/mman.h>

#define LEN 		1000
#define MAX_PRINT 	20
#define MAX_PRINT_RES 1000
#define stack_size 1024 * 1024
#define handle_error(msg) do {perror(msg); exit(EXIT_FAILURE);} while (0)


int cmp(int a, int b) {return a - b;}
void swap(int * a, int * b) {int buf = * a; *a = *b; *b = buf; return;}
void free_a(int ** a, int n) {for(int i = 0; i < n; i++) free(a[i]);}

void print_array(int * a, int size_of_array)
{
    size_of_array = (size_of_array > MAX_PRINT) ? MAX_PRINT : size_of_array;
    for(int i = 0; i < size_of_array; i++)
    	printf("%d ", a[i]);
    printf("\n");
}

void qsort(int * a, int size_of_array)
{
	if(size_of_array < 2) return;
	int x = a[size_of_array >> 1], i, j;

	for(i = 0; j = size_of_array - 1; i++, j--) 
	{
		while(a[i] < x) i++;
		while(a[j] > x) j--;
		if(i >= j) break;
		swap(a + i, a + j);
	}
	qsort(a, i);
	qsort(a + i, size_of_array - i);
}

void merge(int ** a, int * a_size, int n, int total_size)
{
	int curr[n]; for(int i = 0; i < n; i++) curr[i] = 0;
	int min = 0, tmp = 0;
	for(int k = 0; k < total_size; k++)
	{	
		int j = 0;
		while(j < n && curr[j] >= a_size[j]) j++;
			if(j == n) return;
			else {min = a[j][curr[j]]; tmp = j;}

		for(int i = 0; i < n; i++)
		{
			if(curr[i] < a_size[i])
				if(a[i][curr[i]] < min)
					{min = a[i][curr[i]]; tmp = i;}
		}
		if(k < MAX_PRINT_RES) printf("%d ", min);
		curr[tmp]++;
	}
	printf("\n");
}

static ucontext_t uctx_main, uctx_func[LEN];

static void * allocate_stack_mprot()
{
	void * stack = malloc(stack_size);
	mprotect(stack, stack_size, PROT_READ | PROT_WRITE | PROT_EXEC);
	return stack;
}

static void my_coroutine(int id, int n, int ** a, int * a_size)
{
	printf("test%d: started\n", id + 1);
	if(swapcontext(&uctx_func[id], &uctx_func[(id + 1) % (n)]) == -1)  handle_error("swapcontext");
	double t = clock();
	qsort(a[id], a_size[id]); 
	t = (clock() - t) / CLOCKS_PER_SEC;
	printf("Time: %.2lf, test%d: ", t, id + 1); 
	print_array(a[id], a_size[id]);
	printf("test%d: returning\n", id + 1);
}

int main (int argc, char * argv[])
{	
	int * a[argc - 1]; int a_size[argc - 1], n = 0, tmp = 0;
	for(int i = 0; i < argc - 1; i++) 
	{
		FILE * fp;
		if(!(fp = fopen(argv[i + 1], "r"))) 
			{printf("Cannot open %s!\n", argv[i + 1]); free_a(a, i); return 1;}
		while(fscanf(fp, "%d", &tmp) == 1) n++; rewind(fp);
		if(!(a[i] = (int *)malloc(n * sizeof(int)))) 
			{printf("Not enough memory!\n"); fclose(fp); free_a(a, i); return 2;}
		a_size[i] = n; n = 0;
		for(int j = 0; j < a_size[i]; j++)
		{if(fscanf(fp, "%d", a[i] + j) != 1) {fclose(fp); free_a(a, i); return 3;}}
		fclose(fp);
	}
	

	for(int i = 0; i < argc - 1; i++)
		{printf("%s: ", argv[i + 1]); print_array(a[i], a_size[i]);}


	char * func_stack[argc - 1];
	for(int i = 0; i < argc - 1; i++) 
	{
		func_stack[i] = (char *)allocate_stack_mprot();
		if(getcontext(&uctx_func[i]) == -1) handle_error("getcontext");
		uctx_func[i].uc_stack.ss_sp = func_stack[i];
		uctx_func[i].uc_stack.ss_size = stack_size;
		//uctx_func[i].uc_link = (i < argc - 1) ? &uctx_func[i + 1] : &uctx_func[0];
		if(i < argc - 2) uctx_func[i].uc_link = &uctx_func[i + 1];
		else uctx_func[i].uc_link = &uctx_main;
		makecontext(&uctx_func[i], my_coroutine, 4, i, argc - 1, a, a_size);
	}

	printf("Sorting...\n"); 
	//double t = 0;
	//for(int i = 0; i < argc - 1; i++)
	//{
		//t = clock();
		if(swapcontext(&uctx_main, &uctx_func[0]) == -1) handle_error("swapcontext");
		for(int i = 0; i < argc - 1; i++) free(func_stack[i]); // HOW TO FREEEEE?
		//t = (clock() - t) / CLOCKS_PER_SEC;
		//printf("Time: %.2lf, %s: ", t, argv[i + 1]); 
		//print_array(a[i], a_size[i]);
	//}


	int total_size = 0; for(int i = 0; i < argc - 1; i++) total_size += a_size[i];
	printf("Result: \n");
	merge(a, a_size, argc, total_size);


	free_a(a, argc - 1);
	//free(func_stack);
	//free(stack);
	return 0;
}

