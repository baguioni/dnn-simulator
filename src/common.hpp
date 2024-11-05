#ifndef COMMON_H
#define COMMON_H

#include <vector>
#include <cmath>
#include <iostream>

struct request {
    int buffer_id; // ID of the buffer
    int order; // Order of the tile
    float size; // Size of the tile (Height * Width)
};

struct tile {
    int order; // Order of the tile
    unsigned int starting_address; // Starting address in memory
    int jump_size; // Stride for accessing elements
    int tile_width; // Width of the tile
    int tile_height; // Height of the tile
    int total_width; // Total width of the original matrix
    int total_height; // Total height of the original matrix
};

request MakeRequest(int order, float size, int buffer_id);
tile MakeTile(int order, unsigned int starting_address, int jump_size, int tile_width, int tile_height, int total_width, int total_height);
void pop_front(std::vector<request> &v);

#endif