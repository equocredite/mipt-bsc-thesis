#ifndef MYACO_PERTURBATION_H
#define MYACO_PERTURBATION_H

#include "schedule.h"
#include <vector>
#include <random>

class IPerturbator {
public:
    IPerturbator();

    virtual std::vector<Schedule::Move> GenerateMoves(const Schedule& schedule) = 0;

protected:
    std::mt19937 rng_;
    std::uniform_int_distribution<int64_t> teacher_id_gen_, slot_id_gen_;
};

class SimplePerturbator : public IPerturbator {
public:
    std::vector<Schedule::Move> GenerateMoves(const Schedule& schedule) override;
};

class Torquer : public IPerturbator {
public:
    std::vector<Schedule::Move> GenerateMoves(const Schedule& schedule) override;
};

#endif //MYACO_PERTURBATION_H
