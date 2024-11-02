#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "common.hpp"
#include "buffer.hpp"
#include "dram.hpp"
#include "interconnect.hpp"

class DRAM;
class InterconnectRouter;

class Controller {
    private:
        int tile_width;
        int tile_height;
        int order_id;
        int num_buffers;

        std::vector<tile> *activation_tile_queue;
        std::vector<request> *dram_request_queue;
        std::vector<request> *interconnect_request_queue;

    public:
        Controller(DRAM *dram, InterconnectRouter *router, int tile_width, int tile_height);
        void TilePartitioning(int H, int W, bool is_format_nchw, int channel, unsigned int starting_address);
        void PushTileRequest(tile t, int buffer_id);
};

#endif