#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "fea.h"
#include "time.h"
#include "math.h"


typedef struct Particle
{
	int size;
	double fitness;
	double best_fitness;
	double position[L+1];
	double velocity[L+1];
	double best_position[L+1];
}Particle;

 
typedef struct SubPopulation
{
	Particle swarm[NUM_PARTICLE];
	double best_global_position[L+1];
	double best_global_fitness;
}SubPopulation;
SubPopulation sub_populations[SUBPOP_LEN];


// Griewank 函数
double object_function_1(double x[])
{
	double part1 = 0, part2 = 1;
	int i;
	for (i = 0; i < NUM; i++)
	{
		part1 += (x[i] * x[i]);
		part2 *= cos(x[i]/sqrt(i+1));
	}
	part1 /= 1/4000.0;
	return (1 + part1 - part2);
}


// Ackleys Function
double object_function(double x[])
{
	double first_part = 0, second_part = 0;
	int i;
	for (i = 0; i < NUM; i++)
	{
		first_part += x[i] * x[i];
		second_part += cos(2 * PI * x[i]);
	}
	return -20 * exp(-0.2 * sqrt(first_part / NUM)) - exp(second_part / NUM) + 20 + 2.718281;
}


/************************************************************************
* Function Name : make_array_random
* Create Date : 2016/11/04
* Author/Corporation : bigzhao
**
Description : 将一个长度为n的数组打乱顺序
*
* Param : array: 需要被打乱的数组
* length 数组的长度
**
************************************************************************/
void make_array_random(int array[], int length)
{
	int index, tmp, i;
	srand(time(NULL));
	for (i = 0; i <length; i++)
	{
		index = rand() % (length - i) + i;
		if (index != i)
		{
			tmp = array[i];
			array[i] = array[index];
			array[index] = tmp;
		}
	}
}


/************************************************************************
* Function Name : judge_in_array
* Create Date : 2016/11/04
* Author/Corporation : bigzhao
**
Description : 判断元素是否在数组里面，在则返回1 ，不在返回0
利用array是有序的数组这个特性，使用二分查找
*
* Param : element: 被判断的元素
* array: 被判断的数组
* length： 数组的长度
**
************************************************************************/
int judge_in_array(int element, int array[], int length)
{

	int low, high, mid;

	low = 0;
	high = length - 1;

	while (low <= high)
	{
		mid = (low + high) / 2;
		if (array[mid] == element) 
		{
			return 1;
		}
		else if (array[mid] < element) 
		{
			low = mid + 1;
		}	
		else 
		{
			high = mid - 1;
		}
	}

	return 0; 
}


/************************************************************************
* Function Name : rebulid
* Create Date : 2016/11/04
* Author/Corporation : bigzhao
**
Description : 重建维度为NUM的解，用途是亚种群也能计算出适应度，亚种群没有的维度去全局解中拿
*
* Param : index: 亚种群对应全局的下标数组
* index_length: 下标数组的长度
* sub_solution 亚种群的部分解
* global_solution： NUM维度的全局解
* rebulid_solution: 局部解和全局解的结合
**
************************************************************************/
void rebulid(int index[], int index_length, double sub_solution[], double global_solution[], double rebulid_solution[])
{
	int i, j = 0;
	// printf("%d\n", index_length);

	for (i = 0; i < index_length; i++)
	{

		rebulid_solution[index[i]] = sub_solution[j++];

	}

}


