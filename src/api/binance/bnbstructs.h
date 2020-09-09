#include <string>

namespace mgcp {

struct SymbolPairMetadata {
    SymbolPairMetadata(const rapidjson::Value::Object& obj) { Update(obj); }
    ~SymbolPairMetadata() = default;

    void Update(const rapidjson::Value::Object& obj) {
        symbol = obj["symbol"].GetString();
        baseAsset = obj["baseAsset"].GetString();
        baseAssetPrecision = obj["baseAssetPrecision"].GetUint();
        quoteAsset = obj["quoteAsset"].GetString();
        quoteAssetPrecision = obj["quoteAssetPrecision"].GetUint();

        const auto& filters = obj["filters"].GetArray();
        for (size_t i = 0; i < filters.Size(); ++i) {
            const auto& c = filters[i].GetObject();
            const std::string filterType = c["filterType"].GetString();

            if (std::strcmp(filterType.c_str(), "PRICE_FILTER") == 0) {
                if (c.HasMember("minPrice")) {
                    minPrice = std::stod(c["minPrice"].GetString());
                }
                if (c.HasMember("maxPrice")) {
                    maxPrice = std::stod(c["maxPrice"].GetString());
                }
                if (c.HasMember("tickSize")) {
                    tickSize = std::stod(c["tickSize"].GetString());
                }
            }

            if (std::strcmp(filterType.c_str(), "LOT_SIZE") == 0) {
                if (c.HasMember("minQty")) {
                    minQty = std::stod(c["minQty"].GetString());
                }
                if (c.HasMember("maxQty")) {
                    maxQty = std::stod(c["maxQty"].GetString());
                }
                if (c.HasMember("stepSize")) {
                    stepSize = std::stod(c["stepSize"].GetString());
                }
            }

            if (std::strcmp(filterType.c_str(), "MIN_NOTIONAL") == 0) {
                if (c.HasMember("minNotional")) {
                    minNotional = std::stod(c["minNotional"].GetString());
                }
            }
        }
    }

    void PrintSelf() {
        std::cout << "\nSymbolPairMetadata::PrintSelf:: symbol: " << symbol << '\n';
        std::cout << "baseAsset: " << baseAsset << " baseAssetPrecision: " << std::to_string(baseAssetPrecision) << '\n';
        std::cout << "quoteAsset: " << quoteAsset << " quoteAssetPrecision: " << std::to_string(quoteAssetPrecision) << '\n';
        std::cout << "minPrice: " << std::to_string(minPrice) << " maxPrice: " << std::to_string(maxPrice)
                  << " tickSize: " << std::to_string(tickSize) << '\n';
        std::cout << "minQty: " << std::to_string(minQty) << " maxQty: " << std::to_string(maxQty)
                  << " stepSize: " << std::to_string(stepSize) << '\n';
        std::cout << "minNotional: " << std::to_string(minNotional) << " end of symbol metadata\n" << '\n';
    }

    std::string symbol;
    std::string baseAsset;
    uint8_t baseAssetPrecision;
    std::string quoteAsset;
    uint8_t quoteAssetPrecision;
    double minPrice;     // PRICE_FILTER
    double maxPrice;     // PRICE_FILTER
    double tickSize;     // PRICE_FILTER
    double minQty;       // LOT_SIZE
    double maxQty;       // LOT_SIZE
    double stepSize;     // LOT_SIZE
    double minNotional;  // MIN_NOTIONAL
};

}  // namespace mgcp

//      "symbol": "BTCUSDT",
//       "status": "TRADING",
//       "baseAsset": "BTC",
//       "baseAssetPrecision": 8,
//       "quoteAsset": "USDT",
//       "quotePrecision": 8,
//       "quoteAssetPrecision": 8,
//       "baseCommissionPrecision": 8,
//       "quoteCommissionPrecision": 8,
//       "orderTypes": [
//         "LIMIT",
//         "LIMIT_MAKER",
//         "MARKET",
//         "STOP_LOSS_LIMIT",
//         "TAKE_PROFIT_LIMIT"
//       ],
//       "icebergAllowed": true,
//       "ocoAllowed": true,
//       "quoteOrderQtyMarketAllowed": true,
//       "isSpotTradingAllowed": true,
//       "isMarginTradingAllowed": true,
// "filters": [
//     {
//       "filterType": "PRICE_FILTER",
//       "minPrice": "0.01000000",
//       "maxPrice": "1000000.00000000",
//       "tickSize": "0.01000000"
//     },
//     {
//       "filterType": "PERCENT_PRICE",
//       "multiplierUp": "5",
//       "multiplierDown": "0.2",
//       "avgPriceMins": 5
//     },
//     {
//       "filterType": "LOT_SIZE",
//       "minQty": "0.00000100",
//       "maxQty": "9000.00000000",
//       "stepSize": "0.00000100"
//     },
//     {
//       "filterType": "MIN_NOTIONAL",
//       "minNotional": "10.00000000",
//       "applyToMarket": true,
//       "avgPriceMins": 5
//     },
//     { "filterType": "ICEBERG_PARTS", "limit": 10 },
//     {
//       "filterType": "MARKET_LOT_SIZE",
//       "minQty": "0.00000000",
//       "maxQty": "397.19211050",
//       "stepSize": "0.00000000"
//     },
//     { "filterType": "MAX_NUM_ALGO_ORDERS", "maxNumAlgoOrders": 5 },
//     { "filterType": "MAX_NUM_ORDERS", "maxNumOrders": 200 }
//   ],