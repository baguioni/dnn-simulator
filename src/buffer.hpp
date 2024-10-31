#ifndef BUFFER_H
#define BUFFER_H

#include "common.hpp"

class Buffer {
    private:
        float capacity;
        // Buffer-Compute queues (Requested by router) 
        std::vector<request> *incoming_request_queue; // Data that is waiting to be sent to compute unit
        std::vector<request> *outgoing_request_queue; // Data that is was already sent to compute unit
        
        // Buffer-Dram queues (Requested by controller)
        std::vector<request> *waiting_request_queue; // Data that is waiting to be sent to buffer
        std::vector<request> *served_request_queue; // Data that is was already sent to buffer


    public:
        Buffer(float capacity);
        void Cycle();
        float GetCapacity();
        std::vector<request> *GetIncomingRequestQueue();
        std::vector<request> *GetOutgoingRequestQueue();
        std::vector<request> *GetWaitingRequestQueue();
        std::vector<request> *GetServedRequestQueue();
};

#endif