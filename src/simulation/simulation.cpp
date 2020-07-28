#include <fstream>
#include <iostream>

#include "algorithms/fcfs/fcfs_algorithm.hpp"
#include "algorithms/rr/rr_algorithm.hpp"
#include "algorithms/priority/priority_algorithm.hpp"
#include "algorithms/mlfq/mlfq_algorithm.hpp"
#include "algorithms/custom/custom_algorithm.hpp"

#include "simulation/simulation.hpp"
#include "types/enums.hpp"

#include "utilities/flags/flags.hpp"

Simulation::Simulation(FlagOptions flags) {
    // Hello!
    if (flags.scheduler == "FCFS") {
        // Create a FCFS scheduling algorithm
        this->scheduler = std::make_shared<FCFSScheduler>();
    } else if (flags.scheduler == "RR") {
        // Create a RR scheduling algorithm
        this->scheduler = std::make_shared<RRScheduler>(flags.time_slice);
    } else if (flags.scheduler == "PRIORITY") {
        // Create a PRIORITY scheduling algorithm
        this->scheduler = std::make_shared<PRIORITYScheduler>();
    } else if (flags.scheduler == "MLFQ") {
        // Create a MLFQ scheduling algorithm
    } else if (flags.scheduler == "CUSTOM") {
        // Create a custom scheduling algorithm
    }
    this->flags = flags;
    this->logger = Logger(flags.verbose, flags.per_thread, flags.metrics);
}

void Simulation::run() {
    this->read_file(this->flags.filename);
    
    while (!this->events.empty()) {
        auto event = this->events.top();
        this->events.pop();

        // Invoke the appropriate method in the simulation for the given event type.

        switch(event->type) {
            case THREAD_ARRIVED:
                this->handle_thread_arrived(event);
                break;

            case THREAD_DISPATCH_COMPLETED:
            case PROCESS_DISPATCH_COMPLETED:
                this->handle_dispatch_completed(event);
                break;

            case CPU_BURST_COMPLETED:
                this->handle_cpu_burst_completed(event);
                break;

            case IO_BURST_COMPLETED:
                this->handle_io_burst_completed(event);
                break;
            case THREAD_COMPLETED:
                this->handle_thread_completed(event);
                break;

            case THREAD_PREEMPTED:
                this->handle_thread_preempted(event);
                break;

            case DISPATCHER_INVOKED:
                this->handle_dispatcher_invoked(event);
                break;
        }

        // If this event triggered a state change, print it out.
        if (event->thread && event->thread->current_state != event->thread->previous_state) {
            this->logger.print_state_transition(event, event->thread->previous_state, event->thread->current_state);
        }
        this->system_stats.total_time = event->time;
        event.reset();
    }
    // We are done!

    std::cout << "SIMULATION COMPLETED!\n\n";

    for (auto entry: this->processes) {
        this->logger.print_per_thread_metrics(entry.second);
    }

    logger.print_simulation_metrics(this->calculate_statistics());
}

//==============================================================================
// Event-handling methods
//==============================================================================

