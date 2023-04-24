#pragma once

#include <chrono>

#include "callable_literal.h"

class Clock_Callable: public Callable_Literal {
    public:
        Literal::Ptr call(Interpreter &interpreter, const std::vector<Literal::Ptr> &arguments) const override {
            auto time { std::chrono::system_clock::now() };
            auto since_epoch { time.time_since_epoch() };
            auto millis { std::chrono::duration_cast<std::chrono::milliseconds>(since_epoch) };
            return Literal::create(static_cast<double>(millis.count()) / 1000.0);
        }
        int arity() const override { return 0; }
};
