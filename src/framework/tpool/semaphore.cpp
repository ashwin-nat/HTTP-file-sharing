#include "semaphore.hpp"

/**
 * @brief Construct a new Semaphore object with 0 as initial value
 * 
 */
Semaphore :: Semaphore (void)
{
    m_val = 0;
}

/**
 * @brief Construct a new Semaphore object with specified initial value
 * @param initial_val - the initial value
 */
Semaphore :: Semaphore (
    unsigned long initial_val)
{
    m_val = initial_val;
}

/**
 * @brief - equivalent to sem_post()
 * 
 */
void 
Semaphore :: post (void)
{
    std::lock_guard<std::mutex> lock (m_mutex);
    ++m_val;
    m_cv.notify_one();
}

/**
 * @brief equivalent to sem_wait()
 * 
 */
void 
Semaphore :: wait (void)
{
    std::unique_lock<std::mutex> lock (m_mutex);
    //deal with spurious wakeups
    while (0 == m_val) {
        m_cv.wait (lock);
    }
    m_val--;
}