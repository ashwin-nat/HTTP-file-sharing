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
    db.start_transaction();

    //update the stats table
    const std::string query_stats = _build_stats_query (success);
    if (db.prepare_stmt (query_stats) != SQLITE_OK) {
        LOG_S(ERROR) << "Query " << query_stats << " failed in prepare_stmt." << 
            " error = " << db.get_errmsg();
        return;
    }

    if (db.bind_string (1, srcaddr) != SQLITE_OK) {
        LOG_S(ERROR) << "Query " << query_stats << " failed in bind. error = " << 
            db.get_errmsg();
        return;
    }

    if (db.step () != SQLITE_DONE) {
        LOG_S(ERROR) << "Query " << query_stats << " failed in step. error = " << 
            db.get_errmsg();
        return;
    }

    if(update_stats_list_blacklist (srcaddr, success) == false) {

        //if false is returned, then the ip addr is now blacklisted
        db.reset_stmt();
        const std::string blacklist_query = 
            "INSERT or IGNORE into blacklist(ipaddr) values (?)";
        
        if (db.prepare_stmt (blacklist_query) != SQLITE_OK) {
            LOG_S(ERROR) << "Query " << query_stats << 
                " failed in prepare_stmt." << " error = " << db.get_errmsg();
            return;
        }

        if (db.bind_string (1, srcaddr) != SQLITE_OK) {
            LOG_S(ERROR) << "Query " << query_stats << 
                " failed in bind. error = " << db.get_errmsg();
            return;
        }

        if (db.step () != SQLITE_DONE) {
            LOG_S(ERROR) << "Query " << query_stats << 
                " failed in step. error = " << db.get_errmsg();
            return;
        }
    }
    db.end_transaction();
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