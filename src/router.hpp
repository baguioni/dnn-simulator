/*
    Sends data from buffer to a compute unit.
    This is where we can try different routing strategies.
    Each routing strategy should return how many cycles it took to prepare and send the data.
    1. 3x3 Sliding Window
*/

#include "common.hpp"
#include "buffer.hpp"
#include <cmath>
#include <iostream>

enum RoutingStrategy {
    SLIDING_WINDOW_3x3
};

class Router {
    private:
        Buffer *buffer;
        RoutingStrategy routing_strategy;
        int buffer_id;
        float clock;
        float bandwidth;
        float bytes_per_cycle;

        float request_total_bytes = 0.0;
        bool new_request = true;

        float total_bytes_sent;
        int idle_cycles;
        int busy_cycles;

        std::vector<request> *request_queue;
        std::vector<request> *buffer_served_request_queue;
    public:
        Router(Buffer *buffer, float clock, float bandwidth, RoutingStrategy routing_strategy);
        void Cycle();
        int CalculateBusyCycles();
        void PrintStatistics();
};