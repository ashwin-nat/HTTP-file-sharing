#ifndef __DB_HPP__
#define __DB_HPP__

#include <sqlite3.h>
#include <string>

#define SINGLETON_DB_OBJECT

class db_handle {
    private:
        sqlite3 *m_hdl;
        sqlite3_stmt *m_stmt;
        bool m_is_open;
        bool m_ongoing_transaction;

        /**
         * @brief - Performs cleanup, frees resources, closes handles
         *
         */
        void
        cleanup (void);
    public:
        /**
         * @brief Construct a new db handle object, open the db in the given
         * path
         * @param filename - string containing the file name
         */
        explicit db_handle (
            const std::string &filename);
        /**
         * @brief Destroy the iotcDB object, cleanup if required
         */
        ~db_handle (void);
        /**
         * @brief Get the errmsg string
         * @return std::string - string containing the error message
         */
        std::string 
        get_errmsg (void);
        /**
         * @brief       - Prepares the sqlite3_stmt with the given statement
         * @param str   - string containing the statement that can be bound to
         * @return int  - sqlite3 return code
         */
        int
        prepare_stmt (
            const std::string &str);
        /**
         * @brief       - Bind the given int to the specified ? in the query
         * @param offset- the nth question mark in the query (starting from 1)
         * @param val   - the value to be bound
         * @return int  - sqlite3 return code
         */
        int
        bind_int (
            int offset,
            int val);
        /**
         * @brief       - Bind the given string to the specified ? in the
         *                  query
         * @param offset- the nth question mark in the query (starting from 1)
         * @param str   - the value to be bound
         * @return int  - sqlite3 return code
         */
        int
        bind_string (
            int offset,
            const std::string &str);
        /**
         * @brief       - Step through the given statement (get results)
         * @return int  - sqlite3 return code
         */
        int
        step (void);
        /**
         * @brief       - Get the int value at the specified column of the
         *                  result
         * @param col   - column number (starting at 0)
         * @return int  - value
         */
        int
        get_int (
            int col);
        /**
         * @brief       - Get the string value at the specified column of the
         *                  result
         * @param col   - column number (starting at 0)
         * @param result
         * @return int
         */
        int
        get_str (
            int col,
            std::string &result);
        /**
         * @brief       - Begins an sqlite transaction (for multiple inserts)
         * @return int  - sqlite3 return code
         */
        int
        start_transaction (void);
        /**
         * @brief       - Ends an sqlite transaction (for multiple requests)
         * @return int  - sqlite3 return code
         */
        int 
        end_transaction (void);
        /**
         * @brief       - Executes the given string query
         * @param query - the string containing the query
         * @return int  - sqlite3 return code
         */
        int
        exec_query (
            const std::string &query);
        /**
         * @brief       - Reset a prepared statement
         * @return int  - sqlite3 return code
         */
        int 
        reset_stmt (void);
};

#endif  //__DB_HPP__