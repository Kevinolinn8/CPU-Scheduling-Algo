#include <cassert>
#include <stdexcept>
#include "algorithms/custom/custom_algorithm.hpp"

#define FMT_HEADER_ONLY
#include "utilities/fmt/format.h"

/*
    Here is where you should define the logic for the custom algorithm.
*/

CustomScheduler::CustomScheduler(int slice) {
    // TODO
}

std::shared_ptr<SchedulingDecision> CustomScheduler::get_next_thread() {
    // TODO
    return nullptr;
}

void CustomScheduler::add_to_ready_queue(std::shared_ptr<Thread> thread) {
    // TODO
}

size_t CustomScheduler::size() const {
    // TODO
    return 0;
}