void Simulation::handle_thread_arrived(const std::shared_ptr<Event> event) {

    event->thread->set_ready(event->time); //set thread to ready
    event->thread->arrival_time = event->time; //set the arrival time of thread
    scheduler->add_to_ready_queue(event->thread); //add the thread to the ready queue

    if (active_thread == nullptr){
        event_num++;
        std::shared_ptr<Event> newDispatcherEvent = std::make_shared<Event>(DISPATCHER_INVOKED,event->time, event_num, nullptr, nullptr);
        add_event(newDispatcherEvent);
    }

}
void Simulation::handle_dispatch_completed(const std::shared_ptr<Event> event) {
    
    event->thread->set_running(event->time); //set thread to running

    if(scheduler->time_slice > 0 && scheduler->time_slice < event->thread->get_next_burst(CPU)->length){

        event_num++; //increment event_num
        std::shared_ptr<Event> newThreadEvent = std::make_shared<Event>(THREAD_PREEMPTED, event->time + scheduler->time_slice, event_num, event->thread, nullptr);
        add_event(newThreadEvent); // add event to the queue
        event->thread->get_next_burst(BurstType::CPU)->length -= scheduler->time_slice; // update time slice
        system_stats.service_time += scheduler->time_slice; //update system service time
        event->thread->service_time += scheduler->time_slice; //update thread service time
    }else{

        if(event->thread->bursts.size() <= 1){
        event_num++;
        std::shared_ptr<Event> newThreadEvent = std::make_shared<Event>(THREAD_COMPLETED,event->time + event->thread->get_next_burst(CPU)->length, event_num, event->thread, nullptr);
        system_stats.service_time += event->thread->get_next_burst(BurstType::CPU)->length; //update system service time
        event->thread->service_time += event->thread->get_next_burst(BurstType::CPU)->length;  //update thread service time
        add_event(newThreadEvent); // add event to the queue
        event->thread->pop_next_burst(CPU); // POP!!
        
        }
        else if (event->thread->bursts.size() > 1){
    
        event_num++;
        std::shared_ptr<Event> newCPUEvent = std::make_shared<Event>(CPU_BURST_COMPLETED, event->thread->get_next_burst(CPU)->length + event->time,event_num, event->thread, nullptr);
        system_stats.service_time += event->thread->get_next_burst(BurstType::CPU)->length; //update system service time
        event->thread->service_time += event->thread->get_next_burst(BurstType::CPU)->length; //update thread service time
        event->thread->pop_next_burst(CPU);       // POP!!!       
        add_event(newCPUEvent);  // add event to the queue 
        }
    }    
}

void Simulation::handle_cpu_burst_completed(const std::shared_ptr<Event> event) {
    
    if(!event->thread->bursts.empty()){ //make sure queue isnt empty
        event->thread->set_blocked(event->time); //set thread to blocked!
       
        
        event_num++;
        std::shared_ptr<Event> newDispatcherEvent = std::make_shared<Event>(DISPATCHER_INVOKED,event->time,event_num, nullptr, nullptr);
        add_event(newDispatcherEvent); // add event to the queue
       

        event_num++;
        std::shared_ptr<Event> newIOEvent = std::make_shared<Event>(IO_BURST_COMPLETED, event->thread->get_next_burst(IO)->length + event->time,event_num, event->thread, nullptr);
        system_stats.io_time += event->thread->get_next_burst(IO)->length; //update system io time
        event->thread->io_time += event->thread->get_next_burst(IO)->length;  //update thread io time 
        event->thread->pop_next_burst(IO);  // POP!!!
        add_event(newIOEvent);   //add event to the queue
    }
}


void Simulation::handle_io_burst_completed(const std::shared_ptr<Event> event) {
    
    event->thread->set_ready(event->type); // set thread to ready after IO burst
    scheduler->add_to_ready_queue(event->thread); // add thread to the ready queue
    
    if(active_thread == nullptr){
        event_num++;
        std::shared_ptr<Event> newDispatcherEvent = std::make_shared<Event>(DISPATCHER_INVOKED, event->time, event_num, nullptr, nullptr);
        add_event(newDispatcherEvent);

    }

    

 
}

void Simulation::handle_thread_completed(const std::shared_ptr<Event> event) {

    event->thread->set_finished(event->time); // set the thread to finished
    //prev_thread = active_thread;
    //active_thread = nullptr; //set the active thread to = nullptr leaving the CPU idel
   // calculate all of the thread statistics!!!
    system_stats.total_time = event->time; //update total time
    system_stats.thread_counts[event->thread->priority]++; //update thread counts
    system_stats.avg_thread_response_times[event->thread->priority] += event->thread->start_time - event->thread->arrival_time; //update response time
    system_stats.avg_thread_turnaround_times[event->thread->priority] += event->thread->end_time - event->thread->arrival_time; //update turnaround time

    event_num++;
    std::shared_ptr<Event> newDispatcherEvent = std::make_shared<Event>(DISPATCHER_INVOKED, event->time, event_num, nullptr, nullptr);
    add_event(newDispatcherEvent);
     
}

void Simulation::handle_thread_preempted(const std::shared_ptr<Event> event) {
    event->thread->set_ready(event->time);
    scheduler->add_to_ready_queue(event->thread);

    event_num++;
    std::shared_ptr<Event> newDispatcherEvent = std::make_shared<Event>(DISPATCHER_INVOKED, event->time, event_num, nullptr, nullptr);
    add_event(newDispatcherEvent);
           
}

