#ifndef COMPUTEUNIT_H
#define COMPUTEUNIT_H

#include "common.hpp"

class ComputeUnit {
    private:
        int id;
        int idle_cycles;
        int busy_cycles;

        std::vector<request> *request_queue;

    public:
        ComputeUnit(int id);
        void ReceiveRequest(request req);
        void Cycle();
        bool IsIdle();
        void PrintStatistics();

};

#endif