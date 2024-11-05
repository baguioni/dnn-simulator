#include "dram.hpp"

DRAM::DRAM(std::string name, float accelerator_frequency, int channel, int rank, InterconnectRouter *router) {
    // Initialize DRAM configuration
    this->name = name;
    this->frequency = GetFrequencyByName(name);
    this->accelerator_frequency = accelerator_frequency;
    this->channel = channel;
    this->rank = rank;
    this->router = router;

    // Initialize statistics
    this->idle_cycle = 0;
    this->busy_cycle = 0;
    this->stall_cycle = 0;
    this->total_fetched_data = 0;

    // Initialize queues
    this->request_queue = new std::vector<request>;
    // weight_tile_queue = new std::vector<tile>;
    this->activation_tile_queue = new std::vector<tile>;

    CreateDRAMConfigFile(name, channel, rank);
}

void DRAM::ReceiveRequest(request req) {
    request_queue->push_back(req);
}

void DRAM::ReceiveTile(tile t) {
    activation_tile_queue->push_back(t);
}

bool DRAM::IsIdle() {
    return (request_queue->empty());
}

// Check if we are pulling data from DRAM
void DRAM::Cycle() {
    // DRAM is not doing anything
    if (stall_cycle == 0 && request_queue->empty()) {
        idle_cycle++;
        return;
    }

    // Bringing data from DRAM
    if (stall_cycle == 0) {
        stall_cycle = CalculateDRAMCycles();

        // std::cout << stall_cycle << std::endl;
    }
    stall_cycle--;
    busy_cycle++;

    // DRAM has finished fetching data and ready to send
    if (stall_cycle == 0) {
        request req = request_queue->front();
        pop_front(*request_queue);
        router->ReceiveRequest(MakeRequest(req.order, req.size, req.buffer_id));
        total_fetched_data += (float)req.size;
    }
}

int DRAM::CalculateDRAMCycles() {
    // Find the tile of the next request
    int order = request_queue->front().order;
    tile found_tile;
    bool found = false;

    for (auto it = activation_tile_queue->begin(); it != activation_tile_queue->end(); ++it) {
        if (it->order == order) {
            found_tile = *it;
            found = true;
            break;
        }
    }

    if (!found) {
        std::cerr << "Error: Tile with order " << order << " not found." << std::endl;
        assert(0);
    }

    // Generate instruction trace for the tile
    GenerateInstructionTrace(found_tile);

    // Run Ramulator with the generated instruction trace
    std::string order_string = std::to_string(order);
    std::string cmd_line = "../ramulator/ramulator dram_config.cfg --mode=dram --stats ./ramulator_output/"
               + order_string + ".output.txt ./traces/" + order_string + ".trace";
    system(cmd_line.c_str());

    // Parse the output file of Ramulator to get the number of DRAM cycles
    std::string file_name = "./ramulator_output/" + order_string + ".output.txt";
    int ramulator_dram_cycles = ParseRamulatorOutput(file_name);

    // now calculate accelerators's dram_cycles

    // std::cout << "ramulator_dram_cycles: " << ramulator_dram_cycles << std::endl;
    // std::cout << "accelerator_frequency: " << accelerator_frequency << std::endl;
    // std::cout << "frequency: " << frequency << std::endl;

    return (int)((ramulator_dram_cycles * accelerator_frequency * (double)1000 - 1) / frequency) + 1;
}

int DRAM::ParseRamulatorOutput(const std::string& file_name){
    std::ifstream fileInput(file_name);
    if (!fileInput.is_open()) {
        std::cerr << "Error: Could not open file " << file_name << std::endl;
        return -1;
    }

    std::string line;
    while (getline(fileInput, line)) {
        if (line.find("ramulator.dram_cycles") != std::string::npos) {
            std::regex number_regex("\\d+");
            std::smatch match;
            if (std::regex_search(line, match, number_regex)) {
                return std::stoi(match.str());
            }
        }
    }

    std::cerr << "Error: 'ramulator.dram_cycles' not found in file " << file_name << std::endl;
    return -1;
}

