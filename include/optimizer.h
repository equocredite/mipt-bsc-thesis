#ifndef MYACO_OPTIMIZER_H
#define MYACO_OPTIMIZER_H

#include "matrix.h"
#include "problem_data.h"

class IOptimizer {
public:
    virtual void Run() = 0;

    virtual Schedule GetSchedule() = 0;

    virtual ~IOptimizer() = default;
};


#endif //MYACO_OPTIMIZER_H
