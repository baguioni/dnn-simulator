#include "interconnect_router.hpp"

InterconnectRouter::InterconnectRouter(std::vector<Interconnect *> *interconnect_list, std::vector<Router *> *router_list) {
    this->interconnect_list = interconnect_list;
    this->router_list = router_list;
}

void InterconnectRouter::ReceiveRequest(request req) {
    // Send request to the appropriate interconnect
    interconnect_list->at(req.buffer_id)->ReceiveRequest(req);
    router_list->at(req.buffer_id)->ReceiveRequest(req);
}

int InterconnectRouter::NumberOfInterconnects() {
    return interconnect_list->size();
}