#pragma once

#include <chrono>
#include <iostream>

class LogDuration {
public:
    // заменим имя типа std::chrono::steady_clock
    // с помощью using для удобства
    using Clock = std::chrono::steady_clock;

    LogDuration() {
    }

    LogDuration(std::string operation_name) : operation_name_(operation_name){
    }


    ~LogDuration() {
        using namespace std::chrono;
        using namespace std::literals;

        const auto end_time = Clock::now();
        const auto dur = end_time - start_time_;
        std::cerr   << operation_name_ << ": "s << duration_cast<milliseconds>(dur).count() 
                    << " ms"s << std::endl;
    }

private:
    const Clock::time_point start_time_ = Clock::now();
    std::string operation_name_;
};