/************************************************************************
* Function Name : initialize_subpops
* Create Date : 2016/11/04
* Author/Corporation : bigzhao
**
Description : 亚种群初始化，这里用的是CS-L的分解结构，即{X1,X2,...,XL+1},{X2,...,XL+2}
*
* Param : sub_index: 一个SUBPOP_LEN × （L + 1）的二维数组，用来存放每个亚种群对应的下标
* sub_populations: 结构体数组，存放输出的亚种群
* global_solution： NUM维度的全局解，用来计算适应度
**
************************************************************************/
void initialize_subpops(int sub_index[SUBPOP_LEN][L + 1], SubPopulation sub_populations[], double global_solution[])
{
	int i, j, k;

	double best_global_fitness;
	double random_position[L + 1];
	double random_velocity[L + 1];
	double fitness;
	double rebulid_solution[NUM];

	for (i = 0; i < SUBPOP_LEN; i++)
	{
		printf("the %dth subgroup: ", i+1);
		for (j = 0; j < L + 1; j++)
		{
			sub_index[i][j] = i + j;
			printf("%d ", sub_index[i][j]);
		}
		printf("\n");
	}

	srand((unsigned)time(NULL));
	// printf("in\n");
	for (i = 0; i < SUBPOP_LEN; i++)
	{
		best_global_fitness = 1.0e308;
		// initialize paticle
		for (j = 0;j < NUM_PARTICLE; j++)
		{
			for (k = 0; k < L + 1; k++)
			{
				random_position[k] = (MAX_X - MIN_X) * rand() / (double)RAND_MAX  + MIN_X;
				random_velocity[k] = (MAX_V - MIN_V) * rand() / (double)RAND_MAX  + MIN_V;
			}
			memcpy(rebulid_solution, global_solution, sizeof(double) *NUM);
			rebulid(sub_index[i], L+1, random_position, global_solution, rebulid_solution);
			fitness = object_function(rebulid_solution);
			// struct Particle p = {L + 1,fitness, fitness, *random_position, *random_velocity, *random_position};
			
			sub_populations[i].swarm[j].size = L + 1;
			sub_populations[i].swarm[j].fitness = fitness;
			sub_populations[i].swarm[j].best_fitness = fitness;
			memcpy(sub_populations[i].swarm[j].position, random_position, sizeof(double)*(L+1));
			memcpy(sub_populations[i].swarm[j].best_position, random_position, sizeof(double)*(L+1));
			memcpy(sub_populations[i].swarm[j].velocity, random_velocity, sizeof(double)*(L+1));

			if (sub_populations[i].swarm[j].fitness < best_global_fitness)
			{
				best_global_fitness = sub_populations[i].swarm[j].fitness;
				for (k = 0; k < L+1; k++)
				{
					sub_populations[i].best_global_position[k] = sub_populations[i].swarm[j].position[k];
				}

			}
			sub_populations[i].best_global_fitness = best_global_fitness;
		}
	}
}


/************************************************************************
* Function Name : pso
* Create Date : 2016/11/04
* Author/Corporation : bigzhao
**
Description : 亚种群的pso优化算法，这里默认是CS-L结构
*
* Param : index: 亚种群对应全局的下标数组
* sub_population: 亚种群的指针，指向该次优化的亚种群
* global_solution： NUM维度的全局解，用来计算适应度
**
************************************************************************/
void pso(int index[], int index_length, SubPopulation *sub_population, double global_solution[])
{
	int i, j, k, l;
	double r1, r2;
	double new_velocity;
	double new_position;
    double new_fitness;
    double rebulid_solution[NUM];

	srand((unsigned)time(NULL));

	for (i = 0; i < NUM_ITERATION; i++)
	{
		for (j = 0; j < NUM_PARTICLE; j++)
		{
			for (k = 0; k < index_length; k++)
			{
				r1 = rand() / (double)RAND_MAX;
				r2 = rand() / (double)RAND_MAX;
				new_velocity = (W * (*sub_population).swarm[j].velocity[k]) + 
				               (C1 * r1 * ((*sub_population).swarm[j].best_position[k] - (*sub_population).swarm[j].position[k])) + 
				               (C2 * r2 * ((*sub_population).best_global_position[k] - (*sub_population).swarm[j].position[k]));

				if (new_velocity < MIN_V)
				{
					new_velocity = MIN_V;
				}
				else if (MAX_V < new_velocity)
				{
					new_velocity = MAX_V;
				}

				(*sub_population).swarm[j].velocity[k] = new_velocity;
				new_position = (*sub_population).swarm[j].position[k] + new_velocity;

				if (new_position < MIN_X)
				{
					new_position = MIN_X;
				}
				else if (MAX_X < new_position)
				{
					new_position = MAX_X;
				}
				(*sub_population).swarm[j].position[k] = new_position;
			}

			memcpy(rebulid_solution, global_solution, sizeof(double)*NUM);

			rebulid(index, index_length, (*sub_population).swarm[j].position, global_solution, rebulid_solution);
			new_fitness = object_function(rebulid_solution);

			if (new_fitness < (*sub_population).swarm[j].best_fitness)
			{
				(*sub_population).swarm[j].best_fitness = new_fitness;
				for (k = 0; k < index_length; k++)
				{
					(*sub_population).swarm[j].best_position[k] = (*sub_population).swarm[j].position[k];
				}
			}

			if (new_fitness < (*sub_population).best_global_fitness)                
			{
				(*sub_population).best_global_fitness = new_fitness;
				for (int k = 0; k < index_length; k++)
				{
					(*sub_population).best_global_position[k] = (*sub_population).swarm[j].position[k];
				}
			}
		}
	}

}


