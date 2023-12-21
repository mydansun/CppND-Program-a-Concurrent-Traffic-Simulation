#include <iostream>
#include <random>
#include "TrafficLight.h"
#include <chrono>

/* Implementation of class "MessageQueue" */

template<typename T>
T MessageQueue<T>::receive() {
    std::unique_lock<std::mutex> uLock(_mutex);
    _cond.wait(uLock, [this] { return !_queue.empty(); });
    T msg = std::move(_queue.back()); // NOLINT(*-move-const-arg)
    _queue.pop_back();
    return msg;
}

template<typename T>
void MessageQueue<T>::send(T &&msg) {
    std::lock_guard<std::mutex> lock(_mutex);
    _queue.push_back(std::move(msg));
    _cond.notify_one();
}

/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate() {
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

void TrafficLight::waitForGreen() {
    while (true) {
        if (_queue.receive() == TrafficLightPhase::green){
            return;
        }
    }
}


[[noreturn]] void TrafficLight::cycleThroughPhases() {
    std::uniform_real_distribution<double> dis(4000, 6000);
    std::random_device rd;
    std::default_random_engine rng{rd()};
    double cycleDuration = dis(rng);

    auto start_at = std::chrono::steady_clock::now();
    while (true) {
        double timeSpan = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - start_at).count());

        if (timeSpan >= cycleDuration) {
            _currentPhase = _currentPhase == TrafficLightPhase::red ? TrafficLightPhase::green : TrafficLightPhase::red;
            _queue.send(std::move(_currentPhase)); // NOLINT(*-move-const-arg)
            start_at = std::chrono::steady_clock::now();
            cycleDuration = dis(rng);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
