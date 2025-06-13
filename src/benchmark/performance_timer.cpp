#include "performance_timer.h"
#include <stdexcept>

PerformanceTimer::PerformanceTimer() : m_isRunning(false) {
    // Initialize with default time points
    m_startTime = Clock::now();
    m_endTime = Clock::now();
}

void PerformanceTimer::start() {
    m_startTime = Clock::now();
    m_isRunning = true;
}

double PerformanceTimer::stop() {
    if (!m_isRunning) {
        throw std::runtime_error("Timer is not running - call start() first");
    }
    
    m_endTime = Clock::now();
    m_isRunning = false;
    
    return calculateDurationMs(m_startTime, m_endTime);
}

double PerformanceTimer::getElapsedMs() const {
    if (!m_isRunning) {
        throw std::runtime_error("Timer is not running - call start() first");
    }
    
    TimePoint currentTime = Clock::now();
    return calculateDurationMs(m_startTime, currentTime);
}

void PerformanceTimer::reset() {
    m_isRunning = false;
    m_startTime = Clock::now();
    m_endTime = Clock::now();
}

bool PerformanceTimer::isRunning() const {
    return m_isRunning;
}

double PerformanceTimer::calculateDurationMs(const TimePoint& start, const TimePoint& end) const {
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    return static_cast<double>(duration.count());
}
