#ifndef CUSTOM_ALGORITHM_HPP
#define CUSTOM_ALGORITHM_HPP

#include <memory>
#include "algorithms/scheduling_algorithm.hpp"

/*
    CustomScheduler:
        This is a custom algorithm of your own design.
        
        This is a derived class from the base scheduling algorithm class.

        You are free to add any member functions or member variables that you
        feel are helpful for implementing the algorithm.
*/

class CustomScheduler : public Scheduler {
public:

    //==================================================
    //  Member variables
    //==================================================

    // Add any member variables you may need.


    //==================================================
    //  Member functions
    //==================================================

    CustomScheduler(int slice = -1);

    std::shared_ptr<SchedulingDecision> get_next_thread();

    void add_to_ready_queue(std::shared_ptr<Thread> thread);

    size_t size() const;

};

#endif
