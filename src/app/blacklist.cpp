#include "blacklist.hpp"
#include "loguru.hpp"
#include "prog_options.hpp"
#include "db.hpp"
#include <shared_mutex>
#include <map>
#include <cassert>

/******************************************************************************/
/* local macros */
#define _ACQUIRE_BL_RD_LOCK()   \
    std::shared_lock<std::shared_mutex> lock (_bl_rwlock);
#define _ACQUIRE_BL_WR_LOCK()   \
    std::unique_lock<std::shared_mutex> lock (_bl_rwlock);

/******************************************************************************/
/* File local variables */
static std::shared_mutex _bl_rwlock;
static std::map<std::string,int> _stats_list;

/******************************************************************************/
/* Static function declarations */
static void _get_stats_from_db (void);
static void _insert_into_stats_db (const std::string &ipaddr, int score);
static void _update_in_stats_db (const std::string &ipaddr, int score);
static void __insert_update_stats_db (const std::string &query,
    const std::string &ipaddr, int score);

/******************************************************************************/
/* Publicly visible function definitions */
/**
 * @brief   Initialise the internal blacklist stats data structure from the DB
 *
 */
void init_blacklist (void)
{
    _get_stats_from_db();
}

/**
 * @brief   Check if the given IP address is blacklisted
 * @param   ipaddr - string version of the IP address
 * @return  true - the IP address is blacklisted
 * @return  false - the IP address is not blacklisted
 */
bool is_ipaddr_blacklisted (const std::string &ipaddr)
{
    //get read lock since we're only looking up
    extern ProgOptions prog_options;
    _ACQUIRE_BL_RD_LOCK();

    //look up the stats list map for the given IP address
    auto result = _stats_list.find (ipaddr);
    if (_stats_list.end() == result) {
        return false;
    }
    else {
        if (result->second < prog_options.blacklist_threshold) return true;
        else return false;
    }
}

/**
 * @brief   Update the stats and blacklist tables for a given IP address
 * @param   ipaddr - string version of the IP address
 * @param   good_request - true if the request was valid
 * @return  true - the IP address is good for now
 * @return  false - the IP address should now be blacklisted
 */
bool update_ip_addr_stats (const std::string &ipaddr, bool good_request)
{
    //get write lock since we will update/insert into the map
    _ACQUIRE_BL_WR_LOCK();

    //try to find the address in the map
    extern ProgOptions prog_options;
    int final_score = 0;
    auto result = _stats_list.find (ipaddr);
    if (_stats_list.end() == result) {

        //not found in map. need to insert into map and DB
        final_score = (good_request) ? (+1) : (-1);
        _stats_list.emplace (ipaddr, final_score);
        _insert_into_stats_db (ipaddr, final_score);
    }
    else {

        //update score in map and db
        result->second = result->second + ((good_request) ? (1) : (-1));
        final_score = result->second;
        _update_in_stats_db (ipaddr, final_score);
    }

    //check if the ip address should now be blacklisted
    return (final_score > prog_options.blacklist_threshold) ? (true) : (false);
}

/**
 * @brief   Get the score for the given IP address
 * @param   ipaddr - String version of the IP address
 * @param   score - ref to the variable where the score is to be written to
 * @return  true - score has been found successfully
 * @return  false - could not retrieve score
 */
bool get_score_for_ipaddr (const std::string &ipaddr, int &score)
{
    //get read lock since we're just looking up the map
    _ACQUIRE_BL_RD_LOCK();
    auto result = _stats_list.find (ipaddr);
    if (_stats_list.end() == result) {
        return false;
    }
    else {
        score = result->second;
        return true;
    }
}

/******************************************************************************/
/* Static function definitions */

/**
 * @brief   Initial retrieval of stats from the DB
 *
 */
static void _get_stats_from_db (void)
{
    extern ProgOptions prog_options;
    db_handle db (prog_options.db_file);

    //prepare the query statement
    const std::string &query = "SELECT ipaddr,count from stats;";
    if (db.prepare_stmt (query) != SQLITE_OK) {
        LOG_S(ERROR) << "Error in query: " << query << " error=" <<
            db.get_errmsg();
        assert (false);
        exit (1);
    }

    //run the query and loop through the results
    std::string ipaddr;
    int count;
    //get write lock since we're inserting into the map
    _ACQUIRE_BL_WR_LOCK();
    while (db.step() == SQLITE_ROW) {
        if (db.get_str(0, ipaddr) == 0) {
            LOG_S(ERROR) << "Error in get_str. error=" << db.get_errmsg();
            assert (false);
            exit (1);
        }
        count = db.get_int (1);

        //add to the stats map
        _stats_list.emplace (ipaddr, count);
    }
}

/**
 * @brief   Create a new entry in the stats table with the given score
 * @param   ipaddr - string version of the IP address
 * @param   score - score value
 */
static void _insert_into_stats_db (const std::string &ipaddr, int score)
{
    //prepare and process the query
    const std::string &query = "INSERT into stats (count,ipaddr) VALUES (?,?)";
    __insert_update_stats_db (query, ipaddr, score);
}

/**
 * @brief   Update an existing entry in the stats table with the given score
 * @param   ipaddr - string version of the IP address
 * @param   score - score value
 */
static void _update_in_stats_db (const std::string &ipaddr, int score)
{
    //prepare and process the query
    const std::string &query = "UPDATE stats SET count=? where ipaddr=?;";
    __insert_update_stats_db (query, ipaddr, score);
}

/**
 * @brief   Insert/Update the entry in the stats table with the given score
 * @param   query - const ref to the query
 * @param   ipaddr - const ref to the ip addr
 * @param   score - the score of this address
 */
static void __insert_update_stats_db (
    const std::string &query,
    const std::string &ipaddr,
    int score)
{
    extern ProgOptions prog_options;
    db_handle db (prog_options.db_file);

    //prepare the statement
    if (db.prepare_stmt (query) != SQLITE_OK) {
        LOG_S(ERROR) << "Error in query: " << query << " error=" <<
            db.get_errmsg();
        assert (false);
        exit (1);
    }

    //bind the score
    if (db.bind_int(1, score) != SQLITE_OK) {
        LOG_S(ERROR) << "query " << query <<
            " bind_string failed with error" << db.get_errmsg();
        assert (false);
        exit (1);
    }

    //bind the ip addr
    if (db.bind_string(2, ipaddr) != SQLITE_OK) {
        LOG_S(ERROR) << "query " << query <<
            " bind_int failed with error" << db.get_errmsg();
        assert (false);
        exit (1);
    }

    //process the statement
    if (db.step() != SQLITE_DONE) {
        LOG_S(ERROR) << "query " << query << " step failed with error" <<
            db.get_errmsg();
        assert (false);
        exit (1);
    }
}