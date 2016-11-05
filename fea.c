#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "fea.h"
#include "time.h"
#include "math.h"


struct Particle
{
	int size;
	double fitness;
	double best_fitness;
	double position[L+1];
	double velocity[L+1];
	double best_position[L+1];
};


struct SubPopulation
{
	struct Particle swarm[NUM_PARTICLE];
	double best_global_position[L+1];
	double best_global_fitness;
}sub_populations[SUBPOP_LEN];


double object_function1(double x[])
{
	int i;
    double sum = 0;
	for(i = 0;i < NUM; i++)
	{
		sum += (x[i] * x[i]);
	}
    return sum;
}


double object_function(double x[])
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


void make_array_random(int a[], int n)
{
	int index, tmp, i;
	srand(time(NULL));
	for (i = 0; i <n; i++)
	{
		index = rand() % (n - i) + i;
		if (index != i)
		{
			tmp = a[i];
			a[i] = a[index];
			a[index] = tmp;
		}
	}
}


int judge_in_array(int element, int array[], int length)    //judge a element whether in sub_index, if in return 1, else return 0 
{
	int i;
	for(i = 0; i < length; i++)
	{
		if(element == array[i])
			return 1;
	}
	return 0;
}

/* 

:param sub_index:

*/

void rebulid(int single_index[], int index_length, double sub_solution[], double global_solution[], double rebulid_solution[]) //rebulit solution
{
	int i, index, j = 0;
	// printf("%d\n", index_length);

	for(i = 0; i < index_length; i++)
	{

		rebulid_solution[single_index[i]] = sub_solution[j++];

	}

}


void initialize_subpops(int sub_index[SUBPOP_LEN][L + 1], struct SubPopulation sub_populations[], double global_solution[])
{
	int i, j, k;

	double best_global_fitness;
	double random_position[L + 1];
	double random_velocity[L + 1];
	double fitness;
	double rebulid_solution[NUM];

	for(i = 0; i < SUBPOP_LEN; i++)
	{
		printf("the %dth subgroup: ", i+1);
		for(j = 0; j < L + 1; j++)
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
		for(j = 0;j < NUM_PARTICLE; j++)
		{
			for(k = 0; k < L + 1; k++)
			{
				random_position[k] = (MAX_X - MIN_X) * rand() / (double)RAND_MAX  + MIN_X;
				random_velocity[k] = (MAX_V - MIN_V) * rand() / (double)RAND_MAX  + MIN_V;
			}
			memcpy(rebulid_solution, global_solution, sizeof(double) *NUM);
			rebulid(sub_index[i], L+1, random_position, global_solution, rebulid_solution);
			fitness = object_function(rebulid_solution);
			// struct Particle p = {L + 1,fitness, fitness, *random_position, *random_velocity, *random_position};
			
			// sub_populations[i].swarm[j] = p;
			sub_populations[i].swarm[j].size = L + 1;
			sub_populations[i].swarm[j].fitness = fitness;
			sub_populations[i].swarm[j].best_fitness = fitness;
			memcpy(sub_populations[i].swarm[j].position, random_position, sizeof(double)*(L+1));
			memcpy(sub_populations[i].swarm[j].best_position, random_position, sizeof(double)*(L+1));
			memcpy(sub_populations[i].swarm[j].velocity, random_velocity, sizeof(double)*(L+1));


			if(sub_populations[i].swarm[j].fitness < best_global_fitness)
			{
				// printf("\n");
				// printf("xiaoyule \n");s
				best_global_fitness = sub_populations[i].swarm[j].fitness;
				for (k = 0; k < L+1; k++)
				{
					sub_populations[i].best_global_position[k] = sub_populations[i].swarm[j].position[k];
				}
				// printf("%lf, %lf\n", sub_populations[i].swarm[j].fitness, sub_populations[i].best_global_fitness);
				// printf("\n");
			}
			sub_populations[i].best_global_fitness = best_global_fitness;
			// printf("%lf %lf\n", p.fitness, sub_populations[i].best_global_fitness);
		}
	}
}


