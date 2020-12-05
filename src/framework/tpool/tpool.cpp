#include "tpool.hpp"
#include "loguru.hpp"
#include <stdexcept>

/**
 * @brief       - Construct a new TPool object with specified number of 
 *                  threads
 * @param count - Worker thread count
 */
TPool :: TPool (
    unsigned int count)
{
    if (count > TPOOL_MAX_WORKERS) {
        throw std::invalid_argument ("max supported thread pool workers is " + 
                std::to_string(TPOOL_MAX_WORKERS));
    }
    for (unsigned int i=0; i<count; ++i) {
        m_pool.push_back (std::thread (&TPool::_tpool_worker_fn, this, i+1));
    }
}

/**
 * @brief       - Destroy the Tpool object
 */
TPool :: ~TPool (void)
{
    ;
}

/**
 * @brief       - Add this connection to the thread pool job queue
 * @param connection - shared ptr to the TCP connection object
 */
void 
TPool :: add_job (
    void (*fn_ptr) (std::shared_ptr<TCPConnection>),
    std::shared_ptr<TCPConnection> connection)
{
    _TPool_job job;
    job.fn_ptr = fn_ptr;
    job.connection = connection;
    
    std::lock_guard<std::mutex> lock (m_queue_mutex);
    m_queue.push (job);
    m_sem.post ();
}

/**
 * @brief           - Get a job from the queue
 * @return std::shared_ptr<TCPConnection> - pointer to the connection obj
 */
// std::shared_ptr<TCPConnection> 
_TPool_job 
TPool :: get_job (void)
{
    m_sem.wait ();
    std::lock_guard<std::mutex> lock (m_queue_mutex);
    auto ret = m_queue.front ();
    m_queue.pop();
    return ret;
}

void 
TPool :: _tpool_worker_fn (
    unsigned int id)
{
    char tname[10] = {0,};
    snprintf (tname, sizeof(tname)-1, "TPW%02u", id);
    loguru::set_thread_name (tname);

    while (1) {
        auto ret = get_job ();
        ret.fn_ptr (ret.connection);
    }
}