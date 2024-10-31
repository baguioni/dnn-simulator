#include "buffer.hpp"

// Buffer gets memory (weights or activation1) from DRAM and sends it to the accelerator
Buffer::Buffer(float capacity) {
    this->capacity = capacity;

    // Buffer-Router queues
    this->incoming_request_queue = new std::vector<request>; // Router adds requests to this queue
    this->outgoing_request_queue = new std::vector<request>; // Buffer sends requests to the Compute Unit

    // Dram-Buffer queues
    this->waiting_request_queue = new std::vector<request>; // Dram.incoming_request_queue
    this->served_request_queue = new std::vector<request>; // Dram.outgoing_request_queue
}

// void Buffer::Cycle() {
//     request req;
//     while (!served_queue->empty()) {
//         // holds requests that are processed and ready to be sent to the next stage
//         req = served_queue->front();
//         pop_front(*served_queue);
//         // queue of requests waiting to be sent to another unit 
//         sender_queue->push_back(MakeRequest(req.order, req.size));
//     }

// }

float Buffer::GetCapacity() {
    return capacity;
}

std::vector<request> *Buffer::GetIncomingRequestQueue() {
    return incoming_request_queue;
}

std::vector<request> *Buffer::GetOutgoingRequestQueue() {
    return outgoing_request_queue;
}

std::vector<request> *Buffer::GetWaitingRequestQueue() {
    return waiting_request_queue;
}

std::vector<request> *Buffer::GetServedRequestQueue() {
    return served_request_queue;
}
