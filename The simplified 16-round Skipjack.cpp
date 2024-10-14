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

#define NUM 16                //The number of round functions of the simplified 16-round Skipjack.
#define n_bit 5              //The size of round functions of the simplified 16-round Skipjack.
#define n_S (1<<n_bit)        //The size of the domain of the round functions.
int c1[1 << (5 * n_bit)];     //An array used to label 1<<(5* n_bit) connected pairs in Vp.
float F[16]={0};              //An array to be written to store the cardinalities of families.
 
/*************************************Note: The readers can modify the value of n_bit to reproduce the experiments with n_bit=4 and n_bit=5.*****************************************/

using namespace std;
struct element {     
	int data;  
	int index;  
};
int cmp(const void *a, const void *b) {  
	return((struct element*)a)->data - ((struct element*)b)->data;
}
void rand_of_n(int a[], int n)  // After applying this function,a[0],a[1],... a[n-1] is a random permutation of 0,1,... n-1.
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
void Round_enc_A(int input[4], int s[16][n_S], int output[4], int flag) //We divide the process of input[2] to output[2] after a round of feistel structure encryption into two parts, namely Round_enc and Round_exchange.
{	
    //This function denotes the partial 'Rule-A' operation.
	//s[16][n_S] denotes the round functions of 16-round Skipjack. 
	//flag indicates the position of the current round. For example, if flag=0, this function denotes the first round of 16-round Skipjack. 
	output[0] = s[flag][input[0]];
	output[1] = input[1];
	output[2] = input[2];
	output[3] = s[flag][input[0]] ^ input[3];
}
void Round_enc_B(int input[4], int s[16][n_S], int output[4], int flag)
{
	//This function denotes the partial 'Rule-B' operation.
	//s[16][n_S] denotes the round functions of 16-round Skipjack. 
	//flag indicates the position of the current round. For example, if flag=0, this function denotes the ninth round of 16-round Skipjack. 
	output[0] = s[flag+8][input[0]];
	output[1] = input[0]^ input[1];
	output[2] = input[2];
	output[3] = input[3];
}
void Round_exchange(int output[4]) // Round_enc_A and Round_exchange together make up the 'rule A' of the simplified 16-round Skipjack;  Round_enc_B and Round_exchange together make up the 'rule B' of the simplified 16-round Skipjack.
{
	int t;
	t = output[0];
	output[0] = output[3];
	output[3] = output[2];
	output[2] = output[1];
	output[1] = t;
}
void Round_invexchange(int input[4])//Round_dec and Round_invexchange are the inverse of Round_enc and Round_exchange, respectively.
{
	int t;
	t = input[1];
	input[1] = input[2];
	input[2] = input[3];
	input[3] = input[0];
	input[0] = t;
}
void Round_dec_A(int input[4], int inv_s[16][n_S], int output[4], int flag)
{
	input[0] = inv_s[flag][output[0]];
	input[1] = output[1];
	input[2] = output[2];
	input[3] = output[3] ^ output[0];
}
void Round_dec_B(int input[4], int inv_s[16][n_S], int output[4], int flag)
{
	input[0] = inv_s[flag+8][output[0]];
	input[1] = output[1]^ input[0];
	input[2] = output[2];
	input[3] = output[3];
}
void Enc(int input[4], int output[4], int s[16][n_S])// This function represents the encryption operation of 5-round Feistel. Note that in the fifth round we omit Round_exchange.
{	
	for (int i = 0; i < 8; i++)
	{
		Round_enc_A(input, s, output, i);
		Round_exchange(output);
		for (int j = 0; j < 4; j++)
			input[j] = output[j];
	}
	for (int i = 0; i < 8; i++)
	{
		Round_enc_B(input, s, output, i);
		Round_exchange(output);
		for (int j = 0; j < 4; j++)
			input[j] = output[j];
	}
}
void Dec(int input[4], int output[4], int inv_s[16][n_S])
{
	for (int i = 7; i >=0 ; i--)
	{   
		Round_invexchange(output);
		Round_dec_B(input, inv_s, output, i);
		for (int j = 0; j < 4; j++)
			output[j] = input[j];
	}
	for (int i = 7; i >= 0; i--)
	{
		Round_invexchange(output);
		Round_dec_A(input, inv_s, output, i);
		for (int j = 0; j < 4; j++)
			output[j] = input[j];
	}
}
void f_operation(int input1[4], int input2[4], int s[16][n_S], int y) // This function denotes the P-transition function f.
{
	int t;
	t = input1[1]; 
	input1[1] = input2[1]; 
	input2[1] = t;
}
void g_operation(int output1[4], int output2[4], int s[16][n_S], int y)// This function denotes the C-transition function g.
{
	int t;
	t = output1[0]; 
	output1[0] = output2[0]; 
	output2[0] = t;
}
int judge_connected_pair(int input1[4], int input2[4], int s[16][n_S], int y) //This function determines whether plaintexts input1[2] and input2[2] are connected. For more details, 
{
	int temp1[4], temp2[4]; 
	int output1[4], output2[4];
	
	for (int i = 0; i < 4; i++)
	{
		temp1[i] = input1[i];
		temp2[i] = input2[i];
	}
	
	for (int i = 0; i < 4; i++)
	{
		Round_enc_A(temp1, s, output1, i);
		Round_enc_A(temp2, s, output2, i);
		Round_exchange(output1);
		Round_exchange(output2);
		for (int j = 0; j < 4; j++)
		{
			temp1[j] = output1[j];
			temp2[j] = output2[j];
		}
	}

	if ((temp1[0]==temp2[0])&&(temp1[1]!=temp2[1])&&(temp1[2]==(temp2[2]))&&(temp1[3] == temp2[3]))
		return 1; // if the plaintexts 'input1[2]' and 'input2[2]' are connected, then return 1; Otherwise, return 0;
	else
		return 0;
}
void Partial_Dec(int output1[4], int output2[4], int inv_s[16][n_S]) // This function is part of the decryption process and is useful in generating concatenated plaintext pairs. In fact, it is difficult to find a connected plaintext pair directly. Therefore, we will decrypt the connected plaintext pair from the special difference at the third round.
{
	int input1[4], input2[4];
	
	for (int i = 3; i >= 0; i--)
	{
		Round_invexchange(output1);
		Round_invexchange(output2);
		Round_dec_A(input1, inv_s, output1, i);
		Round_dec_A(input2, inv_s, output2, i);
		
		for (int j = 0; j < 4; j++)
			output1[j] = input1[j];
		for (int j = 0; j < 4; j++)
			output2[j] = input2[j];
	}
}
void Mark_connected_pair(int input1[4], int input2[4], int s[16][n_S], int y)//This function is used to mark 1<<(2* n_bit) connected pairs in Vp.
{	
	//Note that the array c1  has 1<<(2* n_bit) items, which represent exactly the 1<<(2* n_bit) connected pairs in Vp. If a connected pair is already obtained, then we write the corresponding position of C1 as 1.
	
	int temp1[4], temp2[4]; 
	int output1[4], output2[4];
	
	{
		for (int i = 0; i < 4; i++)
		{
			temp1[i] = input1[i];
			temp2[i] = input2[i];
		}
		for (int i = 0; i < 4; i++)
		{
			Round_enc_A(temp1, s, output1, i);
			Round_exchange(output1);
			for (int j = 0; j < 4; j++)
				temp1[j] = output1[j];
				
			Round_enc_A(temp2, s, output2, i);
			Round_exchange(output2);
			for (int j = 0; j < 4; j++)
				temp2[j] = output2[j];
		}
		c1[output2[1] ^ (output1[3]  << (1 * n_bit))  ^  (output1[2] << (2*n_bit))^  (output1[1] << (3*n_bit))^(output1[0] <<(4 *n_bit))]+= 1;
		c1[output1[1] ^ (output2[3]  << (1 * n_bit) ) ^  (output2[2] << (2*n_bit)) ^ (output2[1] << (3*n_bit))^(output2[0] << (4*n_bit))]+= 1;
	}
	

	{	
		for (int i = 0; i < 4; i++)
		{
			temp1[i] = input1[i];
			temp2[i] = input2[i];
		}
		
		f_operation(temp1, temp2,  s, y);
		
		for (int i = 0; i < 4; i++)
		{
			Round_enc_A(temp1, s, output1, i);
			Round_exchange(output1);
			for (int j = 0; j < 4; j++)
				temp1[j] = output1[j];
				
			Round_enc_A(temp2, s, output2, i);
			Round_exchange(output2);
			for (int j = 0; j < 4; j++)
				temp2[j] = output2[j];
		}
		c1[output2[1] ^ (output1[3]  << (1 * n_bit))  ^  (output1[2] << (2*n_bit))^  (output1[1] << (3*n_bit))^(output1[0] <<(4 *n_bit))]       += 1;
		c1[output1[1] ^ (output2[3]  << (1 * n_bit) ) ^  (output2[2] << (2*n_bit)) ^ (output2[1] << (3*n_bit))^(output2[0] << (4*n_bit))]       += 1;
	
	}
}
void Skipjack_find(int s[16][n_S], int inv_s[16][n_S])//This function is used to count the cardinalities of the families in Vp concerning 5-round Feistel.
{
	int input1[4], input2[4];
	int temp1[4], temp2[4];
	int output1[4], output2[4];
	float i_min;
	int time = 0;
	int y = rand() % (n_S - 1) + 1;
	
    for (int i = 0; i < (1 << (5 * n_bit)); i++)
	{
		if ((i&((1 << n_bit) - 1)) == ((i&(((1 << (4 * n_bit)) - 1) - ((1 << 3 * n_bit) - 1))) >> 3* n_bit))
			c1[i] = 1;
		else
		    c1[i] = 0;
	}// This is an initialization, which means we haven't got any of the connected pairs in the Vp.
	
	for (int i = 0; i < (1 << (5 * n_bit)); i++)
		if (c1[i] == 0)   // If we have not obtained a connected pair related to c1[i].
		{
			time++; //'time' denotes the serial number of a family. For example, if time=3, the connected pair related to c1[i] is belong to F_{x_3}.


			input1[0] = (i&(((1 << (5 * n_bit)) - 1) - ((1 << 4 * n_bit) - 1))) >> 4 * n_bit;
			input1[1] = (i&(((1 << (4 * n_bit)) - 1) - ((1 << 3 * n_bit) - 1))) >> 3 * n_bit;
			input1[2] = (i&(((1 << (3 * n_bit)) - 1) - ((1 << 2 * n_bit) - 1))) >> 2 * n_bit;
			input1[3] = (i&(((1 << (2 * n_bit)) - 1) - ((1 << 1 * n_bit) - 1))) >> 1 * n_bit;

			input2[0] = input1[0];
			input2[1] = i&((1 << n_bit) - 1);
			input2[2] = input1[2];
			input2[3] = input1[3];
				
			Partial_Dec(input1, input2, inv_s);  // Here, 'input1[2]' and 'input[2]' are a pair of connected pair related to c[i].
		
			for (int j = 0; j < 4; j++)  
			{
				temp1[j] = input1[j];
				temp2[j] = input2[j];
			}
			
			i_min = 0;    // 'i_min' denotes to the smallest positive integer such that [P_0]=Y^{i_min}([P_0]) or [P_0]^T=Y^{i_min}([P_0]). See Lemma 1 for more details.

			do
			{
			    Mark_connected_pair(input1, input2, s, y); // Each newly obtained connected pair is marked in c1.
			    
				f_operation(input1, input2, s, y);

				Enc(input1, output1, s);
				Enc(input2, output2, s);

			    g_operation(output1, output2, s, y);

				Dec(input1, output1, inv_s);
				Dec(input2, output2, inv_s);
				
				// Actually, this is one 'Y-operation' of yoyo game.

				i_min++;

			} while (((input1[0] != temp1[0]) || (input1[1] != temp1[1] )|| (input1[2] != temp1[2] )|| (input1[3] != temp1[3]) ||(input2[0] != temp2[0])  || (input2[1] != temp2[1]) || (input2[2] != temp2[2] )||( input2[3] != temp2[3]))&&((input1[0] != temp2[0]) || (input1[1] != temp2[1]) || (input1[2] != temp2[2]) || (input1[3] != temp2[3]) ||(input2[0] != temp1[0]) ||  (input2[1] != temp1[1]) || (input2[2] != temp1[2] ) || (input2[3] != temp1[3])));
		
			for(int j=1;j<=15;j++) // We only consider the cases of F_{x_1}, F_{x_2},...,F_{x_{15}}.
			{
				if(time==j) F[j]=i_min*2; //Since the transition functions of 5-round Feistel satisfy the property I-\overline{C}-\overline{S}, the cardinality of F_{x_i} is i_min*4(See Theorem 3 for more details).
			}	
		}
			
}
int main()
{
	int s[16][n_S], inv_s[16][n_S];
	float sum_c1[31] = {0}, sum_c2[31] = {0}; // 'sum_c1[i]' and 'sum_c2[i]' are used to count the sum of cardinalities |F_{x^1_j}|, |F_{x^2_j}|,...,|F_{x^{100000}_j}|.
	float t = 1;

	for (int i = 0; i < 100000; i++)
	
	{
		if((i%1000)==0) printf("Experimental progress: %3d percent.\n",i/1000+1);

		
		for (int j = 0; j < NUM; j++)
		{

			rand_of_n(s[j], n_S);
			Sleep(3);
		}
		for (int j = 0; j < NUM; j++)
		{
			for (int j1 = 0; j1 < (1 << n_bit); j1++)
				inv_s[j][s[j][j1]] = j1;
		}
		
			
		
		
		Skipjack_find(s,inv_s);
		
		
		
		for(int j=1;j<=15;j++)
		{
			sum_c1[j] = sum_c1[j] +F[j]; 
			F[j]=0;
			if (sum_c1[j] > 10000000)
			{
				sum_c1[j] = sum_c1[j] - 10000000;
				sum_c2[j]++;
			}	// Considering that sometimes |F_{x^1_j}|+|F_{x^2_j}|+...+|F_{x^{100000}_j}| is too large to make the program error, we divide it into two parts as sum_c1 and sum_c2.
		}
	}

	printf("\nConditions: 16-round Skipjack,n=%d.\n Result: \n", n_bit); 
	
	for(int j=1;j<=15;j++)
	{
		t = t * 3;
		printf("E(|F_{x_%d}|)=%f,\n <|F_{x_%d}|>=%f.\n\n",  j, ((1 << (4 * n_bit-1))*((1<<(n_bit))-1)) / (t / 2), j, (sum_c2[j] / 100000) * 10000000 + (sum_c1[j] / 100000)); 
	}
	// 'E(|F_{x_i}|)' denotes the mathematical expectation of |F_{x_i}|(See Theorem 3 for why 'E(|F_{x_%d}|)' is this result).
	// '<|F_{x_%d}|>' denotes the average of observed values |F_{x^1_j}|+|F_{x^2_j}|+...+|F_{x^{100000}_j}|.
	getchar();
	return 0;
}
