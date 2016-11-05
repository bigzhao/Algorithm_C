#ifndef _FEA_
#define _FEA_ _FEA_

#define NUM  200                          // 待优化问题的维度 
#define NUM_PARTICLE 10                   // pso 粒子群的 size
#define NUM_ITERATION 20                  // 亚种群每次优化迭代的次数
#define MAX_X 32.0      				  // 最大值 防止越界 依照问题而定
#define MIN_X -32.0						  // 最小值 防止越界 依照问题而定
#define MIN_V (-1.0 * MAX_X)              // PSO 的最小速度 
#define MAX_V (MAX_X)                     // PSO 的最大速度 
#define W 0.729							  // PSO 的权重 
#define C1 1.49455                        // PSO 学习因子
#define C2 1.49455                        // PSO 学习因子
#define L 2 //this is cs-l                // CS-L 结构的 L
#define SUBPOP_LEN (NUM - L)              // CS-L 结构的亚种群的size
#define PI 3.1415926                      // 派

#endif