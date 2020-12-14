#ifndef MYACO_LOCAL_SEARCH_H
#define MYACO_LOCAL_SEARCH_H

#include "config.h"
#include "matrix.h"
#include "problem_data.h"

class ILocalSearcher {
public:
    explicit ILocalSearcher(const ProblemData* data, Config config = Config());

    virtual Schedule Search(const Schedule& schedule) const = 0;

protected:
    const ProblemData* data_;
    Config config_;
};

class PairwiseSwapLocalSearcher : public ILocalSearcher {
public:
    explicit PairwiseSwapLocalSearcher(const ProblemData* data, Config config = Config());

    Schedule Search(const Schedule& schedule) const override;
};

#endif //MYACO_LOCAL_SEARCH_H
