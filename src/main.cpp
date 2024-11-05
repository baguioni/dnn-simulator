#include "buffer.hpp"
#include "dram.hpp"
#include "controller.hpp"
#include "interconnect.hpp"
#include "interconnect_router.hpp"
#include "compute_unit.hpp"
#include "router.hpp"

int main (void) {
    float clock = 0.7; // 700MHz
    float bandwidth_dram_buffer = 1 * (float)((1 << 30) / 1000000000); // 1GiB/s
    float buffer_size = (float)(3 * (1 << 22)); // 256 * 48KiB (12 * 2^20B) per buffer, 256 * 96KiB total (24MiB)
    int tile_width = 32;
    int tile_height = 32;

    Buffer *buffer0 = new Buffer(0, buffer_size);
    ComputeUnit *cu0 = new ComputeUnit(0);
    Interconnect *interconnect0 = new Interconnect(buffer0, clock, bandwidth_dram_buffer);
    Router *router0 = new Router(buffer0, cu0, clock, bandwidth_dram_buffer);

    Buffer *buffer1 = new Buffer(1, buffer_size);
    ComputeUnit *cu1 = new ComputeUnit(1);
    Interconnect *interconnect1 = new Interconnect(buffer1, clock, bandwidth_dram_buffer);
    Router *router1 = new Router(buffer1, cu1, clock, bandwidth_dram_buffer);

    std::vector<Interconnect *> *interconnect_list = new std::vector<Interconnect *>();
    std::vector<Router *> *router_list = new std::vector<Router *>();

    interconnect_list->push_back(interconnect0);
    interconnect_list->push_back(interconnect1);
    router_list->push_back(router0);
    router_list->push_back(router1);
    InterconnectRouter *interconnect_router = new InterconnectRouter(interconnect_list, router_list);
    DRAM *dram = new DRAM("DDR4_2400R", clock, 1, 1, interconnect_router);
    Controller *controller = new Controller(dram, interconnect_router, tile_width, tile_height);

    std::cout << "System Clock: " << clock << "GHz" << std::endl;
    std::cout << "DDR4-2400R Frequency: " << dram->GetFrequencyByName("DDR4_2400R") << "MHz" << std::endl;
    std::cout << "DRAM-Interconnect Bandwidth: " << bandwidth_dram_buffer << "GiB/s" << std::endl;
    std::cout << "Tile Size: " << tile_width << "x" << tile_height << std::endl;

    // Partition the input tensor into tiles
    unsigned int activation_starting_address = 0xe0000000;
    bool is_format_nchw = true;
    controller->TilePartitioning(64, 64, is_format_nchw, 1, activation_starting_address);
    std::cout<< "Partitioning Input size 64x64 into Tiles" << std::endl;
    std::cout << "----------------------------------------------------------------------" << std::endl;

    // Run the accelerator
    while (!(interconnect0->IsIdle() && dram->IsIdle() && router0->IsIdle() && interconnect1->IsIdle() && router1->IsIdle() && cu0->IsIdle() && cu1->IsIdle())) {
        dram->Cycle();
        interconnect0->Cycle();
        interconnect1->Cycle();
        router0->Cycle();
        router1->Cycle();
        cu0->Cycle();
        cu1->Cycle();
    }

    // Print statistics
    std::cout << "----------------------------------------------------------------------" << std::endl;
    dram->PrintStatistics();
    std::cout << "----------------------------------------------------------------------" << std::endl;
    std :: cout << "(0) Interconnect -> Buffer -> Router -> Compute Unit" << std :: endl;
    interconnect0->PrintStatistics();
    router0->PrintStatistics();
    cu0->PrintStatistics();
    std::cout << "----------------------------------------------------------------------" << std::endl;
    std :: cout << "(1) Interconnect -> Buffer -> Router -> Compute Unit" << std :: endl;
    interconnect1->PrintStatistics();
    router1->PrintStatistics();
    cu1->PrintStatistics();
    std::cout << "----------------------------------------------------------------------" << std::endl;

    return 0;
}