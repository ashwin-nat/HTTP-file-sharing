#ifndef __TPOOL_HPP__
#define __TPOOL_HPP__

#include "http.hpp"
#include "semaphore.hpp"
#include <thread>
#include <memory>
#include <vector>
#include <queue>
#include <mutex>

struct _TPool_job {
    void (*fn_ptr) (std::shared_ptr<TCPConnection>) = nullptr;
    std::shared_ptr<TCPConnection> connection = nullptr;
};

class TPool {
private:
    std::vector<std::thread> m_pool;
    // std::queue<std::shared_ptr<TCPConnection>> m_queue;
    std::queue<_TPool_job> m_queue;
    std::mutex m_queue_mutex;
    Semaphore m_sem;
    
    /**
     * @brief           - Get a job from the queue
     * @return std::shared_ptr<TCPConnection> - pointer to the connection obj
     */
    // std::shared_ptr<TCPConnection> 
    _TPool_job 
    get_job (void);

    void 
    _tpool_worker_fn (
        unsigned int id);
public:
    /**
     * @brief       - Construct a new TPool object with specified number of 
     *                  threads
     * @param count - Worker thread count
     */
    explicit 
    TPool (
        unsigned int count=5);
    /**
     * @brief       - Destroy the Tpool object
     */
    ~TPool (void);
    /**
     * @brief       - Add this connection to the thread pool job queue
     * @param connection - shared ptr to the TCP connection object
     */
    void 
    add_job (
        void (*fn_ptr) (std::shared_ptr<TCPConnection>),
        std::shared_ptr<TCPConnection> connection);
};

#endif