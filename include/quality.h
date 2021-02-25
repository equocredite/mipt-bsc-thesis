#ifndef MYACO_QUALITY_H
#define MYACO_QUALITY_H

#include "problem_data.h"

class QualityEstimator {
public:
    static double Estimate(const Schedule& schedule);

private:
    static int64_t CountSimultaneousAssignmentsForStudents(const Schedule& schedule);

    static int64_t CountUnsatisfiedRequirements(const Schedule& schedule);

    static int64_t CountHoles(const Schedule& schedule);

    static int64_t CountInconvenientAssignments(const Schedule& schedule);
};

#endif //MYACO_QUALITY_H
