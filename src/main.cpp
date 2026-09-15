#include "SQLiteCpp/Database.h"
#include "SQLiteCpp/Statement.h"
#include <CLI/CLI.hpp>
#include <SQLiteCpp/SQLiteCpp.h>
#include <iostream>
#include <string>

const std::string DEFAULT_DATABASE_PATH = "financial_records.db";
const std::string INIT_MIGRATION = "migrations/001_init.sql";

struct TableVerification {
  int cid;
  std::string name;
  int type;
  int not_null;
  std::string default_value;
  int primary_key;
};

std::ostream &operator<<(std::ostream &os, const TableVerification &tv) {
  os << "row: " << tv.cid << " ";
  os << tv.name << " ";
  os << tv.type << " ";
  os << tv.not_null << " ";
  os << tv.default_value << " ";
  os << tv.primary_key << '\n';

  return os;
}

struct TableForiegnKeyVerification {
  int id;
  int seq;
  std::string table;
  std::string from;
  std::string to;
  std::string on_update;
  std::string on_delete;
  std::string match;
};

std::ostream &operator<<(std::ostream &os,
                         const TableForiegnKeyVerification &tf) {
  os << "row: " << tf.id << " ";
  os << tf.seq << " ";
  os << tf.table << " ";
  os << tf.from << " ";
  os << tf.to << " ";
  os << tf.on_update << " ";
  os << tf.on_delete << " ";
  os << tf.match << '\n';

  return os;
}

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

void verify_table(SQLite::Database &db, std::string table_name) {
  SQLite::Statement verify(db, "PRAGMA table_info(" + table_name + ");");
  std::cout << "Verifying " << table_name << "\n";
  std::cout << "header: " << "cid " << "name " << "type " << "not_null "
            << "default_value " << "primary_key " << '\n';
  TableVerification table_verification;
  while (verify.executeStep()) {
    table_verification.cid = verify.getColumn(0);
    table_verification.name = verify.getColumn(1).getString();
    table_verification.type = verify.getColumn(2);
    table_verification.not_null = verify.getColumn(3);
    table_verification.default_value = verify.getColumn(4).getString();
    table_verification.primary_key = verify.getColumn(5);

    std::cout << "row: " << table_verification << " ";
  }
  std::cout << '\n';
}

void verify_foreign_key_table(SQLite::Database &db, std::string table_name) {
  SQLite::Statement verify_transactions_foreign_keys(
      db, "PRAGMA foreign_key_list(" + table_name + ");");
  std::cout << "Verifying " << table_name << " foreign key\n";
  std::cout << "header: " << "id " << "seq " << "table " << "from "
            << "to " << "on_update " << "on_delete " << "match" << '\n';
  TableForiegnKeyVerification table_verification;
  while (verify_transactions_foreign_keys.executeStep()) {
    table_verification.id = verify_transactions_foreign_keys.getColumn(0);
    table_verification.seq = verify_transactions_foreign_keys.getColumn(1);
    table_verification.table =
        verify_transactions_foreign_keys.getColumn(2).getString();
    table_verification.from =
        verify_transactions_foreign_keys.getColumn(3).getString();
    table_verification.to =
        verify_transactions_foreign_keys.getColumn(4).getString();
    table_verification.on_update =
        verify_transactions_foreign_keys.getColumn(5).getString();
    table_verification.on_delete =
        verify_transactions_foreign_keys.getColumn(6).getString();
    table_verification.match =
        verify_transactions_foreign_keys.getColumn(7).getString();
    std::cout << "row: " << table_verification << " ";
  }
  std::cout << '\n';
}

bool run_database_verification(SQLite::Database &db) {
  try {
    verify_table(db, "accounts");
    verify_table(db, "categories");
    verify_table(db, "transactions");
    verify_foreign_key_table(db, "transactions");

    std::cout << "Verification completed successfully\n";
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

    if (init_migration and !run_database_migration(db))
      return 1;

    if (!run_database_verification(db))
      return 1;

    // Render TUI
  } catch (const std::exception &exp) {
    std::cerr << "Exception: " << exp.what() << '\n';
    return 1;
  }

  return 0;
}
