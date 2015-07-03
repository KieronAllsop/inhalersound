#pragma once

#include <boost/thread/condition_variable.hpp>
#include <boost/thread/lock_types.hpp>
#include <boost/thread/mutex.hpp>

class Semaphore
{
    // Number of tokens
    unsigned int Count;

    // Mutex: lock required for accessing Count
    boost::mutex Mutex;

    // Condition variable: wait and signal
    boost::condition_variable Condition;

public:
    explicit Semaphore(unsigned int startValue) 
       : Count(startValue), Mutex(), Condition() 
	{ }

	Semaphore() { }

    void Signal() // Up
    {
        boost::unique_lock<boost::mutex> lock(Mutex); // Acquire lock for Count update
        Count++; // Increment Count (add token)
        Condition.notify_one(); // Notify a thread waiting on Condition
    }

    void Wait() // Down
    {
        boost::unique_lock<boost::mutex> lock(Mutex); // Acquire lock for Count read
        while (Count == 0) // While Count is 0, wait on Condition
        {
             Condition.wait(lock);
        }
        Count--; // Decrement Count (take token)
    }

};
