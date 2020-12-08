#ifndef MYACO_OPTIMIZER_H
#define MYACO_OPTIMIZER_H

#include "colony.h"
#include <thread>
#include <iostream>

class AntOptimizer {
public:
    explicit AntOptimizer(ProblemData&& data, Config config = Config());

    void Run();

    Schedule GetSchedule();

private:
    void SyncColonies();

    std::vector<Colony>::iterator GetBestColony();

    const ProblemData data_;
    const Config config_;

    std::vector<Colony> colonies_;
};

#endif //MYACO_OPTIMIZER_H
