#ifndef __SEMAPHORE_HPP__
#define __SEMAPHORE_HPP__

#include <mutex>
#include <condition_variable>

class Semaphore {
private:
    std::mutex m_mutex;
    std::condition_variable m_cv;
    unsigned long m_val=0;
public:
    /**
     * @brief Construct a new Semaphore object with 0 as initial value
     * 
     */
    Semaphore (void);

    /**
     * @brief Construct a new Semaphore object with specified initial value
     * @param initial_val - the initial value
     */
    Semaphore (
        unsigned long initial_val);
    /**
     * @brief - equivalent to sem_post()
     * 
     */
    void 
    post (void);
    /**
     * @brief equivalent to sem_wait()
     * 
     */
    void 
    wait (void);
};

#endif