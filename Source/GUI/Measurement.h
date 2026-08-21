/*
  ==============================================================================

    Measurement.h
    Created: 20 Apr 2025 10:24:59pm
    Author:  Aaron Petrini

  ==============================================================================
*/
#pragma once

#include <atomic>

struct Measurement {
    void reset() noexcept
    {
        value.store(0.f);
    }
    
    void updateIfGreater(float newValue) noexcept
    {
        auto oldValue = value.load();
        while (newValue > oldValue && !value.compare_exchange_weak(oldValue, newValue));
    }
    
    float readAndReset() noexcept
    {
        return value.exchange(0.f);
    }
    
    std::atomic<float> value;
};
