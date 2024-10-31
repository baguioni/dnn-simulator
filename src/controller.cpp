#include "controller.hpp"

Controller::Controller(DRAM *dram, Buffer *buffer, int compute_unit_width, int compute_unit_height) {
    dram = dram;
    buffer = buffer;

    this->compute_unit_width = compute_unit_width;
    this->compute_unit_height = compute_unit_height;
    this->order_id = 0;

    activation_tile_queue = dram->GetInputTileQueue();
    request_queue = new std::vector<request>;
    // weight_tile_queue = new std::vector<tile>;
}

void Controller::TilePartitioning(int H, int W, bool is_format_nchw, int channel, unsigned int starting_address) {
    unsigned int tile_start_address;
    int tile_width, tile_height;
    int num_h_tiles = int((H-1)/compute_unit_height) + 1;
    int num_w_tiles = int((W-1)/compute_unit_width) + 1;

    int h_tile_remainder = H - ((num_h_tiles - 1) * compute_unit_height);
    int w_tile_remainder = W - ((num_w_tiles - 1) * compute_unit_width);

    int multiply_factor = (is_format_nchw) ? 1 : channel;

    for (int hh = 0; hh < num_h_tiles; hh++) {
        for (int ww = 0; ww < num_w_tiles; ww++) {
            // Activation Tile
            tile_start_address = starting_address + multiply_factor * ((hh * compute_unit_height * W) + (ww * compute_unit_width));
            tile_width = (ww == num_w_tiles - 1) ? w_tile_remainder : compute_unit_width;
            tile_height = (hh == num_h_tiles - 1) ? h_tile_remainder : compute_unit_height;

            tile activation_tile = MakeTile(order_id, tile_start_address, multiply_factor, tile_width, tile_height, W, H);
            activation_tile_queue->push_back(activation_tile);
            PushTileRequest(activation_tile);
            order_id++;
        }
    }


}

void Controller::PushTileRequest(tile t) {
    request req = MakeRequest(t.order, t.tile_width * t.tile_height);
    request_queue->push_back(req);
}

bool Controller::IsIdle() {
    return (request_queue->empty());
}