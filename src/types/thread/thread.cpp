#include <cassert>
#include <cstddef>
#include <stdexcept>
#include "thread.hpp"


void Thread::set_ready(int time) {
    previous_state = current_state;
    current_state = ThreadState::READY;

    if(previous_state == ThreadState::RUNNING){
        auto frontBurst = get_next_burst(CPU);
        frontBurst->update_time(time - state_change_time);
    }
    state_change_time = time;
}

void Thread::set_running(int time) {

    if(previous_state == ThreadState::EXIT){
        throw("This is NOT a valid transition");
    }

    if (current_state == ThreadState::READY){    
        previous_state = current_state;
        current_state = ThreadState::RUNNING;
    }

    if(start_time == -1){
        start_time = time;
    }

    state_change_time = time;
}

void Thread::set_blocked(int time) {

    if(previous_state == ThreadState::NEW){
        throw("This is NOT a valid transition");
    }

    previous_state = current_state;
    current_state = ThreadState::BLOCKED;

    state_change_time = time;
}

void Thread::set_finished(int time) {

    if(previous_state == ThreadState::EXIT){
        throw("This is NOT a valid transition");
    }

    previous_state = current_state;
    current_state = ThreadState::EXIT;

    if(end_time == -1){
        end_time = time;
    }

    state_change_time = time;

}

int Thread::response_time() const {
    return start_time - arrival_time;
}

int Thread::turnaround_time() const {
    return end_time - arrival_time;
}

void Thread::set_state(ThreadState state, int time) {
        previous_state = current_state;
        current_state = state;

        state_change_time = time;
}

std::shared_ptr<Burst> Thread::get_next_burst(BurstType type) {
        //Make sure queue is not empty
        //Check to see if burst of next burst is same as the parameter
        if(bursts.size() > 0 && bursts.front()->burst_type == type){
        return bursts.front();
        }

        return nullptr;   
}

std::shared_ptr<Burst> Thread::pop_next_burst(BurstType type) {

     //Make sure queue is not empty
    //if(!bursts.empty()){
        //Check to see if burst of next burst is same as the parameter
        if(bursts.size() > 0 && bursts.front()->burst_type == type){
            std::shared_ptr<Burst> NextBurst = bursts.front();
            bursts.pop();
            return NextBurst;
        }
    //}
        return nullptr;
      
}

