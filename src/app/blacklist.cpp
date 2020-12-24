#include "blacklist.hpp"
#include "db.hpp"
#include "prog_options.hpp"
#include "loguru.hpp"
#include <map>
#include <shared_mutex>
#include <assert.h>
#include <set>

#define ACQUIRE_RD_LOCK()   std::shared_lock<std::shared_mutex> lock (_rwlock);
#define ACQUIRE_WR_LOCK()   std::unique_lock<std::shared_mutex> lock (_rwlock);

extern ProgOptions prog_options;

static void _get_statslist_from_db (int threshold);
static void _get_blacklist_from_db (void);
static void _initial_populate_blacklist_in_db (void);

static std::map<std::string,int> _stats_list;
static std::set<std::string> _blacklist;

static std::shared_mutex _rwlock;

void init_blacklist (void)
{
    _get_blacklist_from_db ();
    _get_statslist_from_db (prog_options.blacklist_threshold);
    _initial_populate_blacklist_in_db ();
}

bool is_ipaddr_blacklisted (const std::string &ipaddr)
{
    ACQUIRE_RD_LOCK();
    auto result = _blacklist.find (ipaddr);
    if (result == _blacklist.end()) {
        return false;
    }
    return true;
}

bool update_stats_list_blacklist (const std::string &ipaddr, bool good_request)
{
    ACQUIRE_WR_LOCK();
    auto result = _stats_list.find (ipaddr);

    if (result == _stats_list.end()) {
        _stats_list.emplace (ipaddr, 1);
        return false;
    }
    else {
        int offset = (good_request) ? (1) : (-1);
        result->second += offset;

        //if the count(score) is less than our threshold, blacklist it
        if (result->second <= prog_options.blacklist_threshold) {
            assert (_blacklist.find (ipaddr) != _blacklist.end());

            _blacklist.emplace (result->first);
            LOG_S(INFO) << "Blacklisting IP address " << ipaddr;
        }
        return true;
    }
}

bool get_score_for_ipaddr (const std::string &ipaddr, int score)
{
    ACQUIRE_RD_LOCK();
    auto result = _stats_list.find (ipaddr);

    if (result != _stats_list.end()) {
        score = result->second;
    }
    return false;
}

static void
_get_statslist_from_db (
    int threshold)
{
    ACQUIRE_WR_LOCK();
    db_handle db (prog_options.db_file);
    std::string query = "select ipaddr, count from stats;";
    if (db.prepare_stmt (query) != SQLITE_OK) {
        LOG_S(ERROR) << "Error in query " << query << " error=" << 
            db.get_errmsg();
        exit (1);
    }

    std::string ipaddr;
    int count;
    while (db.step() == SQLITE_ROW) {
        if (db.get_str(0, ipaddr) == 0) {
            LOG_S(ERROR) << "Error in get_str. error=" << db.get_errmsg();
            exit (1);
        }
        count = db.get_int (1);

        _stats_list.emplace (ipaddr, count);
        if (count <= threshold) {
            _blacklist.emplace (ipaddr);
            LOG_S(INFO) << "Blacklisting IP address " << ipaddr;
        }
    }

}

static void 
_get_blacklist_from_db (void)
{
    ACQUIRE_WR_LOCK();
    db_handle db (prog_options.db_file);
    std::string query = "select ipaddr from blacklist;";
    if (db.prepare_stmt (query) != SQLITE_OK) {
        LOG_S(ERROR) << "Error in query " << query << " error=" << 
            db.get_errmsg();
        exit (1);
    }

    std::string ipaddr;
    while (db.step() == SQLITE_ROW) {
        if (db.get_str(0, ipaddr) == 0) {
            LOG_S(ERROR) << "Error in get_str. error=" << db.get_errmsg();
            exit (1);
        }
        _blacklist.emplace (ipaddr);
    }

    LOG_S(INFO) << "Initialsed blacklist with " << _blacklist.size() << 
        " addresses";
}

static void 
_initial_populate_blacklist_in_db (void)
{
    ACQUIRE_WR_LOCK();
    db_handle db (prog_options.db_file);
    std::string query = "INSERT or IGNORE into blacklist (ipaddr) values (?)";
    db.start_transaction();

    for (auto &it : _blacklist) {
        if (db.prepare_stmt (query) != SQLITE_OK) {
            LOG_S(ERROR) << "query " << query << " prepare failed with error" <<
                db.get_errmsg();
            assert (false);
            continue;
        }

        if (db.bind_string(1, it) != SQLITE_OK) {
            LOG_S(ERROR) << "query " << query << 
                " bind_string failed with error" << db.get_errmsg();
            assert (false);
            continue;
        }

        if (db.step() != SQLITE_DONE) {
            LOG_S(ERROR) << "query " << query << " step failed with error" <<
                db.get_errmsg();
            assert (false);
            continue;
        }

        if (db.reset_stmt() != SQLITE_OK) {
            LOG_S(ERROR) << "query " << query << " reset failed with error" <<
                db.get_errmsg();
            assert (false);
            continue;
        }
    }
    db.end_transaction();
}