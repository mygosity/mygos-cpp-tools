#ifndef MGCP_CONTROLLER_TRADER_H
#define MGCP_CONTROLLER_TRADER_H

// std
#include <iostream>
#include <string>

// lib
#include "../api/binance/bnbmanager.h"
#include "../lib/utils/filemanagement/filehelper.h"
#include "../lib/utils/threadpool.h"
#include "../lib/utils/timemanager.h"

namespace mgcp {

class Trader {
   public:
    Trader(CodeBlacksmith::ThreadPool& threadPool, mgcp::FileHelper& fileHelper, mgcp::TimeManager& timeManager);
    ~Trader();

   private:
    CodeBlacksmith::ThreadPool& threadPool;
    mgcp::FileHelper& fileHelper;
    mgcp::TimeManager& timeManager;
};

}  // namespace mgcp

#endif