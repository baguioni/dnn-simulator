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
        Buffer *buffer;
        DRAM *dram;

        // properties
        float clock;
        float bandwidth;
        float bytes_per_cycle;

        // stats
        float total_bytes_sent;
        int idle_cycles;
        int busy_cycles;

        // DRAM queues
        std::vector<request> *dram_incoming_request_queue;     
        
        // Buffer queues
        std::vector<request> *buffer_served_request_queue;     // vector of requests that the receiver has been serviced
        std::vector<request> *buffer_waiting_request_queue;    // vector of requests that the receiver has to receive from sender
        
        // Controller queues
        std::vector<request> *controller_request_queue;

    public:
        Interconnect(Buffer *buffer, DRAM *dram, Controller *controller, float clock, float bandwidth);
        void Cycle();
        bool IsIdle();
        void PrintStatistics();
        void SendRequestToDRAM(request req);
};

#endif