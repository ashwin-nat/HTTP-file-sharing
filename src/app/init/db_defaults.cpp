#include <vector>
#include <map>
#include <tuple>
#include <string>

using db_col_info_t = std::tuple<std::string,std::string,std::string>;
using db_table_t = std::vector<db_col_info_t>;
using db_schema_t = std::map<std::string,db_table_t>;

static void _add_info_of_stats (db_schema_t &schema);
static void _add_info_of_blacklist (db_schema_t &schema);

void _get_expected_db_schema (db_schema_t &schema)
{
    _add_info_of_stats (schema);
    _add_info_of_blacklist (schema);
}

static void 
_add_info_of_stats (
    db_schema_t &schema)
{
    const std::string table_name = "stats";
    db_table_t cols = {
        {"ipaddr", "TEXT", "PRIMARY KEY"},
        {"count", "INTEGER", "DEFAULT 0"},
    };

    schema.insert (std::pair<std::string,db_table_t> (table_name, cols));
}

static void 
_add_info_of_blacklist (
    db_schema_t &schema)
{
    const std::string table_name = "blacklist";
    db_table_t cols = {
        {"ipaddr", "TEXT", "PRIMARY KEY"},
    };

    schema.insert (std::pair<std::string,db_table_t> (table_name, cols));
}