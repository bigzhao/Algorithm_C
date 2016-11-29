/************************************************************************
* Author Name : bigzhao
* Email       : tandazhao@email.szu.edu.cn
* Filename    : EDAs.cpp
* Description : Using EDAs to optimize the multimodel function.
************************************************************************/
#include "EDAs.h"

double shift_array[Dim];             // 偏移向量


// 偏移的Griewank 函数
double object_function(double *x)
{
	double part1 = 0, part2 = 1;
	int i;
	for (i = 0; i < Dim; i++)
	{
		part1 += ((x[i] - shift_array[i]) * (x[i] - shift_array[i]));
		part2 *= cos((x[i] - shift_array[i]) / sqrt(i + 1));
	}
	part1 /= 1 / 4000.0;
	return (1 + part1 - part2);
}

//// 偏移的Griewank 函数
//double object_function(double *x)
//{
//	double part1 = 0, part2 = 1;
//	int i;
//	for (i = 0; i < Dim; i++)
//	{
//		part1 += ((x[i]) * (x[i]));
//		part2 *= cos((x[i]) / sqrt(i + 1));
//	}
//	part1 /= 1 / 4000.0;
//	return (1 + part1 - part2);
//}

struct Individual
{
	double position[Dim];
	double fitness;
};


/************************************************************************
* Function Name : initialize_population
* Create Date : 2016/11/26
* Author/Corporation : bigzhao
**
Description : 初始化种群函数，此处的u01使用了boots库的uniform_01，函数声明
在EDAs.h里
*
* Param : num: 种群的大小
* pop: 指针，指向种群
**
************************************************************************/
void initialize_population(int num, struct Individual *pop)
{
	int i, j;

	for (i = 0; i < num; i++)
	{
		for (j = 0; j < Dim; j++)
			pop[i].position[j] = u01() * (maxX - minX) + minX;  //u01是boots的uniform_01函数
		pop[i].fitness = object_function(pop[i].position);
	}

}


/************************************************************************
* Function Name : comp
* Create Date : 2016/11/26
* Author/Corporation : bigzhao
**
Description : 结构体 Individual 之间比较大小的依据函数，供std::qsort使用
*
* Param : a: 指向第一个个体
* b: 指向另一个个体
**
************************************************************************/
int comp(const void*a, const void*b)
{
	if ((*(struct Individual*)a).fitness > (*(struct Individual*)b).fitness)
		return 1;
	else
		return -1;
}


/************************************************************************
* Function Name : get_stddev
* Create Date : 2016/11/26
* Author/Corporation : bigzhao
**
Description : 计算标准差
*
* Param : x: 待计算的数组
* mean: 平均值
* length: 数组长度
**
************************************************************************/
double get_stddev(double *x, double mean, int length)
{
	int    i;
	double sum = 0.0;

	for (i = 0;i < length; i++)
		sum += pow(x[i] - mean, 2);

	return sqrt(sum / length);
}


/************************************************************************
* Function Name : generate_new_population
* Create Date : 2016/11/26
* Author/Corporation : bigzhao
**
Description : EDAs 里面重要的一个环节，通过高斯分布产生下一代种群，里面的高
斯函数使用 boots 库的 normal_distribution，用法是百度查的，日后得好好琢磨一
下
*
* Param : pop: 旧种群
* new_pop: 新种群
**
************************************************************************/
void generate_new_population(struct Individual *pop, struct Individual *new_pop)
{
	int    i, j;
	double mean, stddev, sum = 0;
	double temp_dimension[K];

	for (i = 0; i < Dim; i++)
	{
		sum = 0;

		for (j = 0; j < K; j++)
		{
			temp_dimension[j] = pop[j].position[i];
			sum += temp_dimension[j];
		}

		mean = sum / K;
		stddev = get_stddev(temp_dimension, mean, K);
		boost::normal_distribution<> nd(mean, stddev); // boots 库的高斯分布函数

		for (j = 0; j < NP; j++)
		{
			new_pop[j].position[i] = nd(u01);   // 高斯分布产生新的值，组成个体

			if (new_pop[j].position[i] < minX)
				new_pop[j].position[i] = minX;
			else if (maxX < new_pop[j].position[i])
				new_pop[j].position[i] = maxX;
			//std::cout << mean << "  " << stddev << "  "<<  new_pop[j].position[i] << std::endl;
		}

	}
	// 计算 fitness
	for (i = 0; i < NP; i++)
		new_pop[i].fitness = object_function(new_pop[i].position);
}


/************************************************************************
* Function Name : select
* Create Date : 2016/11/26
* Author/Corporation : bigzhao
**
Description : EDAs 里面的一个环节，结合新旧种群通过优胜略汰选择下一个种群，具
体实现方法是将两个种群排序，然后用归并的方法组成 NP 个解，再 COPY 给POP
*
* Param : pop: 旧种群
* new_pop: 新种群
**
************************************************************************/
void select(struct Individual *pop, struct Individual *new_pop)
{
	int i = 0, j = 0, k = 0;
	struct Individual combine[NP];

	while (k < NP)
	{
		if ((pop + i)->fitness <= (new_pop + j)->fitness)
			combine[k++] = *(pop + i++);
		else
			combine[k++] = *(new_pop + j++);
	}
	memcpy(pop, combine, NP * sizeof(struct Individual));
}


/************************************************************************
* Function Name : find_the_best
* Create Date : 2016/11/26
* Author/Corporation : bigzhao
**
Description : 找到种群里面最好的个体
*
* Param : pop: 种群
* best: 最好个体
**
************************************************************************/
void find_the_best(struct Individual *pop, struct Individual *best)
{
	int i;
	*best = pop[0];

	for (i = 1; i < NP; i++)
		if (pop[i].fitness < best->fitness)
			*best = pop[i];
	//return best_fitness;
}


/************************************************************************
* Function Name : EDAs
* Create Date : 2016/11/26
* Author/Corporation : bigzhao
**
Description : 主算法，流程:
step1: 随机初始化种群
step2: 排序，找出前 K 个最好的个体
step3: 利用 step2 找出的个体生成新的种群
step4: 新旧种群优胜略汰，产生下一代种群
step5: 如果终止条件满足则退出，否则回到step2
*
* Param : pop: 种群
* best: 最好个体
**
************************************************************************/
void EDAs()
{
	int i;
	struct Individual pop[NP], new_pop[NP], best;
	initialize_population(NP, pop);
	std::cout << pop[0].fitness << std::endl;

	for (i = 0; i < ITERATION; i++)
	{
		std::qsort(pop, NP, sizeof(pop[0]), comp);
		generate_new_population(pop, new_pop);
		std::qsort(new_pop, NP, sizeof(struct Individual), comp);
		select(pop, new_pop); 
	}

	find_the_best(pop, &best);
	printf("Iteration Stop, the best fitness is %1.2E\n", best.fitness);

	for (i = 0; i < Dim; i++)
		printf("x%d: %lf\n", i, best.position[i]);
}


void generate_shift_array() 
{
	for (int i = 0; i < Dim; i++)
	{
		shift_array[i] = u01() * (maxX - minX) + minX;
		//std::cout << shift_array[i]<< std::endl;
	}
}


int main()
{
	generate_shift_array(); // 创建偏移向量
	EDAs();
	return 0;
}