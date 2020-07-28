#include "main.h"

#define _TIMER_MULTI_THREADED 0
#define _BENCHMARK_INTERRUPT_TIME 0

void DisplayIntroductionMessage()
{
	spdlog::info("Using SpeedLog!");
	fmt::print("Hello, world!\n");
	std::cout << "***********************************************"
			  << "\n";
	std::cout << "********* mygos-cpp-tools has started *********"
			  << "\n";
	std::cout << "***********************************************"
			  << "\n";
}

int32_t inputEvaluation(std::map<std::string, DynamicObject *> &objectMap, std::string input)
{
	std::string delimiter = "::";
	std::vector<std::string> *inputWords = mgcp::SplitString(input, delimiter);
	mgcp::PrintVector(*inputWords);

	std::string &firstInput = inputWords->at(0);
	auto iter = objectMap.find(firstInput);
	if (iter != objectMap.end())
	{
		stdlog("found key: " << firstInput);
		if (inputWords->size() > 1)
		{
			stdlog(inputWords->at(1) << ":: invoked");
			auto t = iter->second;
			std::cout << "name: " << t->GetName() << '\n';
		}
	}
	else if (firstInput == "x" || firstInput == "exit")
	{
		return 0;
	}
	return 1;
}

//this should be threaded as it makes the main thread wait for it
int32_t interpretCommand(std::map<std::string, DynamicObject *> &objectMap)
{
	std::string input;
	std::cout << "Type a command:: ";
	//this is a blocking statement
	std::getline(std::cin, input);
	stdlog("You inputted: " << input);
	return inputEvaluation(objectMap, input);
}

int32_t main(int32_t argc, char *argv[])
{
	std::size_t numThreads = std::thread::hardware_concurrency();
	stdlog("System supports: " << numThreads << " threads.");

	CodeBlacksmith::ThreadPool threadPool{numThreads < 4 ? 4 : numThreads};

	mgcp::FileHelper fileHelper;
	fileHelper.Init(&threadPool);
	fileHelper.PrintDebugLogs();

	std::mutex mainEventMutex;
	std::condition_variable mainEventSystem;

	mgcp::TimeManager timeManager;
#if (_TIMER_MULTI_THREADED)
	timeManager.Start();
#endif
	int64_t lastTimeStamp = mgcp::TimeManager::GetMicroTime();

	DisplayIntroductionMessage();

	std::map<std::string, DynamicObject *> objectMap = {
		{"fh", (DynamicObject *)&fileHelper},
		{"tp", (DynamicObject *)&threadPool}
		//
	};

	mgcp::PrintMapKeys(objectMap);

	int32_t timeoutSystemState = 1;
	int32_t state = 1;
	int32_t counter = 0;
	int32_t test_limit = 100;

	std::function<void()> func([&] {
		counter++;
		if (counter > test_limit)
		{
#if (_TIMER_MULTI_THREADED)
			std::unique_lock<std::mutex> lock{mainEventMutex};
			mainEventSystem.notify_all();
#endif
			stdlog("limit reached state = 0");
			state = 0;
			lastTimeStamp = timeManager.GetMicroTime();
			return;
		}
		// fileHelper.WriteFile("test1", "");
		stdlog("interval count : " << counter);
	});

	int32_t key = 0;
	int32_t writeCounter = 0;
	int32_t writeCountery = 0;
	int32_t writeCounteru = 0;
	//use a separate thread to read command line and push it to state
	std::thread io{
		[&] {
			std::string input;
			while (true)
			{
				// fileHelper.WriteFile("test2input", "");
				std::getline(std::cin, input);
				if (input == "t")
				{
					timeManager.SetOrUpdateTimeout(func, 3000, key);
				}
				else if (input == "i")
				{
					timeManager.SetOrUpdateInterval(func, 100, key);
				}
				else if (input == "c")
				{
					timeManager.ClearAll();
					// timeManager.ClearTimeout(key);
				}
				else if (input == "ca")
				{
					timeManager.ClearAll();
				}
				else if (input == "l")
				{
					fileHelper.LoadSettings();
				}
				else if (input == "r")
				{
				}
				else if (input == "p")
				{
					std::int64_t timenow = mgcp::TimeManager::GetMicroTime();
					std::string data = "{ \"t1\": \"hello world\" }";
					auto options = mgcp::FileWriteOptions();
					options.shouldWrapDataAsArray = true;
					std::string path = "";
					std::string filename = "testfile.json";
					options.callback = [&]() {
						stdlog("Finished writing to the file in time: " << mgcp::TimeManager::GetMicroTime() - timenow);
					};
					fileHelper.WriteFile(path, filename, data, options);
				}
				else if (input == "w")
				{
					auto options = mgcp::FileWriteOptions();
					std::string data = "{ \"test\": \"" + std::to_string(writeCounter++) + std::string("\" }");
					std::string path = "";
					std::string filename = "testfile.json";
					fileHelper.WriteFile(path, filename, data, options);
				}
				else if (input == "wo")
				{
					stdlog("overwritecall");
					auto options = mgcp::FileWriteOptions();
					options.append = false;
					options.overwrite = true;
					std::string data = "{ \"test\": \"" + std::to_string(writeCounter++) + std::string("\" }");
					std::string testpath = "wooohoo/somewhere/gogo/";
					std::string altfile = "testfile2.json";
					fileHelper.WriteFile(testpath, altfile, data, options);
				}
				else if (input == "y")
				{
					std::function<void()> yfunc = [&]() {
						auto options = mgcp::FileWriteOptions();
						std::string data = "{ \"y-test\": \"" + std::to_string(writeCountery++) + std::string("\" }");
						std::string path = "";
						std::string filename = "testfile.json";
						options.callback = [&]() {
							stdlog("interval y test finished");
						};
						fileHelper.WriteFile(path, filename, data, options);
					};
					timeManager.SetOrUpdateInterval(yfunc, 10, 0);
				}
				else if (input == "u")
				{
					std::function<void()> ufunc = [&]() {
						auto options = mgcp::FileWriteOptions();
						std::string data = "{ \"uuuu-test\": \"" + std::to_string(writeCounteru++) + std::string("\" }");
						std::string path = "";
						std::string filename = "testfile.json";
						options.callback = [&]() {
							stdlog("interval uuuu test finished");
						};
						fileHelper.WriteFile(path, filename, data, options);
					};
					timeManager.SetOrUpdateInterval(ufunc, 16, 1);
				}
				else if (input == "z")
				{
					fileHelper.TestPrintSettings();
				}
				else if (input == "testCountLimit")
				{
					counter = 0;
					test_limit = 100;
					stdlog("counter: " << counter << " test_limit: " << test_limit);
				}
				else
				{
					lastTimeStamp = mgcp::TimeManager::GetMicroTime();
					std::lock_guard lock{mainEventMutex};
					state = inputEvaluation(objectMap, input);
					mainEventSystem.notify_all();
				}
			}
		}};
	io.detach();

	while (state == 1)
	{
		std::unique_lock<std::mutex> lock{mainEventMutex};
		//OSX - 57% is 1micro, 25% - 10micro - 5% 100micro
#if (_BENCHMARK_INTERRUPT_TIME)
		const int64_t timer = 700000000;
		if (mainEventSystem.wait_for(lock, std::chrono::microseconds(timer)) != std::cv_status::timeout)
		{
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
	if (numThreads == 1)
	{
		timeManager.Stop();
	}
	return 0;
}
