#include "router.hpp"

Router::Router(Buffer *buffer, float clock, float bandwidth, RoutingStrategy routing_strategy) {
    this->buffer = buffer;
    this->routing_strategy = routing_strategy;
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

void Router::Cycle() {
    if (buffer_served_request_queue->empty() || request_queue->empty()) {
        idle_cycles++;
    } else {
        int num_cycles = 1;
        std::vector<request>::iterator bsrq_it;

        for (bsrq_it = buffer_served_request_queue->begin(); bsrq_it != buffer_served_request_queue->end(); ++bsrq_it) {
            if (request_queue->front().order == bsrq_it->order) {
                num_cycles = CalculateBusyCycles();
                break;
            }
        }

        busy_cycles += num_cycles;
    }
}

/*
    Each subfunction should:
        - Update total_bytes_sent
        - Pop the front of the request_queue
        - Return the number of cycles it took to prepare and send the data
*/
int Router::CalculateBusyCycles() {
    /*
        Output formula [(W−K+2P)/S]+1.

        W is the input volume - in your case 128
        K is the Kernel size - in your case 5
        P is the padding - in your case 0 i believe
        S is the stride - which you have not provided. I will assume it is 1
    */
    if (routing_strategy == SLIDING_WINDOW_3x3) {

        // Given an input tile and kernel size
        // Plus 1 to account for the last cycle
        const int PROCESSING_CYCLES = 9;
        request req = request_queue->front();
        int tile_size = (int) std::sqrt(req.size);
        int output_size = tile_size - 3 + 1;
        pop_front(*request_queue);
        return output_size * output_size * PROCESSING_CYCLES;
    } else {
        // Default strategy
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
        
            // Finished sending data to the buffer
            buffer_served_request_queue->push_back(MakeRequest(order, request_total_bytes, buffer_id));
        }
        return 1;
    }
}

void Router::PrintStatistics() {
    std::cout << "======================================================================" << std::endl;
    std::cout << "Buffer " << buffer_id <<" -> Router -> Compute Unit statistics:" << std::endl;
    std::cout << "  Idle cycles: " << idle_cycles << "  Busy cycles: " << busy_cycles << std::endl;
    std::cout << "  Total cycles: " << idle_cycles + busy_cycles << "  Idle percentage: " 
            << (static_cast<float>(idle_cycles) / (idle_cycles + busy_cycles)) * 100 << "%" << std::endl;
    std::cout << "  Total fetched data: " << total_bytes_sent << " bytes" << std::endl;
    std::cout << "======================================================================" << std::endl;
}