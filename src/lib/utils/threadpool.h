#ifndef MGCP_LIB_UTILS_THREADPOOL_H
#define MGCP_LIB_UTILS_THREADPOOL_H

#include <functional>
#include <future>
#include <queue>
#include <thread>
#include <vector>

#include "../types/dynamicobject.h"

namespace CodeBlacksmith {

class ThreadPool : DynamicObject {
   public:
    using Task = std::function<void()>;

    explicit ThreadPool(std::size_t numThreads);
    ~ThreadPool();

    virtual void InvokeMethod(std::string& methodKey) override;

    template <class T>
    auto Enqueue(T task) -> std::future<decltype(task())> {
        auto wrapper = std::make_shared<std::packaged_task<decltype(task())()>>(std::move(task));
        {
            std::unique_lock<std::mutex> lock{m_EventMutex};
            m_Tasks.emplace([=, this] { (*wrapper)(); });
        }
        m_EventVar.notify_one();
        return wrapper->get_future();
    }

    // inline const std::string &getName() const override { return mName; };

   private:
    std::vector<std::thread> m_Threads;
    std::condition_variable m_EventVar;
    std::mutex m_EventMutex;
    bool m_Stopping = false;
    std::queue<Task> m_Tasks;

    void Start(std::size_t numThreads);
    void Stop() noexcept;
};

}  // namespace CodeBlacksmith

// notes on templating
// https://stackoverflow.com/questions/8024010/why-do-template-class-functions-have-to-be-declared-in-the-same-translation-unit
// Most compilers do not support external templates yet, which would allow the type of cpp/h separate you are looking for. However, you can
// still separate template declarations from implementations similar to what you want. Put the declarations in a .h files, put the
// implementations in a separate source file with whatever extension you want (.i and .ipp are popular), and then #include the source file
// at the bottom of the .h file. The compiler sees a single translation unit, and you get code separation.

// /*
// * foo.h
// *
// * Created on: Nov 5, 2011
// * Author: AutoBotAM
// */
// #ifndef FOO_H_
// #define FOO_H_

// template<typename Type>
// class Foo
// {
// public:
//     void Bar(Type object);
// };

// #include "foo.ipp"

// #endif /* FOO_H_ */

// https://stackoverflow.com/questions/9998402/c11-does-not-deduce-type-when-stdfunction-or-lambda-functions-are-involved
// template <typename R, typename T>
// int32_t myfunc(std::function<R(T)> lambda)
// {
//   return lambda(2);
// }

// template <typename Func, typename Arg1>
// static auto getFuncType(Func* func = nullptr, Arg1* arg1 = nullptr) -> decltype((*func)(*arg1)) {};

// template <typename Func>
// int32_t myfunc(Func lambda)
// {
//   return myfunc<int32_t, decltype(getFuncType<Func, int32_t>())>(lambda);
// }

#endif