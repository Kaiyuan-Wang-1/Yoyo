#include <stdio.h>
#include "time.h"
#include <stdlib.h>
#include <math.h>
#include <set>
#include<iterator>
#include<iostream>
#include"cstdlib"
#include"stdlib.h"
#include"stdio.h"
#include"time.h"
#include"sys/timeb.h"
#include"windows.h"

#define NUM 5                           
#define n_bit 8                       
#define n_S (1<<n_bit)                  
int c1[1<<(2* n_bit)] = {0};           
float F[16]={0};                        

using namespace std;
struct element {     
	int data;  
	int index;  
};
int cmp(const void *a, const void *b) 
{   
	return((struct element*)a)->data - ((struct element*)b)->data;
}
void rand_of_n(int a[], int n) 
{
	int i;
	struct element ele[n_S];
	struct timeb timer;
	ftime(&timer);
	srand(timer.time * 1000 + timer.millitm);   
	for (i = 0; i<n; i++) 
	{
		ele[i].data = rand();  
		ele[i].index = i + 1;
	}
	qsort(ele, n, sizeof(ele[0]), cmp);  
	for (i = 0; i<n; i++)
	{
		a[i] = ele[i].index;
	}
	for (i = 0; i < n; i++) a[i] = a[i] - 1;
}
void Round_enc(int input[2], int s[5][n_S], int output[2], int flag)
{
	output[1] = input[1];
	output[0] = s[flag - 1][input[1]] ^ input[0];
}
void Round_exchange(int output[2])
{
	int t;
	t = output[0];
	output[0] = output[1];
	output[1] = t;
}
void Round_dec(int input[2], int s[5][n_S], int output[2], int flag)
{
	input[0] = s[flag - 1][output[1]] ^ output[0];
	input[1] = output[1];
}
void Round_invexchange(int input[2])
{
	int t;
	t = input[1];
	input[1] = input[0];
	input[0] = t;
}
void Enc(int input[2], int output[2], int s[5][n_S])
{
	int i, j;
	for (i = 1; i < 5; i++)
	{
		Round_enc(input, s, output, i);
		Round_exchange(output);
		for (j = 0; j < 2; j++)
			input[j] = output[j];
	}
	Round_enc(input, s, output, 5);
}
void Dec(int input[2], int output[2], int s[5][n_S]) 
{
	int i, j;
	for (i = 5; i > 1; i--)
	{
		Round_dec(input, s, output, i);
		Round_invexchange(input);
		for (j = 0; j < 2; j++)
			output[j] = input[j];
	}
	Round_dec(input, s, output, 1);
}
void f_operation(int input1[2], int input2[2], int s[5][n_S], int y)
{
	input1[0] = input1[0] ^ y;  
	input2[0] = input2[0] ^ y; 
}
void g_operation(int input1[2], int input2[2], int s[5][n_S], int y)
{
	input1[0] = input1[0] ^ y;  
	input2[0] = input2[0] ^ y;  
}
int judge_connected_pair(int input1[2], int input2[2], int s[5][n_S], int y) 
{
	int output1[2], output2[2];
	int temp1[2], temp2[2];

	for (int i = 0; i <2; i++)
	{
		temp1[i] = input1[i];
		temp2[i] = input2[i];
	}
	Round_enc(temp1, s, output1, 1);
	Round_enc(temp2, s, output2, 1);
	Round_exchange(output1);	
	Round_exchange(output2);
	if ((output1[1] ^ output2[1]) == y)
	{
		for (int i = 0; i < 2; i++)
		{
			temp1[i] = output1[i];
			temp2[i] = output2[i];
		}
		Round_enc(temp1, s, output1, 2);
		Round_enc(temp2, s, output2, 2);
		Round_exchange(output1);	
		Round_exchange(output2);
		
		if (output1[1] == output2[1])
			return 1; 
		else
			return 0;
	}
	else
		return 0;
}
void Partial_Dec(int input[2], int output[2], int s[5][n_S])  
{
	int i, j;
	for (i = 2; i > 0; i--)
	{
		Round_invexchange(output);
		Round_dec(input, s, output, i);
		for (j = 0; j < 2; j++)
			output[j] = input[j];
	}
}
void Mark_connected_pair(int input1[2], int input2[2], int s[5][n_S], int y)
{
	int i, j; 
	int temp1[2], temp2[2]; 
	int output1[2], output2[2];
	{
		for (j = 0; j < 2; j++)
		{
			temp1[j] = input1[j];
			temp2[j] = input2[j];
		}

		for (i = 1; i <= 2; i++)
		{
			Round_enc(temp1, s, output1, i);
			Round_exchange(output1);
			for (j = 0; j < 2; j++)
				temp1[j] = output1[j];

			Round_enc(temp2, s, output2, i);
			Round_exchange(output2);
			for (j = 0; j < 2; j++)
				temp2[j] = output2[j];
		}
		c1[output1[1] + output1[0] * (1 << n_bit)] += 1;
		c1[output2[1] + output2[0] * (1 << n_bit)] += 1;
	}
	
	{
		for (j = 0; j < 2; j++)
		{
			temp1[j] = input1[j];
			temp2[j] = input2[j];
		}

		f_operation(temp1, temp2, s, y);

		for (i = 1; i <= 2; i++)
		{
			Round_enc(temp1, s, output1, i);
			Round_exchange(output1);
			for (j = 0; j < 2; j++)
				temp1[j] = output1[j];

			Round_enc(temp2, s, output2, i);
			Round_exchange(output2);
			for (j = 0; j < 2; j++)
				temp2[j] = output2[j];
		}
		c1[output1[1] + output1[0] * (1 << n_bit)] += 1;
		c1[output2[1] + output2[0] * (1 << n_bit)] += 1;
	}
}
void Feistel_find(int s[5][n_S])
{
	int input1[2], input2[2];
	int output1[2], output2[2];
	int temp1[2], temp2[2];
	float L;
	int time = 0;
	int y = rand()%(n_S-1)+1;

	for (int i = 0; i < (1 << (2 * n_bit)); i++) c1[i] = {0};

	for (int i = 0; i < (1 << (2 * n_bit)); i++)
		if (c1[i] == 0)                                
		{
			time++;                                      

			output1[0] = (i&(((1 << (2 * n_bit)) - 1) - ((1 << n_bit) - 1))) >> n_bit;
			output1[1] = i&((1 << n_bit) - 1);
			output2[0] = output1[0] ^ y;
			output2[1] = output1[1];                          

			Partial_Dec(input1, output1, s);
			Partial_Dec(input2, output2, s);             

			for (int j = 0; j < 2; j++)                            
			{
				temp1[j] = input1[j];
				temp2[j] = input2[j];
			}

			L = 0;                                  

			do
			{
				Mark_connected_pair(input1, input2, s, y);  
		
				{
					f_operation(input1, input2, s, y);

					Enc(input1, output1, s);
					Enc(input2, output2, s);

					for (int j = 0; j < 2; j++)
					{
						input1[j] = output1[j];
						input2[j] = output2[j];
					}

					g_operation(input1, input2, s, y);

					Dec(output1, input1, s);
					Dec(output2, input2, s);

					for (int j = 0; j < 2; j++)
					{
						input1[j] = output1[j];
						input2[j] = output2[j];
					}
				} 

				L = L + 1;              

				
			} while ((input1[0] != temp1[0] || input1[1] != temp1[1] || input2[0] != temp2[0] || input2[1] != temp2[1]) && (input1[0] != temp2[0] || input1[1] != temp2[1] || input2[0] != temp1[0] || input2[1] != temp1[1]));

			for(int j=1;j<=15;j++) 
			{
				if(time==j) F[j]=L*2; 
			}	
		}
}
int main()
{
	int s[5][n_S];
	float sum_c1[16]={0}, sum_c2[16] = {0}; 
	float t=1;
	
	for (int i = 0; i < 100000; i++)
	{
		if((i%1000)==0) printf("Experimental progress: %3d percent.\n",i/1000);
		
		for (int j = 0; j < NUM; j++)
		{
			rand_of_n(s[j], n_S);
			Sleep(3);
		}

		Feistel_find(s);
		
		for(int j=1;j<=15;j++)
		{
			sum_c1[j] = sum_c1[j] +F[j]; 
			
			if (sum_c1[j] > 10000000)
			{
				sum_c1[j] = sum_c1[j] - 10000000;
				sum_c2[j]++; 
			}
			
			F[j]=0;
		}
	}

	printf("\nConditions: 5-round Feistel,n=%d.\n Result: \n", n_bit); 
	
	for(int j=1;j<=15;j++)
	{
			t = t * 3;
			printf("E(|F_{x_%d}|)=%f,\n <|F_{x_%d}|>=%f.\n\n",  j, (1 << (2 * n_bit-1))/(t/2), j, (sum_c2[j] / 100000) * 10000000 + (sum_c1[j] / 100000)); 
	}     
	getchar();	
	return 0;

}
