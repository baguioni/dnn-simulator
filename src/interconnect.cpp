#include "interconnect.hpp"

Interconnect::Interconnect(Buffer *buffer, float clock, float bandwidth) {
    this->buffer_id = buffer->GetId();
    this->clock = clock;
    this->bandwidth = bandwidth;
    this->bytes_per_cycle = bandwidth / clock;

    this->idle_cycles = 0;
    this->busy_cycles = 0;
    this->total_bytes_sent = 0;

    this->request_queue = new std::vector<request>;
    this->buffer_served_request_queue = buffer->GetServedRequestQueue();

}

void Interconnect::Cycle() {
    if (request_queue->empty()) {
        idle_cycles++;
    } else {
        busy_cycles++;

        int order = request_queue->front().order;
        float bytes_to_send = request_queue->front().size;

        // DRAM has a new request
        // Take note of the total bytes to send for this request
        if (new_request) {
            request_total_bytes = bytes_to_send;
            new_request = false;
            total_bytes_sent += request_total_bytes;
        }

        // ensures bytes_to_send is not negative
        bytes_to_send = ((bytes_to_send - bytes_per_cycle) < 0) ? 0 : (bytes_to_send - bytes_per_cycle);

        if (bytes_to_send != 0) {
            // Update the size of the request to reflect remaining bytes to send
            request_queue->front().size = bytes_to_send;
        } else {
            // DRAM has finished sending data through the interconnect
            pop_front(*request_queue);
            request_total_bytes = 0.0;
            new_request = true;
            

            // Send the request to the buffer
            buffer_served_request_queue->push_back(MakeRequest(order, request_total_bytes, buffer_id));
        }
    }
}

void Interconnect::ReceiveRequest(request req) {
    request_queue->push_back(req);
}

bool Interconnect::IsIdle() {
    return (request_queue->empty());
}

void Interconnect::PrintStatistics() {
    std::cout << "======================================================================" << std::endl;
    std::cout << "Buffer " << buffer_id <<" Interconnect statistics:" << std::endl;
    std::cout << "  Idle cycles: " << idle_cycles << "  Busy cycles: " << busy_cycles << std::endl;
    std::cout << "  Total cycles: " << idle_cycles + busy_cycles << "  Idle percentage: " 
            << (static_cast<float>(idle_cycles) / (idle_cycles + busy_cycles)) * 100 << "%" << std::endl;
    std::cout << "  Total fetched data: " << total_bytes_sent << " bytes" << std::endl;
    std::cout << "======================================================================" << std::endl;
}