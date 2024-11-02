#include "controller.hpp"

Controller::Controller(DRAM *dram, InterconnectRouter *router, int tile_width, int tile_height) {
    this->tile_width = tile_width;
    this->tile_height = tile_height;
    this->order_id = 0;
    this->num_buffers = router->NumberOfInterconnects();

    activation_tile_queue = dram->GetInputTileQueue();
    dram_request_queue = dram->GetRequestQueue();
}

// Partition the input tensor into tiles
void Controller::TilePartitioning(int H, int W, bool is_format_nchw, int channel, unsigned int starting_address) {
    int current_buffer = 0;
    unsigned int tile_start_address;
    int tile_width, tile_height;
    int num_h_tiles = int((H-1)/tile_height) + 1;
    int num_w_tiles = int((W-1)/tile_width) + 1;

    int h_tile_remainder = H - ((num_h_tiles - 1) * tile_height);
    int w_tile_remainder = W - ((num_w_tiles - 1) * tile_width);

    int multiply_factor = (is_format_nchw) ? 1 : channel;

    for (int hh = 0; hh < num_h_tiles; hh++) {
        for (int ww = 0; ww < num_w_tiles; ww++) {
            // Activation Tile
            tile_start_address = starting_address + multiply_factor * ((hh * tile_height * W) + (ww * tile_width));
            tile_width = (ww == num_w_tiles - 1) ? w_tile_remainder : tile_width;
            tile_height = (hh == num_h_tiles - 1) ? h_tile_remainder : tile_height;

            // Send tile information to DRAM
            tile activation_tile = MakeTile(order_id, tile_start_address, multiply_factor, tile_width, tile_height, W, H);
            activation_tile_queue->push_back(activation_tile);
            PushTileRequest(activation_tile, current_buffer);
            order_id++;
        }

        // Switch to the next buffer if there are more tiles to process
        // Otherwise, reset the buffer index
        if (current_buffer < num_buffers - 1) {
            current_buffer++;
        } else {
            current_buffer = 0;
        }
    }


}

// Send request to DRAM
void Controller::PushTileRequest(tile t, int buffer_id) {
    request req = MakeRequest(t.order, t.tile_width * t.tile_height, buffer_id);
    dram_request_queue->push_back(req);
}