void Simulation::handle_dispatcher_invoked(const std::shared_ptr<Event> event) {

    std::shared_ptr<SchedulingDecision> NewThread = scheduler->get_next_thread(); 

    if(active_thread != nullptr){ //Is the CPU idle???
        prev_thread = active_thread;
    }

    if(NewThread == nullptr){
        active_thread = nullptr;
        return;
    }
      
    if(prev_thread == nullptr || prev_thread->process_id !=NewThread->thread->process_id){
        event_num++;
        std::shared_ptr<Event> newDispatcherEvent = std::make_shared<Event>(PROCESS_DISPATCH_COMPLETED,event->time + process_switch_overhead,event_num, NewThread->thread, NewThread);
        add_event(newDispatcherEvent);  // add event to queue
        system_stats.dispatch_time += process_switch_overhead; //update the dispatcher time on system
    }else{
        event_num++;
        std::shared_ptr<Event> newThreadEvent = std::make_shared<Event>(THREAD_DISPATCH_COMPLETED, event->time + thread_switch_overhead, event_num,NewThread->thread, NewThread);
        add_event(newThreadEvent); // add event to the queue
        system_stats.dispatch_time += thread_switch_overhead; //update system dispatch time
        }
        
    active_thread = NewThread->thread;

    this->logger.print_verbose(event,active_thread, NewThread->explanation); //seg fault when added

    
}

//==============================================================================
// Utility methods
//==============================================================================

SystemStats Simulation::calculate_statistics(){
    //Loop through the thread types and get all of the stats for remaining values 
    for(int i=0; i < 4; i++){
        if(system_stats.thread_counts[i] != 0){
        system_stats.avg_thread_turnaround_times[i] = system_stats.avg_thread_turnaround_times[i]/system_stats.thread_counts[i];
        system_stats.avg_thread_response_times[i] = system_stats.avg_thread_response_times[i]/system_stats.thread_counts[i];
        }
    }
    //update all of the system stats to get the proper calulation when printing.
    system_stats.total_cpu_time = system_stats.dispatch_time + system_stats.service_time;
    system_stats.total_idle_time = system_stats.total_time - system_stats.total_cpu_time;
    system_stats.cpu_efficiency = 100.0 * system_stats.service_time / system_stats.total_time;
    system_stats.cpu_utilization = 100.0 * system_stats.total_cpu_time / system_stats.total_time;
    return this->system_stats;
}

void Simulation::add_event(std::shared_ptr<Event> event) {
    if (event != nullptr) {
        this->events.push(event);
    }
}

void Simulation::read_file(const std::string filename) {
    std::ifstream input_file(filename.c_str());

    if (!input_file) {
        std::cerr << "Unable to open simulation file: " << filename << std::endl;
        throw(std::logic_error("Bad file."));
    }

    int num_processes;

    input_file >> num_processes >> this->thread_switch_overhead >> this->process_switch_overhead;

    for (int proc = 0; proc < num_processes; ++proc) {
        auto process = read_process(input_file);

        this->processes[process->process_id] = process;
    }
}

std::shared_ptr<Process> Simulation::read_process(std::istream& input) {
    int process_id, priority;
    int num_threads;

    input >> process_id >> priority >> num_threads;

    auto process = std::make_shared<Process>(process_id, (ProcessPriority) priority);

    // iterate over the threads
    for (int thread_id = 0; thread_id < num_threads; ++thread_id) {
        process->threads.emplace_back(read_thread(input, thread_id, process_id, (ProcessPriority) priority));
    }

    return process;
}

std::shared_ptr<Thread> Simulation::read_thread(std::istream& input, int thread_id, int process_id, ProcessPriority priority) {
    // Stuff
    int arrival_time;
    int num_cpu_bursts;

    input >> arrival_time >> num_cpu_bursts;

    auto thread = std::make_shared<Thread>(arrival_time, thread_id, process_id, priority);

    for (int n = 0, burst_length; n < num_cpu_bursts * 2 - 1; ++n) {
        input >> burst_length;

        BurstType burst_type = (n % 2 == 0) ? BurstType::CPU : BurstType::IO;

        thread->bursts.push(std::make_shared<Burst>(burst_type, burst_length));
    }

    this->events.push(std::make_shared<Event>(EventType::THREAD_ARRIVED, thread->arrival_time, this->event_num, thread, nullptr));
    this->event_num++;

    return thread;
}
