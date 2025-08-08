

#include "sqlite3.h"
#include <filesystem>
#include <iostream>
#include "BGUtil.h"

#include "AppIni.h"

namespace fs = std::filesystem;

#ifdef RELEASE_VERSION
bool AppIni_Debug = false;
#else
bool AppIni_Debug = false;
#endif


char AppIni_dbName[120]; 

void bg_SetIniDBFileName(const char* name)
{
    strcpy(AppIni_dbName, name);
}


static bool AppIni_file_exists(const fs::path& p, fs::file_status s = fs::file_status{})
{
    if (fs::status_known(s) ? fs::exists(s) : fs::exists(p))
    {
        if (AppIni_Debug) {
            putsBlue("file exists: %s", p);
        }
        return true;
    }
    else
    {
        if (AppIni_Debug)
        {
            putsBlue("file does not exist: %s", p);
        }
        return false;
    }
}



//Create and intialize Application Database with 3 tables
bool bg_CreateAppIniIfDoesntExist()
{
    putsBlue("Creating new App Ini Data base %s", AppIni_dbName);

    const fs::path sandbox{ AppIni_dbName };
    if (AppIni_file_exists(sandbox))
    {
        putsGreen("\tDatabase %s already exists", AppIni_dbName);
        return false;
    }

    sqlite3* db;
    char* zErrMsg = 0;
    int rc = sqlite3_open(AppIni_dbName, &db);
    if (rc)
    {
        putsRed("\tCan't open database: %s", sqlite3_errmsg(db));
        sqlite3_close(db);
        return false;
    }

    char* errorMsg = nullptr;

    //Create Application  INT table
    std::string sql =
        "CREATE TABLE Integers ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT NOT NULL, "
        "value INTEGER NOT NULL"
        ");";

    if (sqlite3_exec(db, sql.c_str(), 0, 0, &errorMsg) != SQLITE_OK)
    {
        putsRed("\tSQLite error: %s", errorMsg);
        sqlite3_free(errorMsg);
        return false;
    }
    else putsBlue("\tCreated Integer Table");


    //Create Application  String table
    std::string sql2 =
        "CREATE TABLE Strings ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT NOT NULL, "
        "value TEXT NOT NULL"
        ");";

    if (sqlite3_exec(db, sql2.c_str(), 0, 0, &errorMsg) != SQLITE_OK)
    {
        putsRed("\tSQLite error: %s", errorMsg);
        sqlite3_free(errorMsg);
        return false;
    }
    else putsBlue("\tCreated Strings Table");


    //Create Application float table
    std::string sql3 =
        "CREATE TABLE REAL ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT NOT NULL, "
        "value REAL NOT NULL"
        ");";

    if (sqlite3_exec(db, sql3.c_str(), 0, 0, &errorMsg) != SQLITE_OK)
    {
        putsRed("\tSQLite error: %s", errorMsg);
        sqlite3_free(errorMsg);
        return false;
    }
    else putsBlue("\tCreated Reals Table");


    putsBlue("\tCreated DB: %s", AppIni_dbName);

    return true;
}




static sqlite3* openDB(const std::string& dbName) {
    sqlite3* db;
    if (sqlite3_open(dbName.c_str(), &db) == SQLITE_OK) 
    {
        return db;
    }
    putsRed("Could not open Database %s", dbName.c_str());
    return nullptr;
}

static bool createTable(sqlite3* db, const std::string& sql) {
    char* errorMsg = nullptr;
    if (sqlite3_exec(db, sql.c_str(), 0, 0, &errorMsg) != SQLITE_OK) {
        putsRed("\tSQLite error: %s  in createTable", errorMsg);
        sqlite3_free(errorMsg);
        return false;
    }
    if (AppIni_Debug) putsBlue("\tCreated table %s", sql.c_str());
    return true;
}

static bool closeDB(sqlite3* db)
{
    return sqlite3_close(db) == SQLITE_OK;
}

static bool executeSQL(sqlite3* db, const std::string& sql) {
    char* errorMsg = nullptr;
    int result = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errorMsg);
    if (errorMsg) {
        sqlite3_free(errorMsg);
        putsRed("\tError in executeSQL: %s", sql);
    }
    return result == SQLITE_OK;
}

