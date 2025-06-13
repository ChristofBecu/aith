#pragma once

#include <chrono>

/**
 * High-precision performance timer for measuring operation durations
 * 
 * This class provides a clean interface for timing operations, hiding the
 * complexity of std::chrono and providing millisecond precision timing.
 * 
 * Usage:
 *   PerformanceTimer timer;
 *   timer.start();
 *   // ... perform operation ...
 *   double durationMs = timer.stop();
 */
class PerformanceTimer {
public:
    /**
     * Constructor - initializes timer in stopped state
     */
    PerformanceTimer();

    /**
     * Start timing an operation
     * Resets any previous timing data
     */
    void start();

    /**
     * Stop timing and return duration in milliseconds
     * @return Duration in milliseconds since start() was called
     * @throws std::runtime_error if timer was not started
     */
    double stop();

    /**
     * Get elapsed time without stopping the timer
     * @return Elapsed time in milliseconds since start() was called
     * @throws std::runtime_error if timer was not started
     */
    double getElapsedMs() const;

    /**
     * Reset the timer to stopped state
     */
    void reset();

    /**
     * Check if timer is currently running
     * @return true if timer is running, false otherwise
     */
    bool isRunning() const;

private:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;

    TimePoint m_startTime;
    TimePoint m_endTime;
    bool m_isRunning;

    /**
     * Calculate duration between two time points in milliseconds
     */
    double calculateDurationMs(const TimePoint& start, const TimePoint& end) const;
};
