#ifndef DRAM_ROUTER_H
#define DRAM_ROUTER_H

#include "common.hpp"
#include "interconnect.hpp"
#include "router.hpp"

class Router;
class Interconnect;

class InterconnectRouter {
    private:
        std::vector<Interconnect *> *interconnect_list;
        std::vector<Router *> *router_list;    
        
    public:
        InterconnectRouter(std::vector<Interconnect *> *interconnect_list, std::vector<Router *> *router_list);
        void ReceiveRequest(request req);
        int NumberOfInterconnects();
};

#endif