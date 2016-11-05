#ifndef _FEA_
#define _FEA_ _FEA_

#define NUM  200//  problem size
#define NUM_PARTICLE 10 // particle size
#define NUM_ITERATION 20 // subpopularion iter 20 times
#define MAX_X 100.0
#define MIN_X -100.0
#define MIN_V (-1.0 * MAX_X)
#define MAX_V (MAX_X)
#define W 0.729
#define C1 1.49455
#define C2 1.49455 

#define L 2 //this is cs-l
#define SUBPOP_LEN (NUM - L)
#endif