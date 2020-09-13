#include "main.h"

#define _TIMER_MULTI_THREADED 0
#define _BENCHMARK_INTERRUPT_TIME 0

void DispatchInterruptUpdate(int64_t i) { stdlog("DispatchInterruptUpdate:: " << i); }

int32_t inputEvaluation(std::map<std::string, DynamicObject*>& objectMap, std::string input) {
    std::string delimiter = "::";
    std::vector<std::string> inputWords = mgcp::SplitString(input, delimiter);
    mgcp::PrintVector(inputWords);

    std::string& firstInput = inputWords[0];
    auto iter = objectMap.find(firstInput);
    if (iter != objectMap.end()) {
        stdlog("found key: " << firstInput);
        if (inputWords.size() > 1) {
            stdlog(inputWords[1] << ":: invoked");
            auto t = iter->second;
            std::cout << "name: " << t->GetName() << '\n';
        }
    } else if (firstInput == "x" || firstInput == "exit") {
        return 0;
    }
    return 1;
}

// this should be threaded as it makes the main thread wait for it
int32_t interpretCommand(std::map<std::string, DynamicObject*>& objectMap) {
    std::string input;
    std::cout << "Type a command:: ";
    // this is a blocking statement
    std::getline(std::cin, input);
    stdlog("You inputted: " << input);
    return inputEvaluation(objectMap, input);
}

int main(int argc, char* argv[]) {
#if (NDEBUG)
    stdlog("preprocessor flag: NDEBUG is defined - therefore assertions are off");
#else
    stdlog("preprocessor flag: NDEBUG is not defined - therefore assertions are on");
#endif
    for (int i = 1; i < argc; ++i) {
        stdlog("Launched with command: " << argv[i]);
    }
    std::size_t numThreads = std::thread::hardware_concurrency();
    stdlog("System supports: " << numThreads << " threads.");

    CodeBlacksmith::ThreadPool threadPool{numThreads < 4 ? 4 : numThreads};
    spdlog::init_thread_pool(8192, 2);

    mgcp::FileHelper fileHelper(threadPool);
    mgcp::TimeManager timeManager;
    mgcp::Trader trader(threadPool, fileHelper, timeManager);

    mgcp::BinanceManager binanceManager(threadPool, fileHelper, timeManager);

    std::mutex mainEventMutex;
    std::condition_variable mainEventSystem;

#if (_TIMER_MULTI_THREADED)
    timeManager.Start();
#endif
    int64_t lastTimeStamp = mgcp::GetMicroTime();

    std::map<std::string, DynamicObject*> objectMap = {
        {"fh", (DynamicObject*)&fileHelper},  //
        {"tp", (DynamicObject*)&threadPool}   //
        //
    };
    mgcp::PrintMapKeys(objectMap);

    int32_t state = 1;

    mgcp::SocketHandlers socketInputHandlers(threadPool, timeManager, fileHelper);
    mgcp::FileInputHandlers fileInputHandler(threadPool, timeManager, fileHelper);

    std::map<std::string, std::function<void(std::string)>> inputHandlers = fileInputHandler.GetMap();

    // use a separate thread to read command line and push it to state
    std::thread io{[&] {
        std::string input;
        while (true) {
            std::getline(std::cin, input);
            {
                if (input == "t") {
                    const auto timestamp = mgcp::GetMicroTime();
                    binanceManager.Start();
                    // binanceApiHelper.SyncServerTime();
                    const auto finishedTime = mgcp::GetMicroTime() - timestamp;
                    std::cout << "t -> finishedTime: " << finishedTime << '\n';
                } else if (input == "i") {
                    const auto timestamp = mgcp::GetMicroTime();
                    // binanceApiHelper.HandleExchangeInfo([&](std::shared_ptr<Json::Value> data) {
                    //     //
                    // });
                    const auto finishedTime = mgcp::GetMicroTime() - timestamp;
                    std::cout << "t -> finishedTime: " << finishedTime << '\n';
                } else if (input == "p") {
                    // binanceApiHelper.LogPingSpeed();
                } else {
                    auto handler = inputHandlers.find(input);
                    if (handler != inputHandlers.end()) {
                        handler->second(input);
                    } else {
                        lastTimeStamp = mgcp::GetMicroTime();
                        std::lock_guard<std::mutex> lock{mainEventMutex};
                        state = inputEvaluation(objectMap, input);
                        mainEventSystem.notify_all();
                    }
                }
            }
        }
    }};
    io.detach();

    while (state == 1) {
        std::unique_lock<std::mutex> lock{mainEventMutex};
// OSX - 57% is 1micro, 25% - 10micro - 5% 100micro
#if (_BENCHMARK_INTERRUPT_TIME)
        const int64_t timer = 700000000;
        if (mainEventSystem.wait_for(lock, std::chrono::microseconds(timer)) != std::cv_status::timeout) {
            // interrupt detected - can get skipped if update is too fast
            DispatchInterruptUpdate(mgcp::TimeManager::GetMicroTime() - lastTimeStamp);
            lastTimeStamp = mgcp::TimeManager::GetMicroTime();
        }
#else
        const int32_t timer = 70;
        mainEventSystem.wait_for(lock, std::chrono::microseconds(timer));
#endif

#if !(_TIMER_MULTI_THREADED)
        timeManager.Update();
#endif
    }
    if (numThreads == 1) {
        timeManager.Stop();
    }
    return 0;
}
