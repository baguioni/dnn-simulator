#include "buffer.hpp"
#include "dram.hpp"
#include "controller.hpp"
#include "interconnect.hpp"

int main (void) {
    float clock = 0.7; // 700MHz
    float bandwidth_dram_buffer = 1 * (float)((1 << 30) / 1000000000); // 1GiB/s
    float buffer_size = (float)(3 * (1 << 22));
    int compute_unit_width = 32;
    int compute_unit_height = 32;


    DRAM *dram = new DRAM("DDR4_2400R", clock, 1, 1);
    Buffer *buffer = new Buffer(buffer_size);
    Controller *controller = new Controller(dram, buffer, compute_unit_width, compute_unit_height);
    Interconnect *interconnect = new Interconnect(buffer, dram, controller, clock, bandwidth_dram_buffer);

    // Partition the input tensor into tiles
    controller->TilePartitioning(64, 64, true, 1, 0);

    // Run the accelerator
    while (!controller->IsIdle() && !interconnect->IsIdle() && !dram->IsIdle()) {
        interconnect->Cycle();
        dram->Cycle();
    }

    // Print statistics
    dram->PrintStatistics();
    interconnect->PrintStatistics();

    return 0;
}