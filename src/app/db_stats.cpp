#include "db_stats.hpp"
#include "db.hpp"
#include "prog_options.hpp"
#include "loguru.hpp"
#include "blacklist.hpp"

#define _INCREMENT_BY_ONE       "+1"
#define _DECREMENT_BY_ONE       "-1"

extern ProgOptions prog_options;

static std::string _build_stats_query (bool success);

void 
update_db_stats (
    const std::string &srcaddr, 
    HTTPStatus status)
{
    db_handle db(prog_options.db_file);
    bool success = (status == HTTPStatus::HTTP_OK) ? (true) : (false);

    const std::string query = _build_stats_query (success);
    if (db.prepare_stmt (query) != SQLITE_OK) {
        LOG_S(ERROR) << "Query " << query << " failed in prepare_stmt." << 
            " error = " << db.get_errmsg();
        return;
    }

    if (db.bind_string (1, srcaddr) != SQLITE_OK) {
        LOG_S(ERROR) << "Query " << query << " failed in bind. error = " << 
            db.get_errmsg();
        return;
    }

    if (db.step () != SQLITE_DONE) {
        LOG_S(ERROR) << "Query " << query << " failed in step. error = " << 
            db.get_errmsg();
        return;
    }

    const bool good_request = (status == HTTPStatus::HTTP_OK);
    update_stats_list_blacklist (srcaddr, good_request);
}

static std::string 
_build_stats_query (
    bool success)
{
    std::string ret = "INSERT INTO stats (ipaddr,count) values (?,";

    int offset = (success) ? (+1) : (-1);
    ret += std::to_string (offset) + ") ";

    ret += "ON CONFLICT(ipaddr) DO UPDATE SET count=count";
    ret += ((success) ? _INCREMENT_BY_ONE : _DECREMENT_BY_ONE);

    return ret;
}