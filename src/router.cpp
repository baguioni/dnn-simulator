#include "router.hpp"

// Simulate the routing of the input 3x3 sliding window
// Reads the 3x3 sliding window from the buffer and sends it to the compute unit 
Router::Router(Buffer *buffer, ComputeUnit *cu, float clock, float bandwidth) {
    this->buffer = buffer;
    this->cu = cu;
    this->buffer_id = buffer->GetId();
    this->clock = clock;
    this->bandwidth = bandwidth;
    this->bytes_per_cycle = bandwidth / clock;

    this->idle_cycles = 0;
    this->busy_cycles = 0;
    this->total_bytes_sent = 0;

    // Initialize state
    this->new_request = true;
    this->number_outputs = 0;
    this->current_output = 0;

    this->request_queue = new std::vector<request>;
    this->buffer_served_request_queue = buffer->GetServedRequestQueue();
}

void Router::ReceiveRequest(request req) {
    request_queue->push_back(req);
}

bool Router::IsIdle() {
    return request_queue->empty();
}

void Router::Cycle() {
    if (buffer_served_request_queue->empty() || request_queue->empty()) {
        idle_cycles++;
    } else {
        busy_cycles++;

        if (new_request) {
            // Reset state
            current_output = 0;
            number_outputs = 0;
            new_request = false;

            int size = request_queue->front().size;

            int tile_size = (int) std::sqrt(size);
            int output_size = tile_size - 3 + 1;

            number_outputs = output_size * output_size * 9;
            total_bytes_sent = number_outputs;
        }

        if (current_output == number_outputs) {
            pop_front(*request_queue);
            new_request = true;
            current_output = 0;
        } else {
            current_output++;
        }

        if (current_output % 9 == 0) {
            cu->ReceiveRequest(MakeRequest(request_queue->front().order, 9, buffer_id));
        }

    }
}

void Router::PrintStatistics() {
    std::cout << "Buffer " << buffer_id <<" -> Router -> Compute Unit statistics:" << std::endl;
    std::cout << "\tIdle cycles: " << idle_cycles << std::endl;
    std::cout << "\tBusy cycles: " << busy_cycles << std::endl;
    std::cout << "\tTotal cycles: " << idle_cycles + busy_cycles << std::endl;
    std::cout << "\tIdle percentage: " << (static_cast<float>(idle_cycles) / (idle_cycles + busy_cycles)) * 100 << "%" << std::endl;
    std::cout << "\tTotal fetched data: " << total_bytes_sent << " bytes" << std::endl;
}