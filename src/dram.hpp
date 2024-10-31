#ifndef DRAM_H
#define DRAM_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include <cassert>
#include <iomanip>
#include <regex>
#include "common.hpp"
#include "interconnect_router.hpp"

class InterconnectRouter;

class DRAM {
    private:
        int channel, rank; // DRAM configuration
        int idle_cycle, busy_cycle, stall_cycle; // Statistics
        float total_fetched_data, frequency, accelerator_frequency; // in MHz
        InterconnectRouter *router;
        std::string name;
        std::vector<request> *request_queue; 
        // std::vector<tile> *weight_tile_queue; // Queue of weight tiles to be fetched
        std::vector<tile> *activation_tile_queue; // Queue of input tiles to be fetched

    public:
        DRAM(std::string name, float accelerator_frequency, int channel, int rank, InterconnectRouter *router);
        // DRAM functions
        void Cycle();
        bool IsIdle();
        void ReceiveRequest(request req);
        void ReceiveTile(tile t);
        int CalculateDRAMCycles();

        // Ramulator functions
        float GetFrequencyByName(std::string name);
        void CreateDRAMConfigFile(const std::string& name, int channel, int rank);
        int ParseRamulatorOutput(const std::string& fileName);
        void GenerateInstructionTrace(tile t);

        // std::vector<tile> *GetWeightTileQueue() {return weight_tile_queue;};
        std::vector<tile> *GetInputTileQueue() {return activation_tile_queue;};
        std::vector<request> *GetRequestQueue() {return request_queue;};
        void PrintStatistics();
};

#endif