#ifndef INTERCONNECT_H
#define INTERCONNECT_H

#include "buffer.hpp"
#include "dram.hpp"
#include "common.hpp"
#include "controller.hpp"
#include "buffer.hpp"
#include <iostream>

class Interconnect {
    private:
        // properties
        int buffer_id;
        float clock;
        float bandwidth;
        float bytes_per_cycle;

        float request_total_bytes = 0.0;
        bool new_request = true;

        // stats
        float total_bytes_sent;
        int idle_cycles;
        int busy_cycles;

        std::vector<request> *request_queue;
        std::vector<request> *dram_incoming_request_queue;     
        std::vector<request> *buffer_served_request_queue;
        

    public:
        Interconnect(Buffer *buffer, float clock, float bandwidth);
        void Cycle();
        void ReceiveRequest(request req);
        bool IsIdle();
        void PrintStatistics();      
};

#endif