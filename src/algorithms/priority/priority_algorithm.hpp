#ifndef PRIORITY_ALGORITHM_HPP
#define PRIORITY_ALGORITHM_HPP

#include <memory>
#include <string>
#include <vector>
#include "algorithms/fcfs/fcfs_algorithm.hpp"
#include "algorithms/scheduling_algorithm.hpp"

/*
    PRIORITYScheduler:
        A representation of a priority scheduling algorithm.

        This is a derived class from the base scheduling algorithm class.

        You are free to add any member functions or member variables that you
        feel are helpful for implementing the algorithm.
*/

class PRIORITYScheduler : public Scheduler {
public:

    //==================================================
    //  Member variables
    //==================================================

    std::queue<std::shared_ptr<Thread>> system_queue;
    std::queue<std::shared_ptr<Thread>> interactive_queue;
    std::queue<std::shared_ptr<Thread>> normal_queue;
    std::queue<std::shared_ptr<Thread>> batch_queue;


    //==================================================
    //  Member functions
    //==================================================

    PRIORITYScheduler(int slice = -1);

    std::shared_ptr<SchedulingDecision> get_next_thread();

    void add_to_ready_queue(std::shared_ptr<Thread> thread);

    size_t size() const;

};

#endif