/************************************************************************
* Function Name : competition
* Create Date : 2016/11/04
* Author/Corporation : bigzhao
**
Description : 重叠种群之间的竞争函数，随机抽取每个维度，找出重叠的亚种群，比较适应度得出最好的组成全局解
*
* Param : sub_index: 一个SUBPOP_LEN × （L + 1）的二维数组，用来存放每个亚种群对应的下标
* global_solution： NUM维度的全局解，用来计算适应度
**
************************************************************************/
void competition(int sub_index[SUBPOP_LEN][L + 1], double global_solution[])
{
	int i, j, random_array[NUM];
	double fitness, best_fitness;
	double best_solution[NUM];
    memcpy(best_solution, global_solution, sizeof(double) * NUM);
    
    for(i = 0; i < NUM; i++)
    {
    	random_array[i] = i;
    }
	make_array_random(random_array, NUM);

	for (i = 0; i < NUM; i++)
    {
    	random_array[i] = i;
    }
	for (i = 0; i < NUM; ++i)
	{ 
		best_fitness = object_function(global_solution);

		for (j = 0; j < SUBPOP_LEN; j++)
		{
			// 找出重叠的亚种群
			if (judge_in_array(random_array[i], sub_index[j], L + 1))
			{
				global_solution[random_array[i]] = sub_populations[sub_index[j][0]].best_global_position[random_array[i] - sub_index[j][0]];
				fitness = object_function(global_solution);

				// 找出当前维度最好的值组成全局解
				if (fitness < best_fitness)
				{
					best_solution[random_array[i]] = global_solution[random_array[i]];
					best_fitness = fitness;
				}
			}
		}
	}
	memcpy(global_solution, best_solution, sizeof(double) * NUM);
}


/************************************************************************
* Function Name : sharing
* Create Date : 2016/11/04
* Author/Corporation : bigzhao
**
Description : 亚种群之间的分享，经过竞争步骤之后此时全局解为各个亚种群的最优组成，利用全局解反向影响局部。
找出当前亚种群最差的解用全局解去替换。
*
* Param : sub_index: 一个SUBPOP_LEN × （L + 1）的二维数组，用来存放每个亚种群对应的下标
* global_solution： NUM维度的全局解，用来计算适应度
**
************************************************************************/
void sharing(int sub_indexp[SUBPOP_LEN][L + 1], double global_solution[])
{
	int i, j, worst;

	for (i = 0; i < SUBPOP_LEN; i++)
	{
		worst = 0;

		for (j = 1; j < NUM_PARTICLE; j++)
		{
			if (sub_populations[i].swarm[worst].fitness < sub_populations[i].swarm[j].fitness)
			{
				worst = j;
			}
		}

		for (j = 0; j < L + 1; j++)
		{
			sub_populations[i].swarm[worst].position[j] = global_solution[sub_indexp[i][j]];
		}
	}
}


/************************************************************************
* Function Name : sharing
* Create Date : 2016/11/04
* Author/Corporation : bigzhao
**
Description : FEA算法，先初始化亚种群，然后进入循环进行亚种群的优化、种群间的竞争和分享。
终止条件：连续15代无更好结果出现跳出循环，打印结果。
**
************************************************************************/
void fea()
{
	int sub_index[SUBPOP_LEN][L + 1], i;
	int record = 0;
	double global_solution[NUM], last_fitness;
	srand((unsigned)time(NULL));
	// 随机生成NUM维全局解
	for (i = 0; i < NUM; i++)
	{
		global_solution[i] = (double)(MAX_X - MIN_X) * rand() / (double)RAND_MAX +  MIN_X;
	}

    initialize_subpops(sub_index, sub_populations, global_solution);
    last_fitness = object_function(global_solution);

    while(1)
    {
    	for (i = 0; i < SUBPOP_LEN; i++)
    	{
    		pso(sub_index[i], L+1, &sub_populations[i], global_solution);
    	}
    	competition(sub_index, global_solution);
    	sharing(sub_index, global_solution);
    	if (last_fitness > object_function(global_solution))
    	{
    		record = 0;
    		last_fitness = object_function(global_solution);
    	}
    	else
    	{
    		record += 1;
    	}
    	// 连续15代无更好结果
    	if (15 <= record)
    		break;
    }

    // 打印结果
    printf("the fea run over.\nThe best fitness is %lf \n .The solution is:", last_fitness);
    for (i = 0; i < NUM; i++)
    {
    	printf(" %lf ", global_solution[i]);
    }
    printf("\n");
    pso(sub_index[0], L+1, &sub_populations[0], global_solution);
    printf("out\n");
    printf("the fitst subpop's best global fieness is %lf\n", sub_populations[0].best_global_fitness);
}



int main(int argc, char const *argv[])
{
	fea();
	return 0;
}