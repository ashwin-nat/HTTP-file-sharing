#include "db.hpp"
#include <mutex>
#include <stdexcept>
#include <sstream>
#ifdef  SINGLETON_DB_OBJECT
#include <mutex>
#endif
/******************************************************************************/
#ifdef  SINGLETON_DB_OBJECT
static std::mutex _db_mutex;
#endif
/******************************************************************************/
/* Public functions */
/**
 * @brief Construct a new db handle object, open the db in the given 
 * path
 * 
 * @param filename - string containing the file name
 */
db_handle :: db_handle (const std::string &filename)
{
#ifdef  SINGLETON_DB_OBJECT
    _db_mutex.lock();
#endif
    int rc = sqlite3_open (filename.c_str(), &m_hdl);
    //check open status
    if (SQLITE_OK != rc) {
#ifdef  SINGLETON_DB_OBJECT
        _db_mutex.unlock();
#endif
        const std::string errmsg = sqlite3_errmsg(m_hdl);
        //raise exception here
        throw std::invalid_argument(errmsg);
    }
    m_stmt = nullptr;
    m_is_open = true;
}

/**
 * @brief Destroy the iotcDB object, cleanup if required
 * 
 */
db_handle :: ~db_handle (void)
{
    cleanup();
}

/**
 * @brief Get the errmsg string
 * @return std::string - string containing the error message
 */
std::string 
db_handle :: get_errmsg (void)
{
    return sqlite3_errmsg(m_hdl);
}

/**
 * @brief       - Prepares the sqlite3_stmt with the given statement
 * 
 * @param str   - string containing the statement that can be bound to
 * @return int  - sqlite3 return code
 */
int 
db_handle :: prepare_stmt (
    const std::string &str)
{
    if(m_stmt) {
        sqlite3_finalize (m_stmt);
        m_stmt = nullptr;
    }
    return sqlite3_prepare_v2 (m_hdl, str.c_str(), -1, &m_stmt, NULL);
}

/**
 * @brief       - Bind the given int to the specified ? in the query
 * @param offset- the nth question mark in the query (starting from 1)
 * @param val   - the value to be bound
 * @return int  - sqlite3 return code
 */
int 
db_handle :: bind_int (
    int offset, 
    int val)
{
    return sqlite3_bind_int (m_stmt, offset, val);
}

/**
 * @brief       - Bind the given string to the specified ? in the 
 *                  query
 * @param offset- the nth question mark in the query (starting from 1)
 * @param str   - the value to be bound
 * @return int  - sqlite3 return code
 */
int 
db_handle :: bind_string (
    int offset, 
    const std::string &str)
{
    return sqlite3_bind_text (m_stmt, offset, str.c_str(), -1, 
            SQLITE_TRANSIENT);
}

/**
 * @brief       - Step through the given statement (get results)
 * 
 * @return int  - sqlite3 return code
 */
int 
db_handle :: step (void)
{
    return sqlite3_step (m_stmt);
}

/**
 * @brief       - Get the int value at the specified column of the result
 * @param col   - column number (starting at 0)
 * @return int  - value
 */
int 
db_handle :: get_int (
    int col)
{
    return sqlite3_column_int (m_stmt, col);
}

/**
 * @brief       - Get the string value at the specified column of the result
 * @param col   - column number (starting at 0)
 * @param result 
 * @return int 
 */
int db_handle :: get_str (int col, std::string &result)
{
    auto ret = sqlite3_column_text (m_stmt, col);
    if (ret) {
        std::stringstream ss;
        ss << ret;
        result = ss.str();
        return result.length();
    }
    return 0;
}

/**
 * @brief       - Begins an sqlite transaction (for multiple inserts)
 * @return int  - sqlite3 return code
 */
int
db_handle :: start_transaction (void)
{
    return sqlite3_exec(m_hdl, "BEGIN", nullptr, nullptr, nullptr);
}

/**
 * @brief       - Ends an sqlite transaction (for multiple requests)
 * @return int  - sqlite3 return code
 */
int 
db_handle :: end_transaction (void)
{
    return sqlite3_exec(m_hdl, "COMMIT", nullptr, nullptr, nullptr);
}

/**
 * @brief       - Executes the given string query
 * @param query - the string containing the query
 * @return int  - sqlite3 return code
 */
int
db_handle :: exec_query (
    const std::string &query)
{
    return sqlite3_exec (m_hdl, query.c_str(), nullptr, nullptr, nullptr);
}
/******************************************************************************/
/* Private functions */
/**
 * @brief - Performs cleanup, frees resources, closes handles
 * 
 */
void 
db_handle :: cleanup (void)
{
    if (m_is_open) {
        if (m_stmt) {
            sqlite3_finalize (m_stmt);
        }
        sqlite3_close (m_hdl);
#ifdef  SINGLETON_DB_OBJECT
        _db_mutex.unlock();
#endif
        m_is_open = false;
    }
}