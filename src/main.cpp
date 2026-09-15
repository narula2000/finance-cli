#include "SQLiteCpp/Database.h"
#include "SQLiteCpp/Statement.h"
#include <CLI/CLI.hpp>
#include <SQLiteCpp/SQLiteCpp.h>
#include <iostream>
#include <string>

const std::string DEFAULT_DATABASE_PATH = "financial_records.db";
const std::string INIT_MIGRATION = "migrations/001_init.sql";

bool run_database_migration(SQLite::Database &db) {
  std::ifstream file(INIT_MIGRATION);

  if (!file) {
    std::cerr << "Cannot find migration file: " << INIT_MIGRATION << '\n';
    return false;
  }

  std::string init_query((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());

  try {
    SQLite::Transaction transaction(db);

    db.exec(init_query);

    transaction.commit();

    std::cout << "Migration completed successfully\n";
    return true;
  } catch (const SQLite::Exception &e) {
    std::cerr << "Migration failed: " << e.what() << '\n';
    return false;
  }
}

bool run_database_verification(SQLite::Database &db) {
  try {
    SQLite::Transaction transaction(db);

    SQLite::Statement verify_accounts(db, "PRAGMA table_info(accounts);");
    std::cout << "Verifying accounts\n";
    std::cout << "header: " << "cid " << "name " << "type " << "not_null "
              << "default_value " << "primary_key " << '\n';
    while (verify_accounts.executeStep()) {
      int cid = verify_accounts.getColumn(0);
      std::string name = verify_accounts.getColumn(1);
      int type = verify_accounts.getColumn(2);
      int not_null = verify_accounts.getColumn(3);
      std::string default_value = verify_accounts.getColumn(4);
      int primary_key = verify_accounts.getColumn(5);

      std::cout << "row: " << cid << " ";
      std::cout << name << " ";
      std::cout << type << " ";
      std::cout << not_null << " ";
      std::cout << default_value << " ";
      std::cout << primary_key << '\n';
    }
    std::cout << '\n';

    std::cout << "Verifying categories\n";
    SQLite::Statement verify_categories(db, "PRAGMA table_info(categories);");
    std::cout << "header: " << "cid " << "name " << "type " << "not_null "
              << "default_value " << "primary_key " << '\n';
    while (verify_categories.executeStep()) {
      int cid = verify_categories.getColumn(0);
      std::string name = verify_categories.getColumn(1);
      int type = verify_categories.getColumn(2);
      int not_null = verify_categories.getColumn(3);
      std::string default_value = verify_categories.getColumn(4);
      int primary_key = verify_categories.getColumn(5);

      std::cout << "row: " << cid << " ";
      std::cout << name << " ";
      std::cout << type << " ";
      std::cout << not_null << " ";
      std::cout << default_value << " ";
      std::cout << primary_key << '\n';
    }
    std::cout << '\n';

    SQLite::Statement verify_transactions(db,
                                          "PRAGMA table_info(transactions);");
    std::cout << "Verifying transactions\n";
    std::cout << "header: " << "cid " << "name " << "type " << "not_null "
              << "default_value " << "primary_key " << '\n';
    while (verify_transactions.executeStep()) {
      int cid = verify_transactions.getColumn(0);
      std::string name = verify_transactions.getColumn(1);
      int type = verify_transactions.getColumn(2);
      int not_null = verify_transactions.getColumn(3);
      std::string default_value = verify_transactions.getColumn(4);
      int primary_key = verify_transactions.getColumn(5);

      std::cout << "row: " << cid << " ";
      std::cout << name << " ";
      std::cout << type << " ";
      std::cout << not_null << " ";
      std::cout << default_value << " ";
      std::cout << primary_key << '\n';
    }
    std::cout << '\n';

    SQLite::Statement verify_transactions_foreign_keys(
        db, "PRAGMA foreign_key_list(transactions);");
    std::cout << "Verifying transactions foreign key\n";
    std::cout << "header: " << "id " << "seq " << "table " << "from "
              << "to " << "on_update " << "on_delete " << "match" << '\n';
    while (verify_transactions_foreign_keys.executeStep()) {
      int id = verify_transactions_foreign_keys.getColumn(0);
      int seq = verify_transactions_foreign_keys.getColumn(1);
      std::string table = verify_transactions_foreign_keys.getColumn(2);
      std::string from = verify_transactions_foreign_keys.getColumn(3);
      std::string to = verify_transactions_foreign_keys.getColumn(4);
      std::string on_update = verify_transactions_foreign_keys.getColumn(5);
      std::string on_delete = verify_transactions_foreign_keys.getColumn(6);
      std::string match = verify_transactions_foreign_keys.getColumn(7);
      std::cout << "row: " << id << " ";
      std::cout << seq << " ";
      std::cout << table << " ";
      std::cout << from << " ";
      std::cout << to << " ";
      std::cout << on_update << " ";
      std::cout << on_delete << " ";
      std::cout << match << '\n';
    }
    std::cout << '\n';

    std::cout << "Migration completed successfully\n";
    return true;
  } catch (const SQLite::Exception &e) {
    std::cerr << "Migration failed: " << e.what() << '\n';
    return false;
  }
}

int main(int argc, char **argv) {
  CLI::App app;

  std::string database_path;
  app.add_option("--records", database_path,
                 "This is the path to save the financial records");
  CLI11_PARSE(app, argc, argv);

  bool init_migration = false;
  if (database_path.empty()) {
    database_path = DEFAULT_DATABASE_PATH;
    std::ifstream file(database_path);

    if (!file) {
      init_migration = true;
    }
  }

  try {
    SQLite::Database db(database_path,
                        SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

    // Run migration
    if (init_migration) {
      if (!run_database_migration(db))
        return 1;

    } else {
      if (!run_database_verification(db))
        return 1;
    }

    // Render TUI
  } catch (const std::exception &exp) {
    std::cerr << "Exception: " << exp.what() << '\n';
    return 1;
  }

  return 0;
}
