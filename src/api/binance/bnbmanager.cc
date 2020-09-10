#include "bnbmanager.h"

namespace mgcp {

BinanceManager::BinanceManager(CodeBlacksmith::ThreadPool& threadPool, mgcp::FileHelper& fileHelper, mgcp::TimeManager& timeManager)
    : threadPool(threadPool), fileHelper(fileHelper), timeManager(timeManager) {
    // Create a file rotating logger with 5mb size max and 10000 rotated files
    auto fileLogger = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("_logs/binance/manager.log", 1048576 * 5, 10000);
    auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    std::vector<spdlog::sink_ptr> sinks{stdout_sink, fileLogger};
    logger = std::make_shared<spdlog::async_logger>("BinanceManager", sinks.begin(), sinks.end(), spdlog::thread_pool(),
                                                    spdlog::async_overflow_policy::block);
    spdlog::register_logger(logger);

    m_pApiHelper = new BinanceApiHelper();
}

BinanceManager::~BinanceManager() {
    logger->flush();
    delete m_pApiHelper;
}

void BinanceManager::Start() {
    LoadMarkets();
    m_pApiHelper->SyncServerTime();
    m_pApiHelper->LogPingSpeed();
}

void BinanceManager::LoadMarkets() {
    const std::string date = fmt::format("{:%Y_%m_%d}/", fmt::localtime(std::time(nullptr)));
    const std::string directory = std::string("_logs/configs/binance/").append(date);
    const std::string fileName = "serverinfo.json";

    std::promise<std::shared_ptr<rapidjson::Document>> fileReadPromise;
    std::future<std::shared_ptr<rapidjson::Document>> futureFileRead = fileReadPromise.get_future();
    fileHelper.SafeJsonFileRead(directory, fileName, fileReadPromise);
    std::shared_ptr<rapidjson::Document> doc = futureFileRead.get();

    if (doc != nullptr && doc->HasMember("serverTime") && (GetTimeNow() - (*doc)["serverTime"].GetInt64()) < DAY_IN_MILLISECONDS) {
        logger->info(std::string("LoadMarkets:: valid serverConfig already exists - using it"));
        exchangeInfo = doc;
        ConfigureServerInfo();
    } else {
        logger->info(std::string("LoadMarkets:: valid serverConfig either doesnt exist or out of date - fetching and assigning a new one"));
        m_pApiHelper->HandleExchangeInfo([=](std::shared_ptr<rapidjson::Document> data) {
            if (data != nullptr) {
                fileHelper.WriteFile(directory, fileName, *data, GetWriteOnceOptions());
                exchangeInfo = data;
                ConfigureServerInfo();
            }
        });
    }
}

void BinanceManager::ConfigureServerInfo() {
    logger->info(std::string("ConfigureServerInfo:: started"));
    const auto& rateLimits = (*exchangeInfo)["rateLimits"].GetArray();
    for (size_t i = 0; i < rateLimits.Size(); ++i) {
        const auto& current = rateLimits[i];
        if (current.IsObject()) {
            if (current.GetObject().HasMember("rateLimitType")) {
                const std::string rateLimitType = current.GetObject()["rateLimitType"].GetString();
                const int64_t limit = current.GetObject()["limit"].GetInt64();
                const int64_t intervalNum = current.GetObject()["intervalNum"].GetInt64();
                const std::string intervalType = current.GetObject()["interval"].GetString();
                if (std::strcmp(rateLimitType.c_str(), "REQUEST_WEIGHT") == 0) {
                    m_pApiHelper->SetWeightRateLimits(limit, intervalNum, intervalType);
                } else if (std::strcmp(rateLimitType.c_str(), "ORDERS") == 0) {
                    if (std::strcmp(intervalType.c_str(), "SECOND") == 0) {
                        m_pApiHelper->SetOrderShortRateLimits(limit, intervalNum, intervalType);
                    } else {
                        m_pApiHelper->SetOrderLongRateLimits(limit, intervalNum, intervalType);
                    }
                }
            }
        }
    }

    const auto& symbols = (*exchangeInfo)["symbols"].GetArray();
    for (size_t i = 0; i < symbols.Size(); ++i) {
        auto& currentRoot = symbols[i];
        if (currentRoot.IsObject() && currentRoot.GetObject().HasMember("symbol")) {
            const rapidjson::Value::Object& currentTarget = currentRoot.GetObject();
            const std::string currentSymbol = currentTarget["symbol"].GetString();
            const auto found = std::find_if(symbolList.begin(), symbolList.end(),
                                            [&](const std::string itr) {  //
                                                return std::strcmp(itr.c_str(), currentSymbol.c_str()) == 0;
                                            });

            if (found != symbolList.end()) {
                const auto symbolPairData = symbolPairMetadata.find(currentSymbol);
                if (symbolPairData == symbolPairMetadata.end()) {
                    logger->info(std::string("symbol found: ").append(*found).append(" adding symbol metadata to map"));
                    const auto symbolData = SymbolPairMetadata(currentTarget);
                    symbolPairMetadata.insert({currentSymbol, symbolData});
                } else {
                    logger->info(std::string("symbol exists: ").append(*found).append(" updating symbol metadata in place"));
                    symbolPairData->second.Update(currentTarget);
                }
            }
        }
    }
}

}  // namespace mgcp