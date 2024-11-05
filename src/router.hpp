#ifndef ROUTER_H
#define ROUTER_H

#include "common.hpp"
#include "buffer.hpp"
#include "compute_unit.hpp"


class Router {
    private:
        Buffer *buffer;
        ComputeUnit *cu;
        int buffer_id;
        float clock;
        float bandwidth;
        float bytes_per_cycle;

        // Current state
        bool new_request;
        int number_outputs;
        int current_output;
        
        float total_bytes_sent;
        int idle_cycles;
        int busy_cycles;

        std::vector<request> *request_queue;
        std::vector<request> *buffer_served_request_queue;
    public:
        Router(Buffer *buffer, ComputeUnit *cu, float clock, float bandwidth);
        void Cycle();
        bool IsIdle();
        int CalculateBusyCycles();
        void ReceiveRequest(request req);
        void PrintStatistics();
};

#endif
