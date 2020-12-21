#ifndef __BLACKLIST_HPP__
#define __BLACKLIST_HPP__

#include <string>

void init_blacklist (void);
bool is_ipaddr_blacklisted (const std::string &ipaddr);
void update_stats_list_blacklist (const std::string &ipaddr, bool good_request);

#endif  //__BLACKLIST_HPP__