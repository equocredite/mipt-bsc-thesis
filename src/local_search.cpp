#include "local_search.h"

ILocalSearcher::ILocalSearcher(const ProblemData* data, Config config)
        : data_(data)
        , config_(config) {
}


PairwiseSwapLocalSearcher::PairwiseSwapLocalSearcher(const ProblemData* data, Config config)
        : ILocalSearcher(data, config) {
}

Schedule PairwiseSwapLocalSearcher::Search(const Schedule& schedule) const {
    // TODO
    return schedule;
}
