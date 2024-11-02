#include "buffer.hpp"
#include "dram.hpp"
#include "controller.hpp"
#include "interconnect.hpp"
#include "interconnect_router.hpp"

int main (void) {
    float clock = 0.7; // 700MHz
    float bandwidth_dram_buffer = 1 * (float)((1 << 30) / 1000000000); // 1GiB/s
    float buffer_size = (float)(3 * (1 << 22));
    int tile_width = 32;
    int tile_height = 32;

    Buffer *buffer = new Buffer(0, buffer_size);
    Interconnect *interconnect = new Interconnect(buffer, clock, bandwidth_dram_buffer);
    std::vector<Interconnect *> *interconnect_list = new std::vector<Interconnect *>();
    interconnect_list->push_back(interconnect);
    InterconnectRouter *router = new InterconnectRouter(interconnect_list);
    DRAM *dram = new DRAM("DDR4_2400R", clock, 1, 1, router);
    Controller *controller = new Controller(dram, router, tile_width, tile_height);

    std::cout << "Tile Size: " << tile_width << "x" << tile_height << std::endl;
    std::cout << "System Clock: " << clock << "GHz" << std::endl;
    std::cout << "DRAM Bandwidth: " << bandwidth_dram_buffer << "GiB/s" << std::endl;
    // Partition the input tensor into tiles
    controller->TilePartitioning(64, 64, true, 1, 0);
    std::cout<< "Tile Partitioning of Input Size 64x64" << std::endl;

    // Run the accelerator
    while (!(interconnect->IsIdle() && dram->IsIdle())) {
        dram->Cycle();
        interconnect->Cycle();
    }

    // Print statistics
    dram->PrintStatistics();
    interconnect->PrintStatistics();

    return 0;
}