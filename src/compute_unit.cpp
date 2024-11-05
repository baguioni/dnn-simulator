#include "compute_unit.hpp"

ComputeUnit::ComputeUnit(int id) {
    this->id = id;
    this->idle_cycles = 0;
    this->busy_cycles = 0;
    this->request_queue = new std::vector<request>;
}

void ComputeUnit::ReceiveRequest(request req) {
    request_queue->push_back(req);
}

bool ComputeUnit::IsIdle() {
    return request_queue->empty();
}

// Assume 3x3 PE array
void ComputeUnit::Cycle() {
    if (request_queue->empty()) {
        idle_cycles++;
    } else {
        pop_front(*request_queue);
        busy_cycles++;
    }
}

void ComputeUnit::PrintStatistics() {
    std::cout << "Compute Unit " << id << " statistics:" << std::endl;
    std::cout << "\tIdle cycles: " << idle_cycles << std::endl;
    std::cout << "\tBusy cycles: " << busy_cycles << std::endl;
    std::cout << "\tTotal cycles: " << idle_cycles + busy_cycles << std::endl;
    std::cout << "\tIdle percentage: " << (static_cast<float>(idle_cycles) / (idle_cycles + busy_cycles)) * 100 << "%" << std::endl;

}