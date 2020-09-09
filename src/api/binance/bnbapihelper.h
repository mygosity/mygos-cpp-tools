#ifndef MGCP_API_BINANCE_BNBAPIHELPER_H
#define MGCP_API_BINANCE_BNBAPIHELPER_H

// drogon
#include <drogon/HttpRequest.h>
#include <drogon/drogon.h>
#include <trantor/net/EventLoopThread.h>
#include <trantor/net/TcpClient.h>
// spdlog
#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
// rapidjson
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/pointer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

// std
#include <array>
#include <chrono>
#include <iostream>
#include <mutex>
#include <string>
#include <vector>

#include "../../lib/utils/time.h"

namespace mgcp {

constexpr int PUBLIC_REQUEST_CONTAINER_SIZE_START = __LINE__;
enum PublicRequestType : size_t {
    Ping,
    ServerTime,
    ExchangeInfo,
    Depth,
    Trades,
    HistoricalTrades,
    AggTrades,
    Klines,
    AvgPrice,
    Ticker24hr,
    TickerPrice,
    TickerBookTicker
};
constexpr int PUBLIC_DISTINCT_REQUEST_LENGTH = __LINE__ - PUBLIC_REQUEST_CONTAINER_SIZE_START - 3;

constexpr int PRIVATE_REQUEST_CONTAINER_SIZE_START = __LINE__;
enum PrivateRequestType : size_t {
    PostOrderTest,
    GetOrder,
    PostOrder,
    DeleteOrder,
    GetAllOrders,
    GetOpenOrders,
    DeleteOpenOrders,
    GetOneCancelsOtherOrder,
    PostOneCancelsOtherOrder,
    DeleteOneCancelsOtherOrder,
    GetAllOneCancelsOtherOrders,
    GetOpenOneCancelsOtherOrders,
    GetAccountInfo,
    GetAccountTradesList,
    PostUserDataStream,
    PutUserDataStream,
    DeleteUserDataStream
};
constexpr int PRIVATE_DISTINCT_REQUEST_LENGTH = __LINE__ - PRIVATE_REQUEST_CONTAINER_SIZE_START - 3;

const size_t MAX_EVENT_LOOPS = 16;

struct DrogonClient {
    std::shared_ptr<drogon::HttpClient> client;
    int8_t requests = 0;
    int8_t index = 0;

    DrogonClient(){};
    DrogonClient(std::shared_ptr<drogon::HttpClient> client, size_t index) : client(client), index(index){};
    ~DrogonClient() = default;

    void AddRequest() {
        requests += 1;
        // std::cout << "AddRequest:: requests: " << std::to_string(requests) << "\n";
    };
    void SubtractRequest() {
        requests -= 1;
        // std::cout << "SubtractRequest:: requests: " << std::to_string(requests) << "\n";
    };
};

class BinanceApiHelper {
   public:
    BinanceApiHelper();
    ~BinanceApiHelper();

    using HttpReqCallback = std::function<void(drogon::ReqResult, const drogon::HttpResponsePtr&)>;

    void LogPingSpeed(const std::function<void(std::shared_ptr<rapidjson::Document>)>& callback = nullptr);
    void SyncServerTime(const std::function<void(std::shared_ptr<rapidjson::Document>)>& callback = nullptr);
    void HandleExchangeInfo(const std::function<void(std::shared_ptr<rapidjson::Document>)>& callback = nullptr);

    void PrintDrogonClientStatus();

    void SetWeightRateLimits(int64_t value, int64_t interval, std::string intervalType);
    void SetOrderShortRateLimits(int64_t value, int64_t interval, std::string intervalType);
    void SetOrderLongRateLimits(int64_t value, int64_t interval, std::string intervalType);

   private:
    std::mutex clientAccessMutex;
    trantor::EventLoopThread apiEventLoop[MAX_EVENT_LOOPS];

    std::array<DrogonClient, MAX_EVENT_LOOPS> binanceClients;
    std::array<std::mutex, MAX_EVENT_LOOPS> clientMutexes;
    std::array<std::shared_ptr<drogon::HttpRequest>, PUBLIC_DISTINCT_REQUEST_LENGTH> publicRequestMap;
    std::array<std::shared_ptr<drogon::HttpRequest>, PRIVATE_DISTINCT_REQUEST_LENGTH> privateRequestMap;

    std::shared_ptr<spdlog::logger> logger;

    int64_t timeAdjustment = 0;
    int32_t lastReportedWeight = 0;
    int64_t lastApiTimeStamp = 0;

    int64_t weightLimit = 1200;
    int64_t weightInterval = 60000;

    void OnClientRequestStarted(size_t index);
    void OnClientRequestCompleted(size_t index);
    DrogonClient& GetNextHttpClient();
    std::shared_ptr<drogon::HttpRequest> GenerateHttpRequest(drogon::HttpMethod method, const std::string path);

    std::shared_ptr<rapidjson::Document> GetApiResponse(     //
        const std::shared_ptr<drogon::HttpRequest> request,  //
        drogon::ReqResult result,                            //
        const drogon::HttpResponsePtr& response              //
    );

    std::string GetHeadersAsString(const drogon::HttpResponsePtr& response);
};

}  // namespace mgcp

#endif