void pso(int index[], int index_length, struct SubPopulation *sub_population, double global_solution[])
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
			// currP = (*sub_population).swarm[j];
			for(k = 0; k < index_length; k++)
			{
				r1 = rand() / (double)RAND_MAX;
				r2 = rand() / (double)RAND_MAX;

				new_velocity = (W * (*sub_population).swarm[j].velocity[k]) + 
				               (C1 * r1 * ((*sub_population).swarm[j].best_position[k] - (*sub_population).swarm[j].position[k])) + 
				               (C2 * r2 * ((*sub_population).best_global_position[k] - (*sub_population).swarm[j].position[k]));
				// printf("r1:%lf r2:%lf\n", r1, r2);
				if(new_velocity < MIN_V)
				{
					new_velocity = MIN_V;
				}

				else if (MAX_V < new_velocity)
				{
					new_velocity = MAX_V;
				}

				(*sub_population).swarm[j].velocity[k] = new_velocity;

				new_position = (*sub_population).swarm[j].position[k] + new_velocity;
				if(new_position < MIN_X)
				{
					new_position = MIN_X;
				}
				else if(MAX_X < new_position)
				{
					new_position = MAX_X;
				}
				(*sub_population).swarm[j].position[k] = new_position;
			}

			memcpy(rebulid_solution, global_solution, sizeof(double)*NUM);

			rebulid(index, index_length, (*sub_population).swarm[j].position, global_solution, rebulid_solution);
			new_fitness = object_function(rebulid_solution);

            //tihuan jubu position
			if(new_fitness < (*sub_population).swarm[j].best_fitness)
			{
				(*sub_population).swarm[j].best_fitness = new_fitness;
				for (k = 0; k < index_length; k++)
				{
					(*sub_population).swarm[j].best_position[k] = (*sub_population).swarm[j].position[k];
				}
			}
            // tihuan quanju position
			if(new_fitness < (*sub_population).best_global_fitness)                
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

	for(i = 0; i < NUM; i++)
    {
    	random_array[i] = i;
    	// printf("%d\n", random_array[i]);
    }
	for (i = 0; i < NUM; ++i)
	{ 
		best_fitness = object_function(global_solution);

		for(j = 0; j < SUBPOP_LEN; j++)
		{
			if(judge_in_array(random_array[i], sub_index[j], L + 1))
			{
				global_solution[random_array[i]] = sub_populations[sub_index[j][0]].best_global_position[random_array[i] - sub_index[j][0]];
				fitness = object_function(global_solution);
				if(fitness < best_fitness)
				{
					best_solution[random_array[i]] = global_solution[random_array[i]];
					best_fitness = fitness;
				}
			}
		}
	}
	memcpy(global_solution, best_solution, sizeof(double) * NUM);
}


void sharing(int sub_indexp[SUBPOP_LEN][L + 1], double global_solution[])
{
	int i, j, worst, random_array[NUM -L];

    for(i = 0; i < SUBPOP_LEN; i++)
    {
    	random_array[i] = i;
    }
	make_array_random(random_array, SUBPOP_LEN);

	for(i = 0; i < SUBPOP_LEN; i++)
	{
		worst = 0;
		for(j = 1; j < NUM_PARTICLE; j++)
		{
			if(sub_populations[random_array[i]].swarm[worst].fitness < sub_populations[random_array[i]].swarm[j].fitness)
			{
				worst = j;
			}

		}
		for(j = 0; j < L + 1; j++)
		{
			sub_populations[random_array[i]].swarm[worst].position[j] = global_solution[sub_indexp[random_array[i]][j]];
		}
	}
}


void fea()
{
		int sub_index[SUBPOP_LEN][L + 1], i;
	int record = 0;
	double global_solution[NUM], last_fitness;
	srand((unsigned)time(NULL));
	// make a global solution
	for(i = 0; i < NUM; i++)
	{
		global_solution[i] = (double)(MAX_X - MIN_X) * rand() / (double)RAND_MAX +  MIN_X;
	}

    initialize_subpops(sub_index, sub_populations, global_solution);

    last_fitness = object_function(global_solution);

    while(1)
    {
    	for(i = 0; i < SUBPOP_LEN; i++)
    	{
    		pso(sub_index[i], L+1, &sub_populations[i], global_solution);
    	}
    	competition(sub_index, global_solution);
    	sharing(sub_index, global_solution);
    	if(last_fitness > object_function(global_solution))
    	{
    		record = 0;
    		last_fitness = object_function(global_solution);
    	}
    	else
    	{
    		record += 1;
    	}
    	if(15 <= record)
    		break;
    }

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