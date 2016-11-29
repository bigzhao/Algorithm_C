#ifndef __EDAs__
#define __EDAs__ __EDAs__
#define Dim       100                                                // Œ Ã‚Œ¨∂»
#define minX      -100.0
#define maxX      100.0
#define NP        425
#define K         100
#define ITERATION 200
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <algorithm>
#include <iostream>
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>

boost::random::mt19937 rng((unsigned)time(NULL));
boost::uniform_real<double> real(0, 1);
boost::uniform_01<boost::mt19937&> u01(rng);

#endif // !__EDAs__
