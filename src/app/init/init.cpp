#include "init.hpp"
#include "loguru.hpp"
#include "db.hpp"
#include "prog_options.hpp"
#include <vector>
#include <tuple>
#include <map>

using db_col_info_t = std::tuple<std::string,std::string,std::string>;
using db_table_t = std::vector<db_col_info_t>;
using db_schema_t = std::map<std::string,db_table_t>;

extern ProgOptions prog_options;

static bool _check_add_missing_columns (db_handle &db);
static void _get_actual_db_schema (db_handle &db, db_schema_t &actual);
static void _get_db_tables_list (db_handle &db, 
    std::vector<std::string> &tables);
static bool _does_column_exist (db_table_t &curr_table_structure, 
    const std::string &col_name);
static bool _add_column (db_handle &db, const std::string &table_name, 
    db_col_info_t &cols);
static bool _create_table (db_handle &db, const std::string &table_name, 
    db_table_t &table_structure);
static bool _get_cols_for_table (db_handle &db, const std::string &table,
    db_table_t &cols);

void _get_expected_db_schema (db_schema_t &schema);


void 
init_loguru (
    int argc,
    char *argv[])
{
    loguru::init (argc, argv);
    if (prog_options.log_file.empty()) {
        prog_options.log_file = "http-server.log";
    }
    loguru::add_file(prog_options.log_file.c_str(), loguru::Append, 
            loguru::Verbosity_MAX);
}

void 
init_db (
    int argc,
    char *argv[])
{
    if (prog_options.db_file.empty()) {
        prog_options.db_file = "http-server.db";
    }

    //open db file
    db_handle db (prog_options.db_file);
    
    //add missing columns;
    if (_check_add_missing_columns (db) == false) {
        exit (1);
    }
}

static bool 
_check_add_missing_columns (
    db_handle &db)
{
    //get default tables and column info
    db_schema_t expected, actual;
    _get_expected_db_schema (expected);
    _get_actual_db_schema (db, actual);

    //check if all tables and columns in the expected schema exists in the 
    //actual schema
    for (auto &it : expected) {

        auto result = actual.find (it.first);
        if (result != actual.end()) {
            //the table exists, now check if all columns exist
            for (auto &cols : it.second) {
                //loop through all columns in expected schema, check if they 
                //exist in actual schema
                if (!_does_column_exist (result->second, std::get<0>(cols))) {
                    //add column to table
                    _add_column (db, it.first, cols);
                }
            }
        }
        else {
            //table does NOT exist, create it with all its columns
            LOG_S(INFO) << "Table " << it.first << " will be created";
            _create_table (db, it.first, it.second);
        }
    }

    return true;
}

void 
_get_actual_db_schema (
    db_handle &db, 
    db_schema_t &actual)
{
    //get a vector of table names
    std::vector <std::string> tables;
    _get_db_tables_list (db, tables);

    //get the schema for all tables
    for(auto &it : tables) {
        db_table_t temp;
        _get_cols_for_table (db, it, temp);
        actual.insert (std::pair<std::string,db_table_t>(it,temp));
    }
}

static void 
_get_db_tables_list (
    db_handle &db, 
    std::vector<std::string> &tables)
{
    //build the query
    const std::string query = "SELECT name FROM sqlite_master "
                "WHERE type='table'";
    db.prepare_stmt (query);
    std::string temp;
    
    //step through all results
    while (db.step() == SQLITE_ROW) {

        //add the table name to the vector
        temp.clear();
        db.get_str (0, temp);
        tables.push_back (temp);
    }
}

static bool 
_does_column_exist (
    db_table_t &curr_table_structure, 
    const std::string &col_name)
{
    //linear search
    for (auto &it : curr_table_structure) {
        
        if (std::get<0>(it) == col_name) {
            return true;
        }
    }
    return false;
}

static bool 
_add_column (
    db_handle &db, 
    const std::string &table_name, 
    db_col_info_t &cols)
{
    //prep the query
    std::string query = "ALTER TABLE " + table_name + " ADD ";
    query += std::get<0>(cols) + " " + std::get<1>(cols) + " " + 
        std::get<2>(cols);
    
    //execute the query
    if(db.exec_query (query)) {
        LOG_S(ERROR) << "Query " << query << " failed with error " << 
            db.get_errmsg();
        return false;
    }
    
    LOG_S(INFO) << "Added column " << std::get<0>(cols) << " (" << 
        std::get<1>(cols) << " " << std::get<2>(cols) << ") to table" <<
        table_name;
    return true;
}

static bool 
_create_table (
    db_handle &db, 
    const std::string &table_name, 
    db_table_t &table_structure)
{
    //prep the SQL query
    std::string query = "CREATE TABLE " + table_name + " (";
    bool is_first = true;

    for(auto &it : table_structure) {
        if(!is_first) {
            query += ", ";
        }
        query += std::get<0>(it) + " " + std::get<1>(it) + " " + 
                std::get<2>(it);
        // query += it.first + " " + it.second;
        is_first = false;
    }
    query += ");";
    //execute the query
    if(db.exec_query (query) != SQLITE_OK) {
        LOG_S(ERROR) << "Query " << query << " failed with error " 
            << db.get_errmsg ();;
        return false;
    }

    LOG_S(INFO) << "Table " << table_name << " was added to the DB";
    return true;
}

static bool 
_get_cols_for_table (
    db_handle &db, 
    const std::string &table,
    db_table_t &cols)
{
    std::string query = "PRAGMA table_info('" + table + "')";
    db.prepare_stmt (query);
    std::string name,type;
    while(db.step() == SQLITE_ROW) {
        name.clear();
        type.clear();
        db.get_str (1, name);
        db.get_str (2, type);

        //for now, lets leave the col constraints as blank
        cols.push_back (std::tuple<std::string,std::string,std::string> 
            (name, type, ""));
    }
    return true;
}