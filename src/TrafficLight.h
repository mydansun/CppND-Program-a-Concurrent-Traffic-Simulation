#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;


enum TrafficLightPhase{
    red,
    green,
};

template <class T> class MessageQueue
{
public:
    void send(T &&);
    T receive();
private:
    std::mutex _mutex;
    std::condition_variable _cond;
    std::deque<TrafficLightPhase> _queue;
};


class TrafficLight : public TrafficObject
{
public:
    // constructor / desctructor
    TrafficLight();
    // getters / setters
    TrafficLightPhase getCurrentPhase();

    // typical behaviour methods
    void waitForGreen();
    void simulate() override;
private:
    // typical behaviour methods
    [[noreturn]] void cycleThroughPhases();

    MessageQueue<TrafficLightPhase> _queue;
    TrafficLightPhase _currentPhase;
    std::condition_variable _condition;
    std::mutex _mutex;
};

#endif