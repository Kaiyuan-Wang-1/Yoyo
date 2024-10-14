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

#define NUM 7
#define n_bit 5
#define n_S (1<<n_bit)
int c1[1 << (4 * n_bit)]= {0}; 
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
	
	for (i = 0; i < n; i++) 
		a[i] = a[i] - 1;
}
void Round_dec(int input[4], int s[7][n_S], int output[4], int flag)
{
	input[0] = output[0];
	input[1] = s[flag - 1][input[0]] ^ output[1];
	input[2] = s[flag - 1][input[0]] ^ output[2];
	input[3] = s[flag - 1][input[0]] ^ output[3];
}
void Round_exchange(int output[4])
{
	int t;
	
	t = output[0];
	output[0] = output[1];
	output[1] = output[2];
	output[2] = output[3];
	output[3] = t;
}
void Round_enc(int input[4], int s[7][n_S], int output[4], int flag)
{
	output[0] = input[0];
	output[1] = s[flag - 1][input[0]] ^ input[1];
	output[2] = s[flag - 1][input[0]] ^ input[2];
	output[3] = s[flag - 1][input[0]] ^ input[3];
}
void Round_invexchange(int input[4])
{
	int t;
	
	t = input[3];
	input[3] = input[2];
	input[2] = input[1];
	input[1] = input[0];
	input[0] = t;
}
void Enc(int input[4], int output[4], int s[7][n_S])
{
	for (int i = 1; i < 7; i++)
	{
		Round_enc(input, s, output, i);
		Round_exchange(output);
		
		for (int j = 0; j < 4; j++)
			input[j] = output[j];
	}
	Round_enc(input, s, output, 7);
}
void Dec(int input[4], int output[4], int s[7][n_S])
{
	for (int i = 7; i > 1; i--)
	{
		Round_dec(input, s, output, i);
		Round_invexchange(input);
		
		for (int j = 0; j < 4; j++)
			output[j] = input[j];
	}
	Round_dec(input, s, output, 1);
}
int judge_connected_pair(int input1[4], int input2[4], int s[7][n_S], int y)
{
	int output1[4], output2[4];
	int i, j, a[4], b[4];

	for (i = 0; i <4; i++)
	{
		a[i] = input1[i];
		b[i] = input2[i];
	}
	
	Round_enc(a, s, output1, 1);
	Round_exchange(output1);
	Round_enc(b, s, output2, 1);
	Round_exchange(output2);
	
	if ((output1[0] ^ output2[0]) == y)
	{
		
		for (j = 0; j < 4; j++)
		{
			a[j] = output1[j];
			b[j] = output2[j];
		}
		
		Round_enc(a, s, output1, 2);
		Round_exchange(output1);
		Round_enc(b, s, output2, 2);
		Round_exchange(output2);
	
		if (output1[0] == output2[0])
		{
			for (j = 0; j < 4; j++)
			{
				a[j] = output1[j];
				b[j] = output2[j];
			}
			
			Round_enc(a, s, output1, 3);
			Round_exchange(output1);
			Round_enc(b, s, output2, 3);
			Round_exchange(output2);
	
			if (output1[0] == output2[0])
			{		
				for (j = 0; j < 4; j++)
				{
					a[j] = output1[j];
					b[j] = output2[j];
				}
	
				Round_enc(a, s, output1, 4);
				Round_exchange(output1);
				Round_enc(b, s, output2, 4);
				Round_exchange(output2);
	
				if (output1[0] == output2[0])
				{ 
					return 1;
				}
				else
					return 0;
			}
			else
				return 0;
		}
		else
			return 0;
	}
	else
		return 0;
}
void f_operation(int input1[4], int input2[4], int y)
{
	int t;
	
	t = input1[2]; input1[2] = input1[3]; input1[3] = t;
    t = input2[2]; input2[2] = input2[3]; input2[3] = t;

	input1[1] = input1[1] ^ y;  
	input2[1] = input2[1] ^ y;  
	input1[2] = input1[2] ^ 3 ;  
	input2[2] = input2[2] ^ 3 ;
	input1[3] = input1[3] ^ 1;
	input2[3] = input2[3] ^ 1;
}
void g_operation(int output1[4], int output2[4], int y)
{
	int t;
	
	t = output1[1]; output1[1] = output1[2]; output1[2] = t;
	t = output2[1]; output2[1] = output2[2]; output2[2] = t;
	
    output1[1] = output1[1]^2;
	output2[1] = output2[1]^2;
	output1[2] = output1[2]^4 ;
	output2[2] = output2[2]^4;
	output1[3] = output1[3]^y;  
	output2[3] = output2[3]^y;  
}
void Partial_Dec(int input[4], int output[4], int s[7][n_S])
{
	int i, j;
	
	for (i = 2; i > 1; i--)
	{
		Round_dec(input, s, output, i);
		Round_invexchange(input);
		for (j = 0; j < 4; j++)
			output[j]= input[j];
	}
	
	Round_dec(input, s, output, 1);
}
void Mark_connected_pair(int input1[4], int input2[4],int s[7][n_S],int y)
{
	int i, j; 
	int temp1[4], temp2[4]; 
	int output1[4], output2[4];
	{
			for (j = 0; j < 4; j++)
			{
				temp1[j] = input1[j];
				temp2[j] = input2[j];
			}
	
			Round_enc(temp1, s, output1, 1);
			Round_exchange(output1);
			for (int j = 0; j < 4; j++)
				temp1[j] = output1[j];
			Round_enc(temp1, s, output1, 2);
	
			Round_enc(temp2, s, output2, 1);
			Round_exchange(output2);
			for (int j = 0; j < 4; j++)
				temp2[j] = output2[j];
			Round_enc(temp2, s, output2, 2);
	
			c1[output1[3] ^ (output1[2]  << (1 * n_bit))  ^  (output1[1] << (2*n_bit))^  (output1[0] << (3*n_bit))]+= 1;
			c1[output2[3] ^ (output2[2]  << (1 * n_bit) ) ^  (output2[1] << (2*n_bit)) ^ (output2[0] << (3*n_bit))]+= 1;
	}
}
void Mars_find(int s[7][n_S])
{
	int y = 1;
	int input1[4], input2[4];
	int output1[4], output2[4];
	int temp1[4], temp2[4];
	int i;
	int time=0; 
	int L=0;
	
	for (int i = 0; i < (1 << (4 * n_bit)); i++) c1[i] = {0}; 

	for (int i = 0; i < (1 << (4 * n_bit)); i++)
		if (c1[i] == 0)                                 
			{
			//	printf("£¨%d£©,",i);
				time++;
				
				output1[0] = (i&(((1 << (4 * n_bit)) - 1) - ((1 << 3 * n_bit) - 1))) >> 3 * n_bit;
				output1[1] = (i&(((1 << (3 * n_bit)) - 1) - ((1 << 2 * n_bit) - 1))) >> 2 * n_bit;
				output1[2] = (i&(((1 << (2 * n_bit)) - 1) - ((1 << 1 * n_bit) - 1))) >> 1 * n_bit;
				output1[3] =  i&(( 1 << (1 * n_bit)) - 1);
				  
				output2[0] = output1[0]^y;
				output2[1] = output1[1];
				output2[2] = output1[2];
				output2[3] = output1[3];
		    
				Partial_Dec(input1, output1, s);
				Partial_Dec(input2, output2, s); 
		
				for (int j = 0; j < 4; j++) 
				{
					temp1[j] = input1[j];                   
					temp2[j] = input2[j];
				}	
				
				L=0;
	
				do
				{
					Mark_connected_pair(input1, input2,s,y);
					
					f_operation(input1, input2, y);
		   
					Enc(input1, output1, s);
					Enc(input2, output2, s);
		
					g_operation(output1, output2, y);

					Dec(input1, output1, s);
					Dec(input2, output2, s);
		
					L=L+1;
		
				} while ((input1[0] != temp1[0] || input1[1] != temp1[1] || input1[2] != temp1[2] || input1[3] != temp1[3]|| input2[0] != temp2[0] || input2[1] != temp2[1] || input2[2] != temp2[2] || input2[3] != temp2[3])&&
						 (input1[0] != temp2[0] || input1[1] != temp2[1] || input1[2] != temp2[2] || input1[3] != temp2[3]|| input2[0] != temp1[0] || input2[1] != temp1[1] || input2[2] != temp1[2] || input2[3] != temp1[3]));	
				for(int j=1;j<=15;j++) 
					{
						if(time==j) F[j]=L; 
					}	
			}
}
int main()
{
	int s[NUM][n_S];
	float sum_c1[31] = {0}, sum_c2[31] = {0};
	//printf("%d,",n_bit);
	float t=1;
	for (int i = 0; i < 100000; i++)
	{	
		if((i%1000)==0) printf("Experimental progress: %3d percent.\n",i/1000);
		
		for (int j = 0; j < NUM; j++)
		{
			rand_of_n(s[j], n_S);
			Sleep(3);
		}
		
		Mars_find(s);
	
		for(int j=1;j<=15;j++)
		{
			sum_c1[j] = sum_c1[j] +F[j]; 
			F[j]=0;
			if (sum_c1[j] > 10000000)
			{
				sum_c1[j] = sum_c1[j] - 10000000;
				sum_c2[j]++;
			}
		}
		
	}
	
	printf("\nConditions: 7-round Mars,n=%d.\n Result: \n", n_bit); 
	for(int j=1;j<=15;j++)
	{
			t = t * 2;
			printf("E(L_{x_%d})=%f,\n <L_{x_%d}>=%f.\n\n",  j,  (1 << (4 * n_bit-1))/(t), j, (sum_c2[j] / 100000) * 10000000 + (sum_c1[j] / 100000)); 
	}   

	getchar(); 
	
	return 0;
}
