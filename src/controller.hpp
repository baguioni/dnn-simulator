#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "common.hpp"
#include "buffer.hpp"
#include "dram.hpp"

class Controller {
    private:
        int compute_unit_width;
        int compute_unit_height;
        int order_id;

        std::vector<tile> *activation_tile_queue;
        std::vector<request> *request_queue;
        // std::vector<tile> *weight_tile_queue;

    public:
        Controller(DRAM *dram, Buffer *buffer, int compute_unit_width, int compute_unit_height);
        void TilePartitioning(int H, int W, bool is_format_nchw, int channel, unsigned int starting_address);
        void PushTileRequest(tile t);
        bool IsIdle();
        std::vector<request> *GetRequestQueue() {return request_queue;};
};


/*
    controller generates tiles and requests
        - tiles info go to dram
        - requests are stored in controller

    interconnect goes through controller request queue
        - sends all controller requests to dram
         

*/

#endif