#include <chrono>
#include <iostream>

#include <conwrap2/Processor.hpp>


struct Temp
{
    Temp(const Temp&) = delete;             // non-copyable
    Temp& operator=(const Temp&) = delete;  // non-assignable
    Temp(Temp&& rhs) = default;
    Temp& operator=(Temp&& rhs) = default;
 };


int main()
{
    auto start_time = std::chrono::high_resolution_clock::now();
    {
        conwrap2::Processor<Temp> processor{Temp{}};
/*
        start_time = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 1000000; i++)
        {
            processor.process([] {});
        }
*/
/*
        processor.process([ptr = std::make_unique<int>(1)](auto& context)
        {
            std::cout << "HERE1" << std::endl;
            context.getProcessorProxy().process([ptr = std::make_unique<int>(1)](auto& context)
            {
                std::cout << "HERE2" << std::endl;
            });
        });
*/
        processor.processWithDelay([ptr = std::make_unique<int>(1)](auto& context)
        {
            std::cout << "HERE" << std::endl;
        }, std::chrono::seconds{1});
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<uint64_t, std::milli>>(end_time - start_time);
    std::cout << "duration=" << duration.count() << " millisec.\n\r";
}
