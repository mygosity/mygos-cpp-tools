#ifndef MGCP_API_BINANCE_BNBMANAGER_H
#define MGCP_API_BINANCE_BNBMANAGER_H

// 3rd party
#include <fmt/chrono.h>

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/pointer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
// spdlog
#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

// std
#include <future>
#include <iostream>
#include <map>
#include <string>

// lib
#include "../../lib/utils/filemanagement/filehelper.h"
#include "../../lib/utils/threadpool.h"
#include "../../lib/utils/time.h"
#include "../../lib/utils/timemanager.h"

//
#include "bnbapihelper.h"
#include "bnbconstants.h"
#include "bnbstructs.h"

namespace mgcp {

class BinanceManager {
   public:
    BinanceManager(CodeBlacksmith::ThreadPool& threadPool, mgcp::FileHelper& fileHelper, mgcp::TimeManager& timeManager);
    ~BinanceManager();

    void Start();

    // void InitializeOrderBooks(selectedOrderBookSymbolPairs?: BinanceSymbolPairs[])

   private:
    BinanceApiHelper* m_pApiHelper;

    std::shared_ptr<spdlog::logger> logger;

    CodeBlacksmith::ThreadPool& threadPool;
    mgcp::FileHelper& fileHelper;
    mgcp::TimeManager& timeManager;

    bool loadedMarkets = false;
    std::shared_ptr<rapidjson::Document> exchangeInfo;

    const std::vector<std::string> symbolList = {"BTCUSDT", "BNBUSDT", "ETHUSDT", "ETHBTC", "BNBBTC", "BNBETH"};
    std::map<std::string, SymbolPairMetadata> symbolPairMetadata;

    // indexed lookup by symbol string property
    std::map<std::string, std::string> pairmarkets;
    std::map<std::string, std::string> symbolMap;
    std::map<std::string, std::string> filterMap;
    std::map<std::string, std::string> orderBook;

    // void InitPrivateData();
    // void InitOpenOrders();
    // void InitBalance();
    // void InitOrderHistory();
    // void InitTransactionOrderHistory();
    // void InitOrderBookDepth();

    void LoadMarkets();
    void ConfigureServerInfo();
};

}  // namespace mgcp

#endif