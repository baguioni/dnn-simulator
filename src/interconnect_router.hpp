#ifndef DRAM_ROUTER_H
#define DRAM_ROUTER_H

#include "common.hpp"
#include "interconnect.hpp"
class Interconnect;

class InterconnectRouter {
    private:
        std::vector<Interconnect *> *interconnect_list;
    public:
        InterconnectRouter(std::vector<Interconnect *> *interconnect_list);
        void ReceiveRequest(request req);
        int NumberOfInterconnects();
};

#endif