#include <CLI/CLI.hpp>
#include <SQLiteCpp/SQLiteCpp.h>
#include <iostream>
#include <string>

int main(int argc, char **argv) {
  CLI::App app;

  std::string database_path;
  app.add_option("--records", database_path,
                 "This is the path to save the financial records");
  CLI11_PARSE(app, argc, argv);

  if (database_path.empty()) {
    // Create a sqlite file in current directory
    database_path = "financial_records.db";
  }

  try {
    SQLite::Database db(database_path,
                        SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

    SQLite::Statement query(db, "SELECT * FROM OutDummy;");

    while (query.executeStep()) {
      const int num_columns = query.getColumnCount();

      for (int i = 0; i < num_columns; ++i) {
        std::cout << query.getColumnName(i) << " = "
                  << (query.isColumnNull(i) ? "NULL"
                                            : query.getColumn(i).getText())
                  << '\n';
      }

      std::cout << '\n';
    }
  } catch (const std::exception &exp) {
    std::cerr << "SQLite exception: " << exp.what() << '\n';
    return 1;
  }

  return 0;
}