//Not used in AppIni
static bool __prepareAndExecute(sqlite3* db, const std::string& sql)
{
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) 
    {
        putsRed("\tError in __prepareAndExecute with sql: %s", sql.c_str());
        return false;
    }
    while (sqlite3_step(stmt) == SQLITE_ROW) 
    {
        // Process each row, for example:
        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        int age = sqlite3_column_int(stmt, 1);
        if (AppIni_Debug) putsBlue("\tName: %s , Age: %d", name, age);
    }
    sqlite3_finalize(stmt);
    return true;
}



//also used for bools which are stored as Integers
static bool DoesIntegerExistInIntegersTable(char* name)
{
    sqlite3* db;
    int rc = sqlite3_open(AppIni_dbName, &db);
    if (rc)
    {
        //if (AppIni_Debug) 

        putsRed("Can't open database: %s", sqlite3_errmsg(db));
        sqlite3_close(db);
        return false;
    }

    char query[100];
    snprintf(query, 100,"SELECT exists(SELECT 1 FROM integers WHERE name = '%s') AS row_exists;", name);

    if (AppIni_Debug) putsBlue("Running: %s", query);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        //if (AppIni_Debug) 

        putsRed("Error in SQL Prep in DoesIntegerExistInIntegersTable");
        sqlite3_close(db);
        return false;
    }

    bool val = false;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        val = sqlite3_column_int(stmt, 0);

        if (AppIni_Debug) putsBlue("DoesIntExist?: %s val: %s", name, val == true ? "Yes" : "No");
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return val;
}

static bool DoesRealExistInRealsTable(char* name)
{
    sqlite3* db;
    int rc = sqlite3_open(AppIni_dbName, &db);
    if (rc)
    {
        //if (AppIni_Debug) 

        putsRed("Can't open database: %s", sqlite3_errmsg(db));
        sqlite3_close(db);
        return false;
    }

    char query[100];
    sprintf(query, "SELECT exists(SELECT 1 FROM real WHERE name = '%s') AS row_exists;", name);

    if (AppIni_Debug) putsBlue("Running: %s", query);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        //if (AppIni_Debug) 

        putsRed("Error in SQL Prep in DoesRealExistInRealsTable");
        sqlite3_close(db);
        return false;
    }

    bool val = false;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        val = sqlite3_column_int(stmt, 0);
        if (AppIni_Debug) putsBlue("DoesRealExist?: %s val: %s", name, val == true ? "Yes" : "No");
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return val;
}



static bool DoesStringExistInStringsTable(char* name)
{
    sqlite3* db;
    int rc = sqlite3_open(AppIni_dbName, &db);
    if (rc)
    {
        //if (AppIni_Debug) 

        putsRed("Can't open database: %s", sqlite3_errmsg(db));
        sqlite3_close(db);
        return false;
    }

    char query[100];
    sprintf(query, "SELECT exists(SELECT 1 FROM Strings WHERE name = '%s') AS row_exists;", name);

    if (AppIni_Debug) putsBlue("Running: %s", query);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        //if (AppIni_Debug) 

        putsRed("Error in SQL Prep in DoesStringExistInStringsTable");
        sqlite3_close(db);
        return false;
    }

    bool val = false;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        val = sqlite3_column_int(stmt, 0);
        if (AppIni_Debug) putsBlue("\tDoesStringExist?: %s val: %s", name, val == true ? "Yes" : "No");
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return val;
}



//INTEGER
int GetIniInt(char* name, int _default)
{
    putsGreen("\tGetIniInt Line %d", __LINE__);
    sqlite3* db;
    int rc = sqlite3_open(AppIni_dbName, &db);
    if (rc)
    {
        //if (AppIni_Debug) 
        putsRed("\tCan't open database : %s  in GetIniInt", sqlite3_errmsg(db));
        sqlite3_close(db);
        return _default;
    }

    char query[100];
    sprintf(query, "SELECT VALUE FROM INTEGERS WHERE NAME = '%s';", name);
    if (AppIni_Debug) putsBlue("\tRunning: %s", query);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        //if (AppIni_Debug) 
        putsRed("\tError in SQL Prep error in GetIniInt");
        sqlite3_close(db);
        return _default;
    }

    int val = _default;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        //const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        val = sqlite3_column_int(stmt, 0);
        if (AppIni_Debug) putsBlue("\tRetrieved Name : % s val : % d", name, val);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return val;
}

