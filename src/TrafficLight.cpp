#include <iostream>
#include <random>
#include "TrafficLight.h"
#include <queue>


// Implementation of class "MessageQueue" 
 
 // wait for and receive new messages
template <typename T>
   T MessageQueue<T>::receive()
{
     std::unique_lock<std::mutex> lck(_mutex);
	while (_queue.empty()) _cond.wait(lck);  
    
     T message = std::move(_queue.back());    
     _queue.pop_back(); 			
    return message; 
} 

// move messages into queue 
 template <typename T>
 void MessageQueue<T>::send(T &&message)
{
    std::lock_guard<std::mutex> lck(_mutex);
    _queue.emplace_back(std::move(message));  
    _cond.notify_one(); 
} 

TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

// Call the receive function until traffic light is green
void TrafficLight::waitForGreen()
{
    while (true)
    {  
        if (_message_queue.receive() == green) {  
        return;     
        }
    }

} 
// Get traffic light phase
TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

// Start traffic light simulation in a thread
void TrafficLight::simulate()
{
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this)); 
}

void TrafficLight::cycleThroughPhases()
{
    //initialize cycle duration with random number between 4000 and 6000 milliseconds
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(4000,6000);   
    double cycle_duration = distribution(generator);  
    
    auto start = std::chrono::system_clock::now();  // start stop watch	

    while (true)
    {  
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>((std::chrono::system_clock::now()) - start).count();
        
        if (elapsed >= cycle_duration)
        {
            switch(_currentPhase) {
            case TrafficLightPhase::red : _currentPhase = TrafficLightPhase::green; 
            break;
            case TrafficLightPhase::green : _currentPhase = TrafficLightPhase::red;
            break;
            } 
 
            _message_queue.send(std::move(_currentPhase));  // send updated message to queue
           
            cycle_duration = distribution(generator);  // set next cycle duration 
            start = std::chrono::system_clock::now();  // reset stop watch for next cycle
            std::this_thread::sleep_for(std::chrono::milliseconds(1)); // wait 1ms between two cycles
        }

    }
}