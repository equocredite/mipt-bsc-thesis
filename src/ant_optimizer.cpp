#include "ant_optimizer.h"
#include <algorithm>

using namespace myaco;

AntOptimizer::AntOptimizer()
        : AntOptimizer(CreateInitialSchedule()) {
}

AntOptimizer::AntOptimizer(Schedule&& initial_schedule)
        : colonies_(config.n_colonies, Colony(initial_schedule)) {
}

void AntOptimizer::Run() {
    for (int64_t iter = 0;;) {
        std::vector<std::thread> threads;
        for (Colony& colony : colonies_) {
            threads.emplace_back(std::thread(&Colony::MakeNIterations, std::ref(colony), config.sync_frequency));
        }
        for (auto& thread : threads) {
            thread.join();
        }
        iter += config.sync_frequency;
        if (iter < config.n_iterations_aco) {
            SyncColonies();
        } else {
            break;
        }
    }
}

Schedule AntOptimizer::GetSchedule() {
    return GetBestColony()->GetSchedule();
}

void AntOptimizer::SyncColonies() {
    auto best_colony = GetBestColony();
    auto best_trail = best_colony->GetTrail();
    for (auto it = colonies_.begin(); it != colonies_.end(); ++it) {
        if (it != best_colony) {
            it->SetTrail(best_trail);
        }
    }
}

std::vector<Colony>::iterator AntOptimizer::GetBestColony() {
    return std::min_element(colonies_.begin(),colonies_.end(),[](const Colony& c1, const Colony& c2) {
        return c1.GetQuality() < c2.GetQuality();
    });
}

Schedule AntOptimizer::CreateInitialSchedule() {
    Schedule schedule = CreateSchedule(data.n_teachers, data.week_length);
    for (int64_t teacher_id = 0; teacher_id < data.n_teachers; ++teacher_id) {
        int64_t next_slot = 0;
        for (int64_t student_id = 0; student_id < data.n_students; ++student_id) {
            int64_t required = data.requirements[teacher_id][student_id];
            while (required--) {
                while (next_slot < data.week_length && !data.available[teacher_id][next_slot]) {
                    ++next_slot;
                }
                if (next_slot == data.week_length) {
                    throw std::invalid_argument("unsatisfiable: teacher " +
                                                std::to_string(teacher_id) + " does not have enough slots available");
                }
                schedule[teacher_id][next_slot++] = student_id;
            }
        }
    }
    return schedule;
}


