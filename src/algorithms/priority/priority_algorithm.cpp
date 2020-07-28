#include "algorithms/priority/priority_algorithm.hpp"

#include <cassert>
#include <stdexcept>
#include <sstream>

#define FMT_HEADER_ONLY
#include "utilities/fmt/format.h"

/*
    Here is where you should define the logic for the priority algorithm.
*/


PRIORITYScheduler::PRIORITYScheduler(int slice) {
    if (slice != -1) {
        throw("PRIORITY must have a timeslice of -1");
    }

}

std::shared_ptr<SchedulingDecision> PRIORITYScheduler::get_next_thread() {
    if(!system_queue.empty()){

        std::string message = "Selected from SYSTEM queue. [S: " + std::to_string(system_queue.size()) + " I: " + std::to_string(interactive_queue.size()) + " N: " + std::to_string(normal_queue.size()) + " B: " + std::to_string(batch_queue.size()) + "] -> ";
        std::string message2 = "[S: " + std::to_string(system_queue.size()) + " I: " + std::to_string(interactive_queue.size()) + " N: " + std::to_string(normal_queue.size()) + " B: " + std::to_string(batch_queue.size()) + "]";
        auto sd = std::make_shared<SchedulingDecision>();
        sd->explanation = message + message2;
        sd->thread = system_queue.front();
        sd->time_slice = -1;

        system_queue.pop();
        return sd; 

    }


    if(!interactive_queue.empty()){
 
         std::string message = "Selected from INTERACTIVE queue. [S: " + std::to_string(system_queue.size()) + " I: " + std::to_string(interactive_queue.size()) + " N: " + std::to_string(normal_queue.size()) + " B: " + std::to_string(batch_queue.size()) + "] -> ";
        std::string message2 = "[S: " + std::to_string(system_queue.size()) + " I: " + std::to_string(interactive_queue.size()) + " N: " + std::to_string(normal_queue.size()) + " B: " + std::to_string(batch_queue.size()) + "]";
        auto sd = std::make_shared<SchedulingDecision>();
        sd->explanation = message + message2;
        sd->thread = interactive_queue.front();
        sd->time_slice = -1; 

        interactive_queue.pop();
        return sd;
    }

    if(!normal_queue.empty()){
        
         std::string message = "Selected from NORMAL queue. [S: " + std::to_string(system_queue.size()) + " I: " + std::to_string(interactive_queue.size()) + " N: " + std::to_string(normal_queue.size()) + " B: " + std::to_string(batch_queue.size()) + "] -> ";
        std::string message2 = "[S: " + std::to_string(system_queue.size()) + " I: " + std::to_string(interactive_queue.size()) + " N: " + std::to_string(normal_queue.size()) + " B: " + std::to_string(batch_queue.size()) + "]";
        auto sd = std::make_shared<SchedulingDecision>();
        sd->explanation = message + message2;
        sd->thread = normal_queue.front();
        sd->time_slice = -1;

        normal_queue.pop();
        return sd; 

    }


    if(!batch_queue.empty()){
        
         std::string message = "Selected from BATCH queue. [S: " + std::to_string(system_queue.size()) + " I: " + std::to_string(interactive_queue.size()) + " N: " + std::to_string(normal_queue.size()) + " B: " + std::to_string(batch_queue.size()) + "] -> ";
        std::string message2 = "[S: " + std::to_string(system_queue.size()) + " I: " + std::to_string(interactive_queue.size()) + " N: " + std::to_string(normal_queue.size()) + " B: " + std::to_string(batch_queue.size()) + "]";
        auto sd = std::make_shared<SchedulingDecision>();
        sd->explanation = message + message2;
        sd->thread = batch_queue.front();
        sd->time_slice = -1; 

        batch_queue.pop();
        return sd;

    }  
    return nullptr;
}

void PRIORITYScheduler::add_to_ready_queue(std::shared_ptr<Thread> thread) {
    if (  thread->priority == SYSTEM){
      system_queue.push(thread);
    }
    if (thread->priority == INTERACTIVE){
      interactive_queue.push(thread);      
    }
    if (thread->priority == NORMAL){
      normal_queue.push(thread);      
    }
    if (thread->priority == BATCH){
      batch_queue.push(thread);      
    }  
}

size_t PRIORITYScheduler::size() const {

      return batch_queue.size() + normal_queue.size()  + interactive_queue.size() + system_queue.size();      
     
}