#include<stdio.h>
#include<math.h>
#include "time.h"
#include <stdlib.h>
#include <set>
#include<iterator>
#include<iostream>
#include"cstdlib"
#include"stdlib.h"
#include"stdio.h"
#include"time.h"
#include"sys/timeb.h"
#include"windows.h"
#include<conio.h>

#define n_bit 7

#define NUM 7
#define n_S (1<<n_bit)
#define n_T 20000
#define n_L 20000
double M[5];
using namespace std;
struct element {     
	int data;  
	int index;  
};
int cmp(const void *a, const void *b) {  
	return((struct element*)a)->data - ((struct element*)b)->data;
}
void rand_of_n(int a[], int n)  
{
	int i;
	struct element ele[n_S];
	struct timeb timer;
	ftime(&timer);
	srand(timer.time * 1000 + timer.millitm); 
	for (i = 0; i<n; i++) {
		ele[i].data = rand(); 
		ele[i].index = i + 1;
	}
	qsort(ele, n, sizeof(ele[0]), cmp);  
	for (i = 0; i<n; i++)
	{
		a[i] = ele[i].index;
	}
	for (i = 0; i < n; i++)
		a[i] = a[i] - 1;
}
struct equation
{
	int F1;
	int F2;
	int c;
};
struct L
{
	struct equation position;
	int flag;
};
void initialize(int input1[4], int input2[4], int S[n_S], int y)
{
	S[input1[0]] = 0;
	S[input2[0]] = input1[1] ^ input2[1] ^ y;
}
int judge_T(int T[n_T])
{
	int i;
	for (i = 0; i< n_T; i++)
		if (T[i] != -1)
			return 1;
	return 0;
}
int insertion_T(int k, int T[n_T])
{
	int i;
	for (i = 0; i < n_T; i++)
		if (T[i] == -1)
		{
			T[i] = k;
			return 0;
		}
}
int expulsion_T(int T[n_T])
{
	int k;
	for (int i = 0; i < n_T; i++)
		if (T[i] != -1)
		{
			k = T[i];
			T[i] = -1;
			return k;
		}
	return -1;
}
int substitute_into_L(int k, L equation_L[n_L], int T[n_T], int S[n_S], int flag_i[1])
{
	int f1, f2;
	int i, s;
	for (i = 0; i < n_L; i++)
	{
		if (equation_L[i].flag == 1)
			if (k == equation_L[i].position.F1)
			{
				equation_L[i].flag = 0;
				f1 = equation_L[i].position.F1;
				f2 = equation_L[i].position.F2;
				s = S[k] ^ equation_L[i].position.c;
				equation_L[i].position.F1 = -1;
				equation_L[i].position.F2 = -1;
				equation_L[i].position.c = 0;
				if (S[f2] == -1)
				{
					S[f2] = s;
					insertion_T(f2, T);
				}
				else if (S[f2] == s)
				{
					flag_i[0]++;
				}
				else return 0;
			}
			else if
				(k == equation_L[i].position.F2)
			{
				equation_L[i].flag = 0;
				f1 = equation_L[i].position.F1;
				f2 = equation_L[i].position.F2;
				s = S[k] ^ equation_L[i].position.c;
				equation_L[i].position.F1 = -1;
				equation_L[i].position.F2 = -1;
				equation_L[i].position.c = 0;
				if (S[f1] == -1)
				{
					S[f1] = s;
					insertion_T(f1, T);
				}
				else if (S[f1] == s)
				{
					flag_i[0]++;
				}
				else return 0;
			}
	}
	return 1;
}
void generate_connected(int input1[4], int input2[4], int y, int s[7][n_S])
{
	for (int i = 0; i < 4; i++)
		input1[i] = rand()&(n_S - 1);                 

    L1:;
    
	for (int i = 0; i < 4; i++)
		input2[i] = rand()&(n_S - 1);
		
	if (input1[0] == input2[0])
		goto L1;

	input1[1] = s[0][input1[0]] ^ s[0][input2[0]] ^ input2[1] ^ y;
}
int judge_equation(int input1[2], int input2[2], int y, int S[n_S], L equation_L[n_L], int T[n_T], int flag_i[1])
{

	equation text;
	int i, k, k1, k2;

	if (S[input1[0]] != -1 && S[input2[0]] != -1)
	{
		k1 = S[input1[0]] ^ S[input2[0]];
		k2 = y^input1[1] ^ input2[1];
		if (k1 == k2)
			flag_i[0]++;
		else
		{
			return 0;
		}
	}
	else if (S[input1[0]] == -1 && S[input2[0]] == -1)
	{
		text.F1 = input1[0];
		text.F2 = input2[0];
		text.c = y^input1[1] ^ input2[1];
		for (i = 0; i < n_L; i++)
		{
			if (equation_L[i].position.F1 == text.F1 && equation_L[i].position.F2 == text.F2 && equation_L[i].position.c != text.c)
				return 0;
			else if (equation_L[i].position.F1 == text.F2 && equation_L[i].position.F2 == text.F1 && equation_L[i].position.c != text.c)
				return 0;
			else if (equation_L[i].position.F1 == text.F1 && equation_L[i].position.F2 == text.F2 && equation_L[i].position.c == text.c)
			{
				flag_i[0]++;
				return 1;
			}
			else if (equation_L[i].position.F1 == text.F2 && equation_L[i].position.F2 == text.F1 && equation_L[i].position.c == text.c)
			{
				flag_i[0]++;
				return 1;
			}
		}
		for (i = 0; i < n_L; i++) 
			if (equation_L[i].position.F1 == -1 && equation_L[i].position.F2 == -1)//
			{
				equation_L[i].position = text;
				equation_L[i].flag = 1;
				break;
			}
	}
	else if (S[input1[0]] != -1 && S[input2[0]] == -1)
	{
		S[input2[0]] = y^input1[1] ^ input2[1] ^ S[input1[0]];
		insertion_T(input2[0], T);
		while (judge_T(T) == 1)
		{
			k = expulsion_T(T);
			if (substitute_into_L(k, equation_L, T, S, flag_i) == 0)
				return 0;
		}
	}
	else if (S[input1[0]] == -1 && S[input2[0]] != -1)
	{
		S[input1[0]] = y^input1[1] ^ input2[1] ^ S[input2[0]];
		insertion_T(input1[0], T);
		while (judge_T(T) == 1)
		{
			k = expulsion_T(T);
			if (substitute_into_L(k, equation_L, T, S, flag_i) == 0)
				return 0;
		}
	}
	return 1;
}
int find(int s[7][n_S])
{
	L equation_L[n_L] = { 0 };
	int  T[n_T];
	int S[n_S];
	int y;
	int input1[4],input2[4];
	int con;
	int flag_i[1];
	int kt;
	int total;
	float sum_c1[5];
	float sum_c2[5];
	
	for (int t = 0; t < 100000; t++)
	{
		if((t%1000)==0) printf("Experimental progress: %3d percent.\n",t/1000+1);
		{
			for (int i = 0; i < n_L; i++)
			{
				equation_L[i].flag = 0;
				equation_L[i].position.F1 = -1;
				equation_L[i].position.F2 = -1;
				equation_L[i].position.c = -1;
			}                                                               

			for (int i = 0; i < n_T; i++)
				T[i] = -1;                                                 

			for (int i = 0; i < n_S; i++)
				S[i] = -1;
				
		    L1:;
			y = rand()&(n_S - 1);
			if (y == 0)
				goto L1;                       

			generate_connected(input1, input2, y, s);
			initialize(input1, input2, S, y);
			//con = input1[0];
			flag_i[0] = 0;

		}
		for (int k = 2; k <= 1000000; k = k + 1)
		{
			generate_connected(input1, input2, y, s);
			kt = judge_equation(input1, input2, y, S, equation_L, T, flag_i);

			for (int i = 0; i < n_S; i++)
				if (S[i] == -1)goto keepon;
			{
				/*printf("\nThe recovered S-box: ");
				for(int j=0;j<n_S;j++)
					printf("%d,",S[j]^s[0][con]);
				printf("\n");*/
				for(int j=4;j>=0;j--)
				{
					if(k <= M[j])
					{
						sum_c1[j] = sum_c1[j] + 1;
						if (sum_c1[j] > 10000000)
						{
							sum_c1[j] = sum_c1[j] - 10000000;
							sum_c2[j]++;
						}
					}
					else
						break;
				}
				total = total + 1;
				goto L7;
			}
		keepon:;
		}
	L7:;
	}
	printf("\nFor a %d-bit secert function, we set E=", n_bit);
	for(int i=0;i<5;i++)
	{
		printf("%d,",int(M[i]));
	}
	printf(" then the corresponding probabilities of G_{yoyo} be connected are as follows:\n"); 
	printf("Theoretical probabilities: 0.200000,0.400000,0.500000,0.600000,0.800000;\n");
	printf("Experimental probabilities:");
	for(int i=0;i<5;i++)
		printf("%f,", (sum_c2[i] / 100000) * 10000000 + (sum_c1[i] / 100000));
	return 0;

}
int main()
{
	int s[NUM][n_S];


	for (int j = 0; j < NUM; j++)
	{
		rand_of_n(s[j], n_S);
		Sleep(3);
	}
	
	
	M[0]=(n_S*(log(n_S)+log(1/log(1/0.2))))/2;
	M[1]=(n_S*(log(n_S)+log(1/log(1/0.4))))/2;
	M[2]=(n_S*(log(n_S)+log(1/log(1/0.5))))/2;
	M[3]=(n_S*(log(n_S)+log(1/log(1/0.6))))/2;
	M[4]=(n_S*(log(n_S)+log(1/log(1/0.8))))/2;
		
	find(s);
	getchar();
}


