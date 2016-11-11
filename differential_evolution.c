/************************************************************************
* Author Name : bigzhao
* Email       : tandazhao@email.szu.edu.cn
* Filename    : differential_evolution.c
* Description : Using simple-DE(Differential Evolution  Algorithm) to optimize the multimodel function.
************************************************************************/
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"
#include "time.h"
#include "differential_evolution.h"


// Griewank 函数
double object_function(double *x)
{
	double part1 = 0, part2 = 1;
	int i;
	for (i = 0; i < DIM; i++)
	{
		part1 += (x[i] * x[i]);
		part2 *= cos(x[i]/sqrt(i+1));
	}
	part1 /= 1/4000.0;
	return (1 + part1 - part2);
}


typedef struct Individual
{
	double x[DIM];
	double fitness;
}Individual;


void initiate_population(Individual *population)
{
	int i, j;
	double x[DIM], fitness;
	srand((unsigned)time(NULL));
	for (i = 0; i < NUM; i++)
	{
		for (j = 0; j < DIM; j++)
		{
			population[i].x[j] = (MAX - MIN) * rand() / (double)RAND_MAX + MIN;
		}
		population[i].fitness = object_function(population[i].x);
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
	// srand(time(NULL));
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


// void generate_different_interger_randomly(int num, int *rand_int)
// {
// 	int i, temp;
// 	for (i = 0;i < num; i++)
// 	{
// 		temp = rand() % NUM;
// 		while (judge_in_array(temp, rand_int, i))
// 			temp = rand() % NUM;
// 		rand_int[i] = temp;
// 	} 
// }

/************************************************************************
* Function Name : mutation
* Create Date : 2016/11/11
* Author/Corporation : bigzhao
**
Description : 突变函数，此处已使用了最简单的随机方法，随机抽取三个个体： new = P1 + F*(P2-P3)
*
* Param : population: 待变异的种群
* mutation_population 突变后的结果
**
************************************************************************/
void mutation(Individual *population, Individual *mutation_population)
{
	int    rand_int[NUM], i, j;
	double new_x;
	for (i = 0; i < NUM; i++)
		rand_int[i] = i;
	for (i = 0; i < NUM; i++)
	{
		// generate_different_interger_randomly(3, rand_int);
		make_array_random(rand_int, NUM);
		for (j = 0; j < DIM; j++)
		{
			new_x = population[rand_int[0]].x[j] + F * (population[rand_int[1]].x[j] 
				- population[rand_int[2]].x[j]);
			if (new_x < MIN)
				new_x = MIN;
			else if (MAX < new_x)
				new_x = MAX;
			mutation_population[i].x[j] = new_x;
		}
		mutation_population[i].fitness = object_function(mutation_population[i].x);
	}

}

/************************************************************************
* Function Name : crossover
* Create Date : 2016/11/11
* Author/Corporation : bigzhao
**
Description : 交叉函数，按照规定的CR概率去交配获得新种群
*
* Param : population  :  待交叉的种群
* mutation_population ： 突变后的结果
* crossover_population： 交叉后的结果
**
************************************************************************/
void crossover(Individual *population, Individual *mutation_population, Individual *crossover_population)
{
	int i, j;
    double r;
	for (i = 0; i < NUM; i++)
	{
		for (j = 0; j < DIM; j++)
		{
			r = rand() / (double) RAND_MAX;
			if (r < CR)
				crossover_population[i].x[j] = mutation_population[i].x[j];
			else
				crossover_population[i].x[j] = population[i].x[j];
		}
		crossover_population[i].fitness = object_function(crossover_population[i].x);
	}
}

/************************************************************************
* Function Name : selecting
* Create Date : 2016/11/11
* Author/Corporation : bigzhao
**
Description : 选择函数，按照适应度fitness优胜略汰
*
* Param : population  :  待交叉的种群
* crossover_population： 交叉后的结果
**
************************************************************************/
void selecting(Individual *population, Individual *crossover_population)
{
	int i;

	for (i = 0; i < NUM; i++)
	{
		if (crossover_population[i].fitness < population[i].fitness)
		{		
			memcpy(population[i].x, crossover_population[i].x, sizeof(double) * DIM);
		    population[i].fitness = crossover_population[i].fitness;
        }
	}
}

/************************************************************************
* Function Name : find_the_best_fitness
* Create Date : 2016/11/11
* Author/Corporation : bigzhao
**
Description : 找到最好的个体
*
* Param : population  :  待交叉的种群
**
************************************************************************/
double find_the_best_fitness(Individual *population)
{
	int i, best_index = 0;

	for (i = 1; i < NUM; i++)
	{
		if (population[i].fitness < population[best_index].fitness)
			best_index = i;
	}
	return population[best_index].fitness;
}


/************************************************************************
* Function Name : differential_evolution_algorithm
* Create Date : 2016/11/11
* Author/Corporation : bigzhao
**
Description : 主算法
**
************************************************************************/
void differential_evolution_algorithm()
{
	int        record = 0;
	Individual population[NUM], mutation_population[NUM], crossover_population[NUM], selected_population[NUM];
	double     temp, last_fitness;

    printf("进入查分进化算法...\n");

    printf("种群初始化...\n");
    initiate_population(population);
    last_fitness = find_the_best_fitness(population);
    printf("成功\n");
	while (1)
	{
		mutation(population, mutation_population);
		crossover(population, mutation_population, crossover_population);
		selecting(population, crossover_population);

		temp = find_the_best_fitness(population);
        if (temp < last_fitness)
        {
        	record = 0;
        	last_fitness = temp;
        }
        else
        {
        	record++;
        }
        if (15 < record)
        	break;
        printf("%1.20E\n", last_fitness);
	}
	printf("查分进化算法连续15代无改进-退出， 最后适应度为%1.20E\n", last_fitness);
}


int main(int argc, char const *argv[])
{
	srand((unsigned)time(NULL));

	differential_evolution_algorithm();
	return 0;
}
