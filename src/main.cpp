#include <CLI/CLI.hpp>
#include <iostream>
#include <sqlite3.h>
#include <string>

int callback(void *, int argc, char **argv, char **columnNames) {
  for (int i = 0; i < argc; ++i) {
    std::cout << columnNames[i] << " = " << (argv[i] ? argv[i] : "NULL")
              << '\n';
  }

  std::cout << '\n';
  return 0;
}

int main(int argc, char **argv) {
  CLI::App app;

  std::string database_path;
  app.add_flag("--records", database_path,
               "This is the path to save the financial records");
  CLI11_PARSE(app, argc, argv);

  sqlite3 *database;
  int connection_status;

  if (database_path.empty()) {
    // Create a sqlite file in current directory
    connection_status = sqlite3_open("financial_records.db", &database);
  } else {
    // Connect to sqlite file path
    connection_status = sqlite3_open(database_path.c_str(), &database);
  }

  if (connection_status != SQLITE_OK) {
    std::cerr << "Can not connect to the financial records\n";
    std::cerr << sqlite3_errmsg(database) << '\n';
    sqlite3_close(database);
    return 1;
  }

  const char *query = "SELECT * FROM OutDummy;";

  char *errorMessage = nullptr;

  if (sqlite3_exec(database, query, callback, nullptr, &errorMessage) !=
      SQLITE_OK) {
    std::cerr << "Can not run the query\n";
    std::cerr << sqlite3_errmsg(database) << '\n';
    sqlite3_free(errorMessage);
  }

  // Close the connection after program closure
  sqlite3_close(database);
  return 0;
}
