#ifndef __BLACKLIST_HPP__
#define __BLACKLIST_HPP__

#include <string>

void init_blacklist (void);
bool is_ipaddr_blacklisted (const std::string &ipaddr);
bool update_stats_list_blacklist (const std::string &ipaddr, bool good_request);
bool get_score_for_ipaddr (const std::string &ipaddr, int score);

#endif  //__BLACKLIST_HPP__