static bool SetIniInt(char* name, int val)
{
    sqlite3* db;
    int rc = sqlite3_open(AppIni_dbName, &db);
    if (rc)
    {
        //if (AppIni_Debug) 
        putsRed("\tCan't open database: %s in SetIniInt", sqlite3_errmsg(db));
        sqlite3_close(db);
        return false;
    }

    char query[100];
    bool insert_successful = true;
    sprintf(query, "INSERT INTO Integers (NAME, VALUE ) VALUES('%s', %d);", name, val);
    if (AppIni_Debug) putsBlue("\tRunning: %s", query);
    insert_successful &= executeSQL(db, query);


    if (insert_successful)
    {
        if (AppIni_Debug) putsBlue("\tInteger insertion complete");
        sqlite3_close(db);
        return true;
    }
    else
    {
        //if (AppIni_Debug) 
        putsRed("\tInteger insertion failed");
        sqlite3_close(db);
        return false;
    }
}

static bool _UpdateIniInt(char* name, int val)
{
    sqlite3* db;
    int rc = sqlite3_open(AppIni_dbName, &db);
    if (rc)
    {
        //if (AppIni_Debug) 
        putsRed("\tCan't open database: %s", sqlite3_errmsg(db));
        sqlite3_close(db);
        return false;
    }

    char query[100];
    bool insert_successful = true;

    sprintf(query, "UPDATE Integers SET VALUE = %d WHERE NAME = '%s';", val, name);
    if (AppIni_Debug) putsBlue("\tRunning: %s", query);
    insert_successful &= executeSQL(db, query);


    if (insert_successful)
    {
        if (AppIni_Debug) putsBlue("\tInteger Update complete");
        sqlite3_close(db);
        return true;
    }
    else
    {
        //if (AppIni_Debug) 
        putsRed("\tInteger Update failed");
        sqlite3_close(db);
        return false;
    }
}

bool UpdateIniInt(char* name, int val)
{
    if (false == DoesIntegerExistInIntegersTable(name))
    {
        bool retVal =  SetIniInt(name, val);
        //if (false == retVal) 
        putsRed("\tFailed to SetIniInt %s", name);
        return retVal;

    }
    int retVal = _UpdateIniInt(name, val);
    if (false == retVal) putsRed("\tFailed to UpdateIniInt %s", name);

    return retVal;
}


//BOOL
int GetIniBool(char* name, bool _default)
{
    sqlite3* db;
    int rc = sqlite3_open(AppIni_dbName, &db);
    if (rc)
    {
        //if (AppIni_Debug) 
        putsRed("\tCan't open database: %s", sqlite3_errmsg(db));
        sqlite3_close(db);
        return _default;
    }

    char query[100];
    sprintf(query, "SELECT VALUE FROM INTEGERS WHERE NAME = '%s';", name);
    if (AppIni_Debug) putsBlue("\tRunning: %s", query);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        //if (AppIni_Debug) 
        putsRed("\tSQL Prep error in GetIniBool: %s", query);
        sqlite3_close(db);
        return _default;
    }

    int val = _default;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        //const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        val = sqlite3_column_int(stmt, 0);
        if (AppIni_Debug) putsBlue("\tRetrived Bool as Int: Name: %s val: %d -> %s", name, val, val == 1 ? "true" : "false");
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return (0==val)?false:true;
}

static bool SetIniBool(char* name, bool _val)
{
    bool val=false;
    if (_val != 0) val = true;

    sqlite3* db;
    int rc = sqlite3_open(AppIni_dbName, &db);
    if (rc)
    {
        //if (AppIni_Debug) 
        putsRed("\tCan't open database: %s in SetIniBool", sqlite3_errmsg(db));
        sqlite3_close(db);
        return false;
    }

    char query[100];
    bool insert_successful = true;
    sprintf(query, "INSERT INTO Integers(NAME, VALUE ) VALUES('%s', %d);", name, val);
    if (AppIni_Debug) putsBlue("\tRunning: %s", query);
    insert_successful &= executeSQL(db, query);


    if (insert_successful)
    {
        if (AppIni_Debug) putsBlue("\tBool as Int insertion complete");
        sqlite3_close(db);
        return true;
    }
    else
    {
        //if (AppIni_Debug) 
        putsRed("\tBool as Int insertion failed");
        sqlite3_close(db);
        return false;
    }
}

static bool _UpdateIniBool(char* name, bool _val)
{
    bool val = false;
    if (_val != 0) val = true;

    sqlite3* db;
    int rc = sqlite3_open(AppIni_dbName, &db);
    if (rc)
    {
        if (AppIni_Debug) putsRed("\tCan't open database: %s", sqlite3_errmsg(db));
        sqlite3_close(db);
        return false;
    }

    char query[100];
    bool insert_successful = true;

    sprintf(query, "UPDATE Integers SET VALUE = '%d' WHERE NAME = '%s';", val, name);
    if (AppIni_Debug) putsBlue("\tRunning: %s", query);
    insert_successful &= executeSQL(db, query);

     if (insert_successful)
    {
         if (AppIni_Debug) putsBlue("\tUpdate bool as Int insertion complete");
        sqlite3_close(db);
        return true;
    }
    else
    {
         if (AppIni_Debug) putsRed("\tUpdate bool as Int failed");
        sqlite3_close(db);
        return false;
    }
}

bool UpdateIniBool(char* name, bool val)
{
    if (false == DoesIntegerExistInIntegersTable(name))
    {
        bool retVal = SetIniInt(name, val);
        if (false == retVal) putsRed("\tFailed to SetIniBool %s", name);
        return retVal;
    }
    bool retVal = _UpdateIniBool(name, val);
    if (false == retVal) putsRed("\tFailed to UpdateIniBools %s", name);
    return retVal;
}


double GetIniReal(char* name, double _default)
{
    sqlite3* db;
    int rc = sqlite3_open(AppIni_dbName, &db);
    if (rc)
    {
        if (AppIni_Debug) putsRed("\tCan't open database: %s", sqlite3_errmsg(db));
        sqlite3_close(db);
        return _default;
    }

    char query[100];
    sprintf(query, "SELECT VALUE FROM REAL WHERE NAME = '%s';", name);
    if (AppIni_Debug)  putsBlue("\tRunning: %s", query);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        if (AppIni_Debug) putsRed("\tSQL Prep error in GetIniReal");
        sqlite3_close(db);
        return _default;
    }

    double val = _default;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        //const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        val = sqlite3_column_double(stmt, 0);
        if (AppIni_Debug) putsBlue("\tRetrieved REAL Name: %s val: %f", name, val);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
       
    putsBlue("\tReturning %s: %f", name,val);
    return val;
}

static bool SetIniReal(char* name, double val)
{
    sqlite3* db;
    int rc = sqlite3_open(AppIni_dbName, &db);
    if (rc)
    {
        if (AppIni_Debug) putsRed("\tCan't open database for Real Insert: %s", sqlite3_errmsg(db));
        sqlite3_close(db);
        return false;
    }

    char query[100];
    bool insert_successful = true;
    sprintf(query, "INSERT INTO REAL(NAME, VALUE ) VALUES('%s', %f);", name, val);
    if (AppIni_Debug) putsBlue("\tRunning: %s", query);
    insert_successful &= executeSQL(db, query);


    if (insert_successful)
    {
        if (AppIni_Debug) putsBlue("\tReal Data insertion complete");
        sqlite3_close(db);
        return true;
    }
    else
    {
        if (AppIni_Debug) putsRed("\tReal Data insertion failed");
        sqlite3_close(db);
        return false;
    }
}

