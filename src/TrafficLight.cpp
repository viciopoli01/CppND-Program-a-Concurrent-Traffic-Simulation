#include "TrafficLight.h"
#include <iostream>
#include <random>

/* Implementation of class "MessageQueue" */

template <typename T>
void  MessageQueue<T>::send(T &&message) {
  std::lock_guard<std::mutex> lck(_mtx);
  _queue.push_back(std::move(message));

  _cond.notify_one();
}

template <typename T>
T  MessageQueue<T>::receive() {
  std::unique_lock<std::mutex> lck(_mtx);
  _cond.wait(lck, [this] { return !_queue.empty(); });

  T msg = std::move(_queue.back());
  _queue.pop_back();

  return msg;
}


/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop
    // runs and repeatedly calls the receive function on the message queue.
    // Once it receives TrafficLightPhase::green, the method returns.
    while (true)
    {
        if(_queue.receive()==TrafficLightPhase::green)
            return;
    }
    
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}


void TrafficLight::simulate() {
  // FP.2b : Finally, the private method „cycleThroughPhases“ should be started
  // in a thread when the public method „simulate“ is called. To do this, use
  // the thread queue in the base class.
  threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases() {
  // FP.2a : Implement the function with an infinite loop that measures the time
  // between two loop cycles and toggles the current phase of the traffic light
  // between red and green and sends an update method to the message queue using
  // move semantics. The cycle duration should be a random value between 4 and 6
  // seconds. Also, the while-loop should use std::this_thread::sleep_for to
  // wait 1ms between two cycles.

  srand(time(NULL));
  int cycle_duration = rand() % 3 + 4;
  auto t1 = std::chrono::high_resolution_clock::now();

  while (true) {
    auto int_s = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - t1);
    if (int_s > std::chrono::seconds(cycle_duration)) {
      cycle_duration = rand() % 3 + 4;
      _currentPhase = _currentPhase == red ? green : red;

      _queue.send(std::move(_currentPhase));

      t1 = std::chrono::high_resolution_clock::now();

    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}
