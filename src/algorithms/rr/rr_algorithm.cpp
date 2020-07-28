#include "algorithms/rr/rr_algorithm.hpp"

#include <cassert>
#include <stdexcept>
#include <sstream>

/*
    Here is where you should define the logic for the round robin algorithm.
*/

RRScheduler::RRScheduler(int slice) {    
     if (slice > 0) {
        RRScheduler::time_slice = slice;
    }else{
        RRScheduler::time_slice = 3;
    }
}

std::shared_ptr<SchedulingDecision> RRScheduler::get_next_thread() {
    if(!RRqueue.empty()){
        
    std::string message = "Selected from " + std::to_string(RRqueue.size()) + " threads. Will run for at most " + std::to_string(RRScheduler::time_slice) + " ticks.";

    auto sd = std::make_shared<SchedulingDecision>();
        sd->explanation = message;
        sd->thread = RRqueue.front();
        sd->time_slice = Scheduler::time_slice; 

    RRqueue.pop();
    return sd;

    }
    return nullptr;
}

void RRScheduler::add_to_ready_queue(std::shared_ptr<Thread> thread) {
    RRqueue.push(thread);
}

size_t RRScheduler::size() const {
    return RRqueue.size();
}
