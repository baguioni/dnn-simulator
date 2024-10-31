#include "common.hpp"

request MakeRequest(int order, float size, int buffer_id) {
    request req;
    req.order = order;
    req.size = size;
    req.buffer_id = buffer_id;

    return req;
};

tile MakeTile(int order, unsigned int starting_address, int jump_size, int tile_width, int tile_height, int total_width, int total_height) {
    tile t;
    t.order             = order;
    t.starting_address  = starting_address;
    t.jump_size         = jump_size;
    t.tile_width        = tile_width;
    t.tile_height       = tile_height;
    t.total_width       = total_width;
    t.total_height      = total_height;

    return t;
}

void pop_front(std::vector<request> &vec) {
    if (!vec.empty()) {
        vec.erase(vec.begin());  // Erase the first element
    }
};

