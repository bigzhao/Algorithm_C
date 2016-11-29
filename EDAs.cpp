/************************************************************************
* Author Name : bigzhao
* Email       : tandazhao@email.szu.edu.cn
* Filename    : EDAs.cpp
* Description : Using EDAs to optimize the multimodel function.
************************************************************************/
#include "EDAs.h"

double shift_array[Dim];             // ƫ������


// ƫ�Ƶ�Griewank ����
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

//// ƫ�Ƶ�Griewank ����
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
Description : ��ʼ����Ⱥ�������˴���u01ʹ����boots���uniform_01����������
��EDAs.h��
*
* Param : num: ��Ⱥ�Ĵ�С
* pop: ָ�룬ָ����Ⱥ
**
************************************************************************/
void initialize_population(int num, struct Individual *pop)
{
	int i, j;

	for (i = 0; i < num; i++)
	{
		for (j = 0; j < Dim; j++)
			pop[i].position[j] = u01() * (maxX - minX) + minX;  //u01��boots��uniform_01����
		pop[i].fitness = object_function(pop[i].position);
	}

}


/************************************************************************
* Function Name : comp
* Create Date : 2016/11/26
* Author/Corporation : bigzhao
**
Description : �ṹ�� Individual ֮��Ƚϴ�С�����ݺ�������std::qsortʹ��
*
* Param : a: ָ���һ������
* b: ָ����һ������
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
Description : �����׼��
*
* Param : x: �����������
* mean: ƽ��ֵ
* length: ���鳤��
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
Description : EDAs ������Ҫ��һ�����ڣ�ͨ����˹�ֲ�������һ����Ⱥ������ĸ�
˹����ʹ�� boots ��� normal_distribution���÷��ǰٶȲ�ģ��պ�úú���ĥһ
��
*
* Param : pop: ����Ⱥ
* new_pop: ����Ⱥ
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
		boost::normal_distribution<> nd(mean, stddev); // boots ��ĸ�˹�ֲ�����

		for (j = 0; j < NP; j++)
		{
			new_pop[j].position[i] = nd(u01);   // ��˹�ֲ������µ�ֵ����ɸ���

			if (new_pop[j].position[i] < minX)
				new_pop[j].position[i] = minX;
			else if (maxX < new_pop[j].position[i])
				new_pop[j].position[i] = maxX;
			//std::cout << mean << "  " << stddev << "  "<<  new_pop[j].position[i] << std::endl;
		}

	}
	// ���� fitness
	for (i = 0; i < NP; i++)
		new_pop[i].fitness = object_function(new_pop[i].position);
}


/************************************************************************
* Function Name : select
* Create Date : 2016/11/26
* Author/Corporation : bigzhao
**
Description : EDAs �����һ�����ڣ�����¾���Ⱥͨ����ʤ��̭ѡ����һ����Ⱥ����
��ʵ�ַ����ǽ�������Ⱥ����Ȼ���ù鲢�ķ������ NP ���⣬�� COPY ��POP
*
* Param : pop: ����Ⱥ
* new_pop: ����Ⱥ
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
Description : �ҵ���Ⱥ������õĸ���
*
* Param : pop: ��Ⱥ
* best: ��ø���
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
Description : ���㷨������:
step1: �����ʼ����Ⱥ
step2: �����ҳ�ǰ K ����õĸ���
step3: ���� step2 �ҳ��ĸ��������µ���Ⱥ
step4: �¾���Ⱥ��ʤ��̭��������һ����Ⱥ
step5: �����ֹ�����������˳�������ص�step2
*
* Param : pop: ��Ⱥ
* best: ��ø���
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
	generate_shift_array(); // ����ƫ������
	EDAs();
	return 0;
}