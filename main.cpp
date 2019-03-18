#include "strings_.h"

int input_str(char* name);

int main (int argc, char *argv[])
{
    const char *filename1, *filename2;
    
    if(argc != 4)
	{printf("Usage: %s input.txt output.txt task!\n", argv[0]); return 1;}

    int number = atoi(argv[3]);
    if(number < 1 || number > 9) 
	{printf("Error number of task!\n"); return 2;}
    filename1 = argv[1]; filename2 = argv[2];
    
    char s[LEN], str[LEN]; int res = 0; double t = 0;
    
    printf("Input string s: "); input_str(s); printf("String s: \"%s\"\n", s);
    printf("Input string t: "); input_str(str); printf("String t: \"%s\"\n", str);
	
    strings_ a(filename1, filename2);
    a.input_s(s);
    a.input_d(str);
        
    switch(number) 
	{
		case 1: t = clock(); res = a.task1(); t = (clock() - t) / CLOCKS_PER_SEC; break;
        case 2: t = clock(); res = a.task2(); t = (clock() - t) / CLOCKS_PER_SEC; break;
		case 3:
		{
			char r[LEN];
			printf("Input string r: "); input_str(r); printf("String r: \"%s\"\n", s);
			strings_ a(filename1, filename2); 
			a.input_s(s); a.input_r(r); 
			a.input_d(str);
			t = clock(); 
			res = a.task3(); 
			t = (clock() - t) / CLOCKS_PER_SEC; 
			break;
		}
		case 4:
		{
			char x[LEN];
			printf("Input string x: "); 
			if(input_str(x) > 2) 
			{printf("Error, x cannot be this string!\n"); return 6;} 
			printf("String x: \"%s\"\n", x);	
			strings_ a(filename1, filename2);
			a.input_s(s);
			a.input_d(str);
			if(a.input_x(x)) 
			{printf("Error, x cannot be this string!\n"); return 7;}
			t = clock();
			res = a.task4(); 
			t = (clock() - t) / CLOCKS_PER_SEC; 
			break;
		}
		case 5:
		{
			char x[LEN];
			printf("Input string x: "); 
			if(input_str(x) > 2) 
			{printf("Error, x cannot be this string!\n"); return 6;} 
			printf("String x: \"%s\"\n", x);		
			strings_ a(filename1, filename2);
			a.input_s(s);
			a.input_d(str);
			if(a.input_x(x)) 
			{printf("Error, x cannot be this string!\n"); return 7;}
			t = clock();
			res = a.task5(); 
			t = (clock() - t) / CLOCKS_PER_SEC; 
			break;	
		}
        case 6: t = clock(); res = a.task6(); t = (clock() - t) / CLOCKS_PER_SEC; break;
        case 7: t = clock(); res = a.task7(); t = (clock() - t) / CLOCKS_PER_SEC; break;
        case 8: t = clock(); res = a.task8(); t = (clock() - t) / CLOCKS_PER_SEC; break;
        case 9: t = clock(); res = a.task9(); t = (clock() - t) / CLOCKS_PER_SEC; break;
        default: printf("Error number of task: %d\n", number); break;
	}    
    printf("\nTime = %.2f\nResult = %d\n", t, res);
    return 0;
}

int input_str(char* name)
{
    if(!fgets(name, LEN, stdin))
	{printf("Error read!\n"); return 1;}
    int k = (strlen(name) - 1);
    if(name[k] && name[k] == '\n') {name[k] = '\0';}
	return k;
}












