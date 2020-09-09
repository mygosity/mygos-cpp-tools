#include "trader.h"

namespace mgcp {

Trader::Trader(CodeBlacksmith::ThreadPool& threadPool, mgcp::FileHelper& fileHelper, mgcp::TimeManager& timeManager)
    : threadPool(threadPool), fileHelper(fileHelper), timeManager(timeManager) {
    //
}

Trader::~Trader() {
    //
}

}  // namespace mgcp