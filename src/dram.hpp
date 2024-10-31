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

class DRAM {
    private:
        int channel, rank; // DRAM configuration
        int idle_cycle, busy_cycle, stall_cycle; // Statistics
        float total_fetched_data, frequency, accelerator_frequency; // in MHz
        std::string name;
        std::vector<request> *incoming_request_queue; // Queue of incoming memory requests, i.e. data that is being fetched
        std::vector<request> *outgoing_request_queue; // Queue of outgoing memory requests, i.e. data that is being sent
        // std::vector<tile> *weight_tile_queue; // Queue of weight tiles to be fetched
        std::vector<tile> *activation_tile_queue; // Queue of input tiles to be fetched

    public:
        DRAM(std::string name, float accelerator_frequency, int channel, int rank);
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

        std::vector<request> *GetIncomingRequestQueue() {return incoming_request_queue;};
        std::vector<request> *GetOutgoingRequestQueue() {return outgoing_request_queue;};
        // std::vector<tile> *GetWeightTileQueue() {return weight_tile_queue;};
        std::vector<tile> *GetInputTileQueue() {return activation_tile_queue;};

        void PrintStatistics();
};

#endif