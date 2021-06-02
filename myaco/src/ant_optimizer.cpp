#include "ant_optimizer.h"
#include "time_util.h"
#include <algorithm>

using namespace myaco;
using std::chrono::steady_clock;

AntOptimizer::AntOptimizer()
        : AntOptimizer(CreateInitialSchedule()) {
}

AntOptimizer::AntOptimizer(const Schedule& initial_schedule)
        : colonies_(config.aco_n_colonies, Colony(initial_schedule, visibility = CalcVisibility()))
        , best_quality_(initial_schedule.GetQuality()) {
}

void AntOptimizer::Run(Timer::fsec max_time, Timer::fsec log_frequency) {
    Run(max_time, log_frequency, 0.5s);
}

void AntOptimizer::Run(Timer::fsec max_time, Timer::fsec log_frequency, Timer::fsec sync_frequency) {
    Timer timer(max_time);
    while (!timer.Expired()) {
        if (timer.Tick(log_frequency)) {
            history_.emplace_back(timer.TimeElapsed(), best_quality_);
        }
        std::vector<std::thread> threads;
        for (Colony& colony : colonies_) {
            threads.emplace_back(std::thread(&Colony::Run, std::ref(colony), sync_frequency));
        }
        for (auto& thread : threads) {
            thread.join();
        }
        SyncColonies();
    }
}

Schedule& AntOptimizer::GetSchedule() {
    return GetBestColony()->GetSchedule();
}

void AntOptimizer::SyncColonies() {
    auto best_colony = GetBestColony();
    auto best_trail = best_colony->GetTrail();
    best_quality_ = best_colony->GetQuality();
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
    auto table = CreateMatrix2D<std::optional<int64_t>>(data.n_teachers, data.n_slots);
    for (int64_t teacher_id = 0; teacher_id < data.n_teachers; ++teacher_id) {
        int64_t next_slot = 0;
        for (int64_t student_id = 0; student_id < data.n_students; ++student_id) {
            int64_t required = data.requirements[teacher_id][student_id];
            while (required--) {
                while (next_slot < data.n_slots && !data.teacher_available[teacher_id][next_slot]) {
                    ++next_slot;
                }
                if (next_slot == data.n_slots) {
                    throw std::invalid_argument("unsatisfiable: teacher " +
                                                std::to_string(teacher_id) + " does not have enough available slots");
                }
                table[teacher_id][next_slot++] = student_id;
            }
        }
    }
    return Schedule(table);
}

Matrix3D<double> AntOptimizer::CalcVisibility() {
    auto visibility = myaco::CreateMatrix3D<double>(data.n_teachers, data.n_students, data.n_slots);
    for (int64_t teacher_id = 0; teacher_id < data.n_teachers; ++teacher_id) {
        for (int64_t student_id = 0; student_id < data.n_students; ++student_id) {
            if (data.requirements[teacher_id][student_id] == 0) {
                continue;
            }
            for (int64_t slot_id = 0; slot_id < data.n_slots; ++slot_id) {
                if (!data.teacher_available[teacher_id][slot_id] || !data.student_available[student_id][slot_id]) {
                    continue;
                }
                visibility[teacher_id][student_id][slot_id] =
                        static_cast<double>(data.requirements[teacher_id][student_id]);
                // visibilities do not change, so we can exponentiate it right away
                visibility[teacher_id][student_id][slot_id] = pow(visibility[teacher_id][student_id][slot_id], config.visibility_weight);
            }
        }
    }
    return visibility;
}
