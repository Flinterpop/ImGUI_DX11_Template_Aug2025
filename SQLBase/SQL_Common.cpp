
#include <string>
#include "sqlite3.h"

#include "SQL_Common.h"


void DropTable(const char* DBname, const char* tablename)
{
	char* messaggeError;
	sqlite3* DB;

	std::string sql = "DROP TABLE " + std::string(tablename);

	int exit = sqlite3_open(DBname, &DB);
	if (exit != SQLITE_OK) {
		puts("Error: Could not openDB");
	}

	exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messaggeError);

	if (exit != SQLITE_OK) {
		printf("Error: %s\r\nCould not Drop Table %s\r\n", messaggeError, tablename);
		sqlite3_free(messaggeError);
	}
	else printf("Table %s dropped Successfully", tablename);

	sqlite3_close(DB);
}

int FindIDforItemInTable(const char* DBname, char * TableName, char* itemName)
{
	sqlite3* db;
	int rc = sqlite3_open(DBname, &db);
	if (rc)
	{
		printf("Can't open database: %s\r\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}

	char query[100];
	sprintf(query, "SELECT ID FROM  %s where name = '%s';", TableName, itemName);
	printf("Running: %s\r\n", query);
	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
		printf("SQL Prep error in FindIDforRouteName\r\n");
		sqlite3_close(db);
		return -1;
	}

	int val = -1;
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		//const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
		val = sqlite3_column_int(stmt, 0);
		printf("Retrieved Name : % s val : % d\r\n", itemName, val);
	}

	sqlite3_finalize(stmt);
	sqlite3_close(db);
	return val;
}