void DRAM::GenerateInstructionTrace(tile t) {
    std::string fileName = "traces/" + std::to_string(t.order) + ".trace";
    
    std::unordered_set<unsigned int> addrSet; // Set of unique aligned addresses, for searching
    std::vector<unsigned int> addrList;  // Store aligned addresses

    // Generate list of aligned addresses
    for (int h = 0; h < t.tile_height; ++h) {
        for (int w = 0; w < t.tile_width; ++w) {
            unsigned int actual_address = t.starting_address + t.jump_size * ((h * t.total_width) + w);
            unsigned int request_address = (actual_address >> 6) << 6;  // 6-bit alignment

            // If address is unique, add to list
            if (addrSet.find(request_address) == addrSet.end()) {
                addrSet.insert(request_address);
                addrList.push_back(request_address);
            }
        }
    }

    std::ofstream outFile(fileName);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open file: " << fileName << std::endl;
        exit(EXIT_FAILURE);
    }

    // Write the aligned addresses to the file in hexadecimal format
    // Convert to hexadecimal string with leading zeros (8 digits)
    for (size_t order = 0; order < addrList.size(); ++order) {
        std::stringstream ss;
        ss << "0x" << std::setw(8) << std::setfill('0') << std::hex << addrList[order];

        // Write the formatted address with 'R' for read
        outFile << ss.str() << " R\n";
    }

    outFile.close();
    // std::cout << "Instruction trace file \"" << fileName << "\" generated successfully." << std::endl;
}

void DRAM::CreateDRAMConfigFile(const std::string& name, int channel, int rank) {
    std::string standard;
    std::string org;

    if (name.find("DDR3") != std::string::npos) {
        standard = "DDR3";
        org = "DDR3_2Gb_x8";
    } else if (name.find("DDR4") != std::string::npos) {
        standard = "DDR4";
        org = "DDR4_4Gb_x8";
    } else if (name.find("HBM") != std::string::npos) {
        standard = "HBM";
        org = "HBM_4Gb";
    } else {
        std::cerr << "Error: DRAM type should be one of: DDR3, DDR4, HBM" << std::endl;
        return;
    }

    const std::string fileName = "../dram_config.cfg";
    std::ofstream configFile(fileName);

    if (!configFile.is_open()) {
        std::cerr << "Error: Could not create the file " << fileName << std::endl;
        return;
    }

    const std::string fileContent = 
        "standard = " + standard + "\n" +
        "channels = " + std::to_string(channel) + "\n" +
        "ranks = " + std::to_string(rank) + "\n" +
        "speed = " + name + "\n" +
        "org = " + org + "\n" +
        "record_cmd_trace = off\n"
        "print_cmd_trace = off\n"
        "cpu_tick = 8\n"
        "mem_tick = 3\n"
        "early_exit = on\n"
        "expected_limit_insts = 200000000\n"
        "warmup_insts = 0\n"
        "cache = no\n"
        "translation = None\n";

    configFile << fileContent;
    configFile.close();
}

// Returns frequency in MHz
float DRAM::GetFrequencyByName(std::string name) {
    // DDR3 frequencies
    if (name == "DDR3_800D" || name == "DDR3_800E") return (400.0 / 3) * 3;
    else if (name == "DDR3_1066E" || name == "DDR3_1066F" || name == "DDR3_1066G") return (400.0 / 3) * 4;
    else if (name == "DDR3_1333G" || name == "DDR3_1333H") return (400.0 / 3) * 5;
    else if (name == "DDR3_1600H" || name == "DDR3_1600J" || name == "DDR3_1600K") return (400.0 / 3) * 6;
    else if (name == "DDR3_1866K" || name == "DDR3_1866L") return (400.0 / 3) * 7;
    else if (name == "DDR3_2133L" || name == "DDR3_2133M") return (400.0 / 3) * 8;

    // DDR4 frequencies
    else if (name == "DDR4_1600K" || name == "DDR4_1600L") return (400.0 / 3) * 6;
    else if (name == "DDR4_1866M" || name == "DDR4_1866N") return (400.0 / 3) * 7;
    else if (name == "DDR4_2133P" || name == "DDR4_2133R") return (400.0 / 3) * 8;
    else if (name == "DDR4_2400R" || name == "DDR4_2400U") return (400.0 / 3) * 9;
    else if (name == "DDR4_3200") return 1600.0;

    // Invalid DRAM name
    std::cerr << "*** Invalid DRAM name: " << name << " ***" << std::endl;
    assert(0);
    return 0.0; // This line is unreachable due to the assertion
}

void DRAM::PrintStatistics() {
    std::cout << "DRAM statistics:" << std::endl;
    std::cout << "\tIdle cycles: " << idle_cycle << std::endl;
    std::cout << "\tBusy cycles: " << busy_cycle << std::endl;
    std::cout << "\tTotal cycles: " << idle_cycle + busy_cycle << std::endl;
    std::cout << "\tIdle percentage: " << (static_cast<float>(idle_cycle) / (idle_cycle + busy_cycle)) * 100 << "%" << std::endl;
    std::cout << "\tTotal fetched data: " << total_fetched_data << " bytes" << std::endl;

}