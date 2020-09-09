#ifndef MGCP_API_BINANCE_BNBCONSTANTS_H
#define MGCP_API_BINANCE_BNBCONSTANTS_H

#include "string"

namespace mgcp {

struct BinanceSymbol {
    const std::string BITCOIN = "BTC";
    const std::string ETHEREUM = "ETH";
    const std::string LITECOIN = "LTC";
    const std::string BINANCECOIN = "BNB";

    const std::string TETHER = "USDT";
    const std::string STABLE_USD = "USDS";
    const std::string USDCOIN = "USDC";
    const std::string TRUE_USD = "TUSD";
    const std::string PAX = "PAX";

    // pairs
    const std::string BTCUSDT = "BTCUSDT";

    const std::string BNBUSDT = "BNBUSDT";  // "tickSize": "0.00010000"
    const std::string BNBBTC = "BNBBTC";    // "stepSize": "0.01000000" "tickSize": "0.00000010" "minNotional": "0.00010000"
    const std::string BNBETH = "BNBETH";

    const std::string ETHUSDT = "ETHUSDT";  // "stepSize": "0.00001000" "minNotional": "10.00000000"
    const std::string ETHBTC = "ETHBTC";    // "stepSize": "0.00100000" "tickSize": "0.00000100" "minNotional": "0.00010000"

    const std::string LTCUSDT = "LTCUSDT";  // "stepSize": "0.00001000" "minNotional": "10.00000000"
    const std::string LTCBTC = "LTCBTC";    // "stepSize": "0.01000000" "tickSize": "0.00000100" "minNotional": "0.00010000"
    const std::string LTCBNB = "LTCBNB";

    const std::string MATICUSDT = "MATICUSDT";
    const std::string MATICBTC = "MATICBTC";
    const std::string MATICBNB = "MATICBNB";

    const std::string BTTUSDT = "BTTUSDT";
    const std::string BTTBNB = "BTTBNB";
    const std::string BTTTRX = "BTTTRX";

    const std::string TRXUSDT = "TRXUSDT";
    const std::string TRXBTC = "TRXBTC";
    const std::string TRXBNB = "TRXBNB";

    const std::string TOMOUSDT = "TOMOUSDT";
    const std::string TOMOBTC = "TOMOBTC";
    const std::string TOMOBNB = "TOMOBNB";
};

}  // namespace mgcp

#endif
