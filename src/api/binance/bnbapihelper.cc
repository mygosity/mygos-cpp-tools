#include "bnbapihelper.h"

// #define BINANCE_LOG_CLIENT_REQUEST_COUNT true;
#define BINANCE_THREAD_SAFE_CLIENT_REQUESTS true;

// https://github.com/binance-exchange/binance-official-api-docs
// https://github.com/binance-exchange/binance-official-api-docs/blob/master/rest-api.md
namespace mgcp {

const std::string BINANCE_BASE_URL = "https://api.binance.com";

// Content-Type: application/x-www-form-urlencoded

BinanceApiHelper::BinanceApiHelper() {
    for (size_t i = 0; i < MAX_EVENT_LOOPS; ++i) {
        binanceClients[i] = DrogonClient(drogon::HttpClient::newHttpClient(BINANCE_BASE_URL, apiEventLoop[i].getLoop()), i);
        apiEventLoop[i].run();
    }
    std::cout << "BinanceApiHelper:: sizeof(binanceClients):" << std::to_string(sizeof(binanceClients)) << "\n";

    publicRequestMap.at(PublicRequestType::Ping) = GenerateHttpRequest(drogon::Get, "/api/v3/ping");
    publicRequestMap.at(PublicRequestType::ServerTime) = GenerateHttpRequest(drogon::Get, "/api/v3/time");
    publicRequestMap.at(PublicRequestType::ExchangeInfo) = GenerateHttpRequest(drogon::Get, "/api/v3/exchangeInfo");
    publicRequestMap.at(PublicRequestType::Depth) = GenerateHttpRequest(drogon::Get, "/api/v3/depth");
    publicRequestMap.at(PublicRequestType::Trades) = GenerateHttpRequest(drogon::Get, "/api/v3/trades");
    publicRequestMap.at(PublicRequestType::HistoricalTrades) = GenerateHttpRequest(drogon::Get, "/api/v3/historicalTrades");
    publicRequestMap.at(PublicRequestType::AggTrades) = GenerateHttpRequest(drogon::Get, "/api/v3/aggTrades");
    publicRequestMap.at(PublicRequestType::Klines) = GenerateHttpRequest(drogon::Get, "/api/v3/klines");
    publicRequestMap.at(PublicRequestType::AvgPrice) = GenerateHttpRequest(drogon::Get, "/api/v3/avgPrice");
    publicRequestMap.at(PublicRequestType::Ticker24hr) = GenerateHttpRequest(drogon::Get, "/api/v3/ticker/24hr");
    publicRequestMap.at(PublicRequestType::TickerPrice) = GenerateHttpRequest(drogon::Get, "/api/v3/ticker/price");
    publicRequestMap.at(PublicRequestType::TickerBookTicker) = GenerateHttpRequest(drogon::Get, "/api/v3/ticker/bookTicker");

    privateRequestMap.at(PrivateRequestType::PostOrderTest) = GenerateHttpRequest(drogon::Post, "/api/v3/order/test");
    privateRequestMap.at(PrivateRequestType::GetOrder) = GenerateHttpRequest(drogon::Get, "/api/v3/order");
    privateRequestMap.at(PrivateRequestType::PostOrder) = GenerateHttpRequest(drogon::Post, "/api/v3/order");
    privateRequestMap.at(PrivateRequestType::DeleteOrder) = GenerateHttpRequest(drogon::Delete, "/api/v3/order");
    privateRequestMap.at(PrivateRequestType::GetAllOrders) = GenerateHttpRequest(drogon::Get, "/api/v3/allOrders");
    privateRequestMap.at(PrivateRequestType::GetOpenOrders) = GenerateHttpRequest(drogon::Get, "/api/v3/openOrders");
    privateRequestMap.at(PrivateRequestType::DeleteOpenOrders) = GenerateHttpRequest(drogon::Delete, "/api/v3/openOrders");
    privateRequestMap.at(PrivateRequestType::GetOneCancelsOtherOrder) = GenerateHttpRequest(drogon::Get, "/api/v3/order/orderList");
    privateRequestMap.at(PrivateRequestType::PostOneCancelsOtherOrder) = GenerateHttpRequest(drogon::Post, "/api/v3/order/oco");
    privateRequestMap.at(PrivateRequestType::DeleteOneCancelsOtherOrder) = GenerateHttpRequest(drogon::Delete, "/api/v3/order/orderList");
    privateRequestMap.at(PrivateRequestType::GetAllOneCancelsOtherOrders) = GenerateHttpRequest(drogon::Get, "/api/v3/allOrderList");
    privateRequestMap.at(PrivateRequestType::GetOpenOneCancelsOtherOrders) = GenerateHttpRequest(drogon::Get, "/api/v3/openOrderList");
    privateRequestMap.at(PrivateRequestType::GetAccountInfo) = GenerateHttpRequest(drogon::Get, "/api/v3/account");
    privateRequestMap.at(PrivateRequestType::GetAccountTradesList) = GenerateHttpRequest(drogon::Get, "/api/v3/myTrades");
    privateRequestMap.at(PrivateRequestType::PostUserDataStream) = GenerateHttpRequest(drogon::Post, "/api/v3/userDataStream");
    privateRequestMap.at(PrivateRequestType::PutUserDataStream) = GenerateHttpRequest(drogon::Put, "/api/v3/userDataStream");
    privateRequestMap.at(PrivateRequestType::DeleteUserDataStream) = GenerateHttpRequest(drogon::Delete, "/api/v3/userDataStream");

    // Create a file rotating logger with 5mb size max and 10000 rotated files
    auto fileLogger = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("_logs/binance/apihelper.log", 1048576 * 5, 10000);
    auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    std::vector<spdlog::sink_ptr> sinks{stdout_sink, fileLogger};
    logger = std::make_shared<spdlog::async_logger>("BinanceApiHelper", sinks.begin(), sinks.end(), spdlog::thread_pool(),
                                                    spdlog::async_overflow_policy::block);
    spdlog::register_logger(logger);
}

BinanceApiHelper::~BinanceApiHelper() {
    logger->flush();
    for (size_t i = 0; i < MAX_EVENT_LOOPS; ++i) {
        binanceClients[i].client->getLoop()->quit();
        binanceClients[i].client.reset();
    }
    publicRequestMap.fill(0);
    privateRequestMap.fill(0);
}

void BinanceApiHelper::SetWeightRateLimits(int64_t value, int64_t interval, std::string intervalType) {
    weightLimit = value;
    int64_t weightInterval = interval;
    if (strcmp("MINUTE", intervalType.c_str()) == 0) {
        weightInterval = 60000 * weightInterval;
    } else {
        logger->error("SetWeightRateLimits:: unexpected weight interval careful!");
        exit(1);
    }
    logger->info(std::string("SetWeightRateLimits:: limit: ")
                     .append(std::to_string(value))
                     .append(" interval: ")
                     .append(std::to_string(interval))
                     .append(" intervalType:")
                     .append(intervalType));
}

void BinanceApiHelper::SetOrderShortRateLimits(int64_t value, int64_t interval, std::string intervalType) {
    logger->info(std::string("SetOrderShortRateLimits:: limit: ")
                     .append(std::to_string(value))
                     .append(" interval: ")
                     .append(std::to_string(interval))
                     .append(" intervalType:")
                     .append(intervalType));
}

void BinanceApiHelper::SetOrderLongRateLimits(int64_t value, int64_t interval, std::string intervalType) {
    logger->info(std::string("SetOrderLongRateLimits:: limit: ")
                     .append(std::to_string(value))
                     .append(" interval: ")
                     .append(std::to_string(interval))
                     .append(" intervalType:")
                     .append(intervalType));
}

inline void BinanceApiHelper::OnClientRequestStarted(size_t index) {
#if defined(BINANCE_THREAD_SAFE_CLIENT_REQUESTS)
    std::unique_lock<std::mutex> lock{clientMutexes[index]};
#endif
    binanceClients[index].AddRequest();
}

inline void BinanceApiHelper::OnClientRequestCompleted(size_t index) {
#if defined(BINANCE_THREAD_SAFE_CLIENT_REQUESTS)
    std::unique_lock<std::mutex> lock{clientMutexes[index]};
#endif
    binanceClients[index].SubtractRequest();
}

DrogonClient& BinanceApiHelper::GetNextHttpClient() {
#if defined(BINANCE_THREAD_SAFE_CLIENT_REQUESTS)
    std::unique_lock<std::mutex> lock{clientAccessMutex};
#endif
    int32_t min = 255;
    int32_t minIndex = 0;
    for (size_t i = 0; i < MAX_EVENT_LOOPS; ++i) {
        if (binanceClients[i].requests == 0) {
            return binanceClients[i];
        }
        if (binanceClients[i].requests <= min) {
            min = binanceClients[i].requests;
            minIndex = i;
        }
    }
    return binanceClients[minIndex];
}

std::shared_ptr<drogon::HttpRequest> BinanceApiHelper::GenerateHttpRequest(drogon::HttpMethod method, const std::string path) {
    auto request = drogon::HttpRequest::newHttpRequest();
    request->setMethod(method);
    request->setPath(path);
    return request;
}

std::shared_ptr<rapidjson::Document> BinanceApiHelper::GetApiResponse(const std::shared_ptr<drogon::HttpRequest> request,
                                                                      const drogon::ReqResult result,
                                                                      const drogon::HttpResponsePtr& response) {
    if (result == drogon::ReqResult::Ok) {
        std::shared_ptr<rapidjson::Document> document = std::make_shared<rapidjson::Document>();
        document->Parse(response->getBody().data());
        if (document) {
            return document;
        } else {
            logger->error(std::string("GetApiResponse::notjson getPath: ")  //
                              .append(request->getPath())
                              .append(" getBody: ")
                              .append(response->getBody()));
        }
    } else {
        logger->error(std::string("GetApiResponse::error getPath: ")  //
                          .append(request->getPath())
                          .append(" getStatusCode: ")
                          .append(std::to_string(response->getStatusCode()))
                          .append(" getBody: ")
                          .append(response->getBody()));
    }
    return nullptr;
}

void BinanceApiHelper::LogPingSpeed(const std::function<void(std::shared_ptr<rapidjson::Document>)>& callback) {
    int64_t timeNow = GetTimeNow();
    const auto request = publicRequestMap.at(PublicRequestType::Ping);
    auto& drogonClient = GetNextHttpClient();
    OnClientRequestStarted(drogonClient.index);
    drogonClient.client->sendRequest(
        request, [=, index = drogonClient.index](drogon::ReqResult result, const drogon::HttpResponsePtr& response) {
            this->OnClientRequestCompleted(index);
            const auto data = GetApiResponse(request, result, response);
            if (data != nullptr) {
                int64_t timeTaken = GetTimeNow() - timeNow;
                this->lastReportedWeight = std::stoi(response->getHeader("x-mbx-used-weight"));
                this->lastApiTimeStamp = GetTimeNow();
                logger->info(std::string("LogPingSpeed: ").append(std::to_string(timeTaken)).append(" milliseconds"));
                logger->info(std::string("LogPingSpeed: completed | x-mbx-used-weight: ").append(response->getHeader("x-mbx-used-weight")));
                PrintDrogonClientStatus();
            }
            if (callback != nullptr) callback(data);
        });
}

void BinanceApiHelper::SyncServerTime(const std::function<void(std::shared_ptr<rapidjson::Document>)>& callback) {
    const auto request = publicRequestMap.at(PublicRequestType::ServerTime);
    auto& drogonClient = GetNextHttpClient();
    OnClientRequestStarted(drogonClient.index);
    drogonClient.client->sendRequest(request, [=, index = drogonClient.index](drogon::ReqResult result,
                                                                                    const drogon::HttpResponsePtr& response) {
        const auto data = GetApiResponse(request, result, response);
        if (data != nullptr) {
            this->OnClientRequestCompleted(index);
            const int64_t serverTime = (*data)["serverTime"].GetInt64();
            const int64_t serverDiff = GetTimeNow() - serverTime;
            timeAdjustment = -(serverDiff * 0.9);
            this->lastReportedWeight = std::stoi(response->getHeader("x-mbx-used-weight"));
            this->lastApiTimeStamp = GetTimeNow();
            logger->info(std::string("SyncServerTime::")
                             .append(std::to_string(serverTime))
                             .append(" serverDifference:")
                             .append(std::to_string(serverDiff))
                             .append(" timeAdjustment:")              //
                             .append(std::to_string(timeAdjustment))  //
            );
            logger->info(std::string("SyncServerTime: completed | x-mbx-used-weight: ").append(response->getHeader("x-mbx-used-weight")));
            PrintDrogonClientStatus();
        }
        if (callback != nullptr) callback(data);
    });
}

void BinanceApiHelper::HandleExchangeInfo(const std::function<void(std::shared_ptr<rapidjson::Document>)>& callback) {
    const auto request = publicRequestMap.at(PublicRequestType::ExchangeInfo);
    auto& drogonClient = GetNextHttpClient();
    OnClientRequestStarted(drogonClient.index);
    drogonClient.client->sendRequest(request, [=, index = drogonClient.index](drogon::ReqResult result,
                                                                                    const drogon::HttpResponsePtr& response) {
        this->OnClientRequestCompleted(index);
        const auto data = GetApiResponse(request, result, response);
        this->lastReportedWeight = std::stoi(response->getHeader("x-mbx-used-weight"));
        this->lastApiTimeStamp = GetTimeNow();
        // logger->info(std::string("HandleExchangeInfo: completed | ").append(this->GetHeadersAsString(response)));
        logger->info(std::string("HandleExchangeInfo: completed | x-mbx-used-weight: ").append(response->getHeader("x-mbx-used-weight")));
        PrintDrogonClientStatus();
        if (callback != nullptr) callback(data);
    });
}

std::string BinanceApiHelper::GetHeadersAsString(const drogon::HttpResponsePtr& response) {
    std::string headersAsString = "";
    const auto headers = response->headers();
    for (auto itr = headers.begin(); itr != headers.end(); ++itr) {
        headersAsString.append(itr->first).append(":").append(itr->second).append("\n");
    }
    return headersAsString;
}

inline void BinanceApiHelper::PrintDrogonClientStatus() {
#if defined(BINANCE_LOG_CLIENT_REQUEST_COUNT)
    for (size_t i = 0; i < MAX_EVENT_LOOPS; ++i) {
        int32_t reqs = binanceClients[i].requests;
        logger->info(
            std::string("PrintDrogonClientStatus:: i: ").append(std::to_string(i)).append(" requests: ").append(std::to_string(reqs)));
    }
#endif
}

}  // namespace mgcp
