#include <chrono>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <string>
#include <thread>

#include <conwrap2/Processor.hpp>


namespace net = std::experimental::net;
using namespace std::literals::string_literals;


struct Temp
{
    std::unique_ptr<int> ptr;
};


int main()
{
    auto start_time = std::chrono::high_resolution_clock::now();
    {
        conwrap2::Processor<Temp> processor
        {
            [](auto& processor) -> Temp
            {
                return std::move(Temp{});
            }
        };

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        start_time = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 1000000; i++)
        {
            processor.process([] {});
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<uint64_t, std::milli>>(end_time - start_time);
    std::cout << "duration=" << duration.count() << " millisec.\n\r";
}
