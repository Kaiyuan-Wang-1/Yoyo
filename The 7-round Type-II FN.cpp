#include <stdio.h>
#include "time.h"
#include <stdlib.h>
#include <math.h>
#include <set>
#include <iterator>
#include <iostream>
#include "cstdlib"
#include "stdlib.h"
#include "stdio.h"
#include "time.h"
#include "sys/timeb.h"
#include "windows.h"

#define   NUM    14
#define   n_bit  6
#define   N1     1<<(4*n+1)
#define   n_S    (1<<n_bit)
float     F[20]={0};
float     E_F[20]={0};
int       c1[300000][5];

using namespace std;
struct element {     
	int data;   
	int index;  
};
int cmp(const void *a, const void *b) { 
		return((struct element*)a)->data - ((struct element*)b)->data;
	}
void rand_of_n(int a[], int n) {
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
void Round_enc(int input[4], int s[14][n_S], int output[4], int flag)
{ 
	output[0] = input[0];
	output[1] = s[2 * flag - 2][input[0]] ^ input[1];
	output[2] = input[2];
	output[3] = s[2 * flag - 1][input[2]] ^ input[3];
}
void Round_dec(int input[4], int s[14][n_S], int output[4], int flag)
{
	input[0]  =  output[0] ;
	input[1]  =  s[2 * flag - 2][input[0]] ^output[1];
	input[2]  =  output[2];
	input[3]  =  s[2 * flag - 1][input[2]] ^ output[3];
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
void Round_invexchange(int input[4])
{
	int t;
	t = input[3];
	input[3] = input[2];
	input[2] = input[1];
	input[1] = input[0];
	input[0] = t;
}
void Enc(int input[4], int output[4], int s[14][n_S])
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
void Dec(int input[4], int output[4], int s[14][n_S])
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
int judge_connected_pair(int input1[4], int input2[4], int s[14][n_S])
{
	int output1[4], output2[4];
	int temp1[4], temp2[4];
	
	for (int i = 0; i <4; i++)
	{
		temp1[i] = input1[i];
		temp2[i] = input2[i];
	}
	
	for (int i = 1; i < 4; i++)
	{
		Round_enc(temp1, s, output1, i);
		Round_exchange(output1);
		
		Round_enc(temp2, s, output2, i);
		Round_exchange(output2);
		
		for (int j = 0; j < 4; j++)
		{
			temp1[j] = output1[j];
			temp2[j] = output2[j];
		}
	}
	
	if (output1[0] == output2[0])
	{
		Round_enc(temp1, s, output1, 4);
		Round_exchange(output1);
		
		Round_enc(temp2, s, output2, 4);
		Round_exchange(output2);
		
		if (output1[0] == output2[0])
			return 1;
		else 
			return 0;
	}
	else
		return 0;
}
void f_operation(int input1[4], int input2[4], int y)
{
	int t;
	t = input1[0]; 
	input1[0] = input2[0];
	input2[0] = t;
	
	t = input1[1];
	input1[1] = input2[1];
	input2[1] = t;
	
	input1[1] = input1[1] ^ y;
	input2[1] = input2[1] ^ y;
}
void g_operation(int input1[4], int input2[4], int y)
{
	int t;
	t = input1[2];
	input1[2] = input2[2];
	input2[2] = t;
	
	t = input1[3];
	input1[3] = input2[3];
	input2[3] = t;
	
	input1[3] = input1[3] ^ y;
	input2[3] = input2[3] ^ y;
}
void Mark_connected_pair(int input1[4], int input2[4], int s[14][n_S], int y)
{
	int temp1[4], temp2[4];
	int output1[4], output2[4];
	
	for (int i = 0; i < 4; i++)
	{
		temp1[i] = input1[i];
		temp2[i] = input2[i];
	}
	
	for (int i = 1; i <= 2; i++)
	{   
		Round_enc(temp1, s, output1, i);
		Round_exchange(output1);
		for (int j = 0; j < 4; j++)
			temp1[j] = output1[j];

		Round_enc(temp2, s, output2, i);
		Round_exchange(output2);
		for (int j = 0; j < 4; j++)
			temp2[j] = output2[j];
	}
	
	Round_enc(temp1, s, output1, 3);
	for (int j = 0; j < 4; j++)
		temp1[j] = output1[j];

	Round_enc(temp2, s, output2, 3);
	for (int j = 0; j < 4; j++)
		temp2[j] = output2[j];

	for(int j=0;j<300000;j++)
	{
		if(c1[j][0]==(-1))
			{
				for(int k=0;k<4;k++)
					c1[j][k]=output1[k];
			
				c1[j][4]=output2[0];
				
				j++;
				
				for(int k=0;k<4;k++)
					c1[j][k]=output2[k];
			
				c1[j][4]=output1[0];
			}
	}
}
void Get_connected_pair(int input1[4], int input2[4], int s[14][n_S],int y)
{
	int output1[4];
	int output2[4];

    output1[0] = input1[3] ^ s[5][input1[2]];
	output1[1] = input1[0];
	output1[2] = input1[1] ^ s[4][input1[0]];
	output1[3] = input1[2];

	for (int i = 0; i < 4; i++)
		input1[i] = output1[i];

	output1[1] = input1[0];
	output1[0] = input1[3] ^ s[3][input1[2]];
	output1[2] = input1[1] ^ s[2][input1[0]];
	output1[3] = input1[2];

	for (int i = 0; i < 4; i++)
		input1[i] = output1[i];

	output1[0] = input1[0];
	output1[1] = input1[1] ^ s[0][input1[0]];
	output1[2] = input1[2];
	output1[3] = input1[3] ^ s[1][input1[2]];

    output2[1] = input2[0];
	output2[0] = input2[3] ^ s[5][input2[2]];
	output2[2] = input2[1] ^ s[4][input2[0]];
	output2[3] = input2[2];

	for (int i = 0; i < 4; i++)
		input2[i] = output2[i];

	output2[1] = input2[0];
	output2[0] = input2[3] ^ s[3][input2[2]];
	output2[2] = input2[1] ^ s[2][input2[0]];
	output2[3] = input2[2];

	for (int i = 0; i < 4; i++)
		input2[i] = output2[i];

	output2[0] = input2[0];
	output2[1] = input2[1] ^ s[0][input2[0]];
	output2[2] = input2[2];
	output2[3] = input2[3] ^ s[1][input2[2]];

	for (int i = 0; i < 4; i++)
	{
		input1[i] = output1[i];
		input2[i] = output2[i];
	}
}
int type2_find(int s[14][n_S])
{
	int input1[4] = {0}, input2[4] = { 0 };
	int output1[4], output2[4];
	int temp1[4], temp2[4];
	int time=0;
	float L=0;
	int y = rand() % (n_S - 1) + 1;
	
	for (int k1 = 0; k1 < 300000; k1++) 
	{
		for(int k2=0;k2<5;k2++)
		{
			c1[k1][k2] = -1;
		}
	}
						
	for (int k1 = 0; k1 < 300000; k1++)
	{
		L1:;
		time++;
				
		input1[0] = rand()%n_S;
		input1[1] = rand()%n_S;
		input1[2] = rand()%n_S;
		input1[3] = rand()%n_S;
		input2[0] = rand()%n_S;
		input2[1] = input1[1];
		input2[2] = input1[2];
		input2[3] = input1[3] ^ y; 
				
		for(int t=0;t<30000;t++)
		{
			if(c1[t][0]!=(-1))
			{
				if(input1[0]==c1[t][0])
					if(input1[1]==c1[t][1])
						if(input1[2]==c1[t][2])
							if(input1[3]==c1[t][3])
								if(input2[0]==c1[t][4])
									goto L1;
									
				if(input2[0]==c1[t][0])
					if(input2[1]==c1[t][1])
						if(input2[2]==c1[t][2])
							if(input2[3]==c1[t][3])
								if(input1[0]==c1[t][4])
									goto L1;		
			}
			else 
				break;
		}
		
		Get_connected_pair(input1, input2, s, y);  
	
		for (int i = 0; i < 4; i++) 
		{
			temp1[i] = input1[i];
			temp2[i] = input2[i];
		}
								
		L = 0; 
				 
		do
		{
			f_operation(temp1, temp2, y);

			Enc(temp1, output1, s);
			Enc(temp2, output2, s);

			g_operation(output1, output2, y);

			Dec(temp1, output1, s);
			Dec(temp2, output2, s);
	
			L = L + 1;
					
		}
		while ((input1[0] != temp2[0] || input1[1] != temp2[1] || input1[2] != temp2[2] || input1[3] != temp2[3] || input2[0] != temp1[0] || input2[1] != temp1[1] || input2[2] != temp1[2] || input2[3] != temp1[3])&&(input1[0] != temp1[0] || input1[1] != temp1[1] || input1[2] != temp1[2] || input1[3] != temp1[3] || input2[0] != temp2[0] || input2[1] != temp2[1] || input2[2] != temp2[2] || input2[3] != temp2[3]));
		
		Mark_connected_pair(input1, input2, s,y);

		for(int i=1;i<=20;i++)
		{
			if(time==i) 
				F[i]=L;
			if(time==20)
			    return 0;
		}			  	
	}
	return 0;
}
void get_E_F()
{
	float Vp=1 << (5 * n_bit-1);
	float N_1=1 << (4*n_bit);
	float sum=0;
	
	E_F[1] = 4 * (Vp/N_1);
	
	for (int i = 2; i <=20; i++)
	{
		for (int j = 1; j < i; j++)
			sum = sum + E_F[j]; 
				
		E_F[i] = 4 * ((Vp-sum)/(N_1-2*(i-1)));
		
		sum = 0;
	}
}
int main()
{
	int s[14][n_S];
	float ave_length1[20]={0}, ave_length2[20] = {0};
	
	for (int i = 0; i < 100; i++)
	{
		if((i%1000)==0) printf("Experimental progress: %3d percent.\n",i/1000+1);
		
		for (int j = 0; j < NUM; j++)
		{
			rand_of_n(s[j], n_S);
			Sleep(3);
		}

		type2_find(s);
		
		for(int j = 1; j <=20;j++)
		{
			ave_length1[j] = ave_length1[j] +F[j]; 
			
			F[j]=0;
			
			if (ave_length1[j] > 10000000)
			{
				ave_length1[j] = ave_length1[j] - 10000000;
				ave_length2[j]++;
			}	
		}
	}
	
	get_E_F();
	

	printf("\nConditions: 7-round Type-2 GFN,n=%d.\n Result: \n", n_bit); 
	
	for(int j=1;j<=15;j++)
		printf("E(|F_{x_%d}|)=%f,\n <|F_{x_%d}|>=%f.\n\n",  j, E_F[j], j, (ave_length2[j] / 100) * 10000000 + (ave_length1[j] / 100)); 
	
	getchar();	
	
	return 0;

}






