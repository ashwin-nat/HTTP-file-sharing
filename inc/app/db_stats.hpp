#ifndef __DB_STATS_HPP__
#define __DB_STATS_HPP__

#include <string>
#include "http_status.hpp"

void 
update_db_stats (
    const std::string &srcaddr, 
    HTTPStatus status);

#endif  //__DB_STATS_HPP__