#include <CLI/CLI.hpp>
#include <SQLiteCpp/SQLiteCpp.h>
#include <iostream>
#include <string>

const std::string DEFAULT_DATABASE_PATH = "financial_records.db";
const std::string INIT_MIGRATION = "migrations/init.sql";

int main(int argc, char **argv) {
  CLI::App app;

  std::string database_path;
  app.add_option("--records", database_path,
                 "This is the path to save the financial records");
  CLI11_PARSE(app, argc, argv);

  bool init_migration = false;
  if (database_path.empty()) {
    init_migration = true;
    database_path = DEFAULT_DATABASE_PATH;
  }

  try {
    SQLite::Database db(database_path,
                        SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

    // Run migration
    if (init_migration) {
      std::ifstream file(INIT_MIGRATION);

      if (!file) {
        std::cerr << "Cannot find migration file: " << INIT_MIGRATION << '\n';
        return 1;
      }

      std::string init_query((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());

      try {
        SQLite::Transaction transaction(db);

        db.exec(init_query);

        transaction.commit();

        std::cout << "Migration completed successfully\n";
      } catch (const SQLite::Exception &e) {
        std::cerr << "Migration failed: " << e.what() << '\n';
        return 1;
      }
    }

    SQLite::Statement query(db, "SELECT * FROM accounts;");

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
