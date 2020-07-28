#include "algorithms/mlfq/mlfq_algorithm.hpp"

#include <cassert>
#include <stdexcept>

#define FMT_HEADER_ONLY
#include "utilities/fmt/format.h"

/*
    Here is where you should define the logic for the MLFQ algorithm.
*/

MFLQScheduler::MFLQScheduler(int slice) {
    // TODO
}

std::shared_ptr<SchedulingDecision> MFLQScheduler::get_next_thread() {
    // TODO
    return nullptr;
}

void MFLQScheduler::add_to_ready_queue(std::shared_ptr<Thread> thread) {
    // TODO
}

size_t MFLQScheduler::size() const {
    // TODO
    return 0;
}