static bool _UpdateIniReal(char* name, double val)
{
    sqlite3* db;
    int rc = sqlite3_open(AppIni_dbName, &db);
    if (rc)
    {
        if (AppIni_Debug) putsRed("\tCan't open database: %s", sqlite3_errmsg(db));
        sqlite3_close(db);
        return false;
    }

    char query[100];
    bool insert_successful = true;

    sprintf(query, "UPDATE REAL SET VALUE = '%f' WHERE NAME = '%s';", val, name);
    if (AppIni_Debug) putsBlue("\tRunning: %s", query);
    insert_successful &= executeSQL(db, query);


    if (insert_successful)
    {
        if (AppIni_Debug) putsBlue("\tReal update complete");
        sqlite3_close(db);
        return true;
    }
    else
    {
        if (AppIni_Debug) putsRed("\tReal update failed");
        sqlite3_close(db);
        return false;
    }
}

bool UpdateIniReal(char* name, double val)
{
    if (false == DoesRealExistInRealsTable(name))
    {
        bool retVal = SetIniReal(name, val);
        if (false == retVal) putsRed("\tFailed to SetIniReal %s", name);
        return retVal;
    }

    bool retVal = _UpdateIniReal(name, val);
    if (false == retVal) putsRed("\tFailed to UpdateIniReal %s", name);
    return retVal;
}



std::string GetIniString(char* name, std::string _default)
{
    sqlite3* db;
    int rc = sqlite3_open(AppIni_dbName, &db);
    if (rc)
    {
        if (AppIni_Debug) putsRed("\tCan't open database: %s", sqlite3_errmsg(db));
        sqlite3_close(db);
        return _default;
    }

    char query[100];
    sprintf(query, "SELECT VALUE FROM Strings WHERE NAME = '%s';", name);
    if (AppIni_Debug) putsBlue("\tRunning: %s", query);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        if (AppIni_Debug) putsRed("\tSQL Prep error in GetIniInt");
        sqlite3_close(db);
        return _default;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        if (AppIni_Debug) putsBlue("\tRetrieved String Value: %s", name);
        _default = std::string(name);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return _default;
}

static bool SetIniString(char* name, char* val)
{
    sqlite3* db;
    int rc = sqlite3_open(AppIni_dbName, &db);
    if (rc)
    {
        if (AppIni_Debug) putsRed("\tCan't open database: %s", sqlite3_errmsg(db));
        sqlite3_close(db);
        return false;
    }

    char query[100];
    bool insert_successful = true;
    sprintf(query, "INSERT INTO Strings (NAME, VALUE ) VALUES('%s', '%s');", name, val);
    if (AppIni_Debug) putsBlue("\tRunning: %s", query);
    insert_successful &= executeSQL(db, query);


    if (insert_successful)
    {
        if (AppIni_Debug) putsBlue("\tString insertion complete");
        sqlite3_close(db);
        return true;
    }
    else
    {
        if (AppIni_Debug) putsRed("\tString insertion failed");
        sqlite3_close(db);
        return false;
    }
}

static bool _UpdateIniString(char* name, char* val)
{
    sqlite3* db;
    int rc = sqlite3_open(AppIni_dbName, &db);
    if (rc)
    {
        if (AppIni_Debug) putsRed("\tCan't open database: %s", sqlite3_errmsg(db));
        sqlite3_close(db);
        return false;
    }

    char query[100];
    bool insert_successful = true;

    sprintf(query, "UPDATE Strings SET VALUE = '%s' WHERE NAME = '%s';", val, name);
    if (AppIni_Debug) putsBlue("\tRunning: %s", query);
    insert_successful &= executeSQL(db, query);


    if (insert_successful)
    {
        if (AppIni_Debug) putsBlue("\tString Update complete");
        sqlite3_close(db);
        return true;
    }
    else
    {
        if (AppIni_Debug) putsRed("\tString Update failed");
        sqlite3_close(db);
        return false;
    }
}

bool UpdateIniString(char* name, char * val)
{
    if (false == DoesStringExistInStringsTable(name))
    {
        bool retVal = SetIniString(name, val);
        if (false == retVal) putsRed("\tFailed to SetIniString %s", name);
        return retVal;
    }

    bool retVal = _UpdateIniString(name, val);
    if (false == retVal) putsRed("\tFailed to UpdateIniString %s", name);
    return retVal;
}