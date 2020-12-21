#include "blacklist.hpp"
#include "db.hpp"
#include "prog_options.hpp"
#include "loguru.hpp"
#include <map>
#include <shared_mutex>
#include <assert.h>

extern ProgOptions prog_options;

static void _get_statslist_from_db (int threshold);
static std::map<std::string,int> _blacklist, _stats_list;

static std::shared_mutex _rwlock;

void init_blacklist (void)
{
    _get_statslist_from_db (prog_options.blacklist_threshold);
}

bool is_ipaddr_blacklisted (const std::string &ipaddr)
{
    std::shared_lock lock (_rwlock);
    auto result = _blacklist.find (ipaddr);
    if (result == _blacklist.end()) {
        return false;
    }
    return true;
}

void update_stats_list_blacklist (const std::string &ipaddr, bool good_request)
{
    std::unique_lock lock (_rwlock);
    auto result = _stats_list.find (ipaddr);

    if (result == _stats_list.end()) {
        _stats_list.emplace (ipaddr, 1);
    }
    else {
        int offset = (good_request) ? (1) : (-1);
        result->second += offset;

        if (result->second <= prog_options.blacklist_threshold) {
            assert (_blacklist.find (ipaddr) != _blacklist.end());

            _blacklist.emplace (result->first, result->second);
            LOG_S(INFO) << "Blacklisting IP address " << ipaddr;
        }
    }
}

static void
_get_statslist_from_db (
    int threshold)
{
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
            _blacklist.emplace (ipaddr, count);
            LOG_S(INFO) << "Blacklisting IP address " << ipaddr;
        }
    }

}