#include "algorithms/fcfs/fcfs_algorithm.hpp"

#include <cassert>
#include <stdexcept>
#include <string>
#include <sstream>
#include <iostream>

#define FMT_HEADER_ONLY
#include "utilities/fmt/format.h"

/*
    Here is where you should define the logic for the FCFS algorithm.
*/


FCFSScheduler::FCFSScheduler(int slice) {
    if (slice != -1) {
        throw("FCFS must have a timeslice of -1");
    }
}

std::shared_ptr<SchedulingDecision> FCFSScheduler::get_next_thread() {
    
    if(!FCFSqueue.empty()){
    
    std::string message = "Selected from " + std::to_string(FCFSqueue.size()) + " threads. Will run to completion of burst.";

    auto sd = std::make_shared<SchedulingDecision>();
    sd->explanation = message;
    sd->thread = FCFSqueue.front();
    sd->time_slice = -1; 

    FCFSqueue.pop();
    return sd;
        
    }


    return nullptr;
}

void FCFSScheduler::add_to_ready_queue(std::shared_ptr<Thread> thread) {
    FCFSqueue.push(thread);
}

size_t FCFSScheduler::size() const {
    return FCFSqueue.size();
}
