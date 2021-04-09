#ifndef MYACO_QUALITY_H
#define MYACO_QUALITY_H

#include "problem_data.h"
#include "matrix.h"
#include "schedule.h"

class QualityEstimator {
public:
    static double Estimate(const Schedule& schedule);

private:
    static int64_t CountStudentAvailabilityViolations(const Schedule& schedule);
};

#endif //MYACO_QUALITY_H
