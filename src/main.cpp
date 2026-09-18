#include <SQLiteCpp/SQLiteCpp.h>

#include <CLI/CLI.hpp>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>

#include "SQLiteCpp/Database.h"
#include "SQLiteCpp/Statement.h"

const std::string DEFAULT_DATABASE_PATH = "financial_records.db";
const std::string INIT_MIGRATION = "migrations/001_init.sql";

struct TableVerificationRow {
  std::string name;
  std::string type;
  int not_null;
  std::optional<std::string> default_value;
  int primary_key;
};

struct AccountsTableVerification {
  TableVerificationRow id{"id", "INTEGER", 0, std::nullopt, 1

  };
  TableVerificationRow name{"name", "TEXT", 1, std::nullopt, 0

  };
  TableVerificationRow type{"type", "TEXT", 1, std::nullopt, 0

  };
  TableVerificationRow currency{"currency", "TEXT", 1, std::nullopt, 0

  };
};

using TableVerificationMap = std::unordered_map<std::string, TableVerificationRow>;

AccountsTableVerification accounts_table_verification;
TableVerificationMap accounts_table_verification_map = {
    {"id", accounts_table_verification.id},
    {"name", accounts_table_verification.name},
    {"type", accounts_table_verification.type},
    {"currency", accounts_table_verification.currency},
};

struct CategoriesTableVerification {
  TableVerificationRow id{"id", "INTEGER", 0, std::nullopt, 1

  };
  TableVerificationRow name{"name", "TEXT", 1, std::nullopt, 0

  };
  TableVerificationRow type{"type", "TEXT", 1, std::nullopt, 0

  };
};

CategoriesTableVerification categories_table_verification;
TableVerificationMap categories_table_verification_map = {
    {"id", categories_table_verification.id},
    {"name", categories_table_verification.name},
    {"type", categories_table_verification.type},
};

struct TransactionsTableVerification {
  TableVerificationRow id{"id", "INTEGER", 0, std::nullopt, 1

  };
  TableVerificationRow account_id{"account_id", "INTEGER", 0, std::nullopt, 0

  };
  TableVerificationRow category_id{"category_id", "INTEGER", 0, std::nullopt, 0

  };
  TableVerificationRow amount{"amount", "REAL", 1, std::nullopt, 0

  };
  TableVerificationRow type{"type", "TEXT", 1, std::nullopt, 0

  };
  TableVerificationRow note{"note", "TEXT", 0, std::nullopt, 0

  };
  TableVerificationRow date{"date", "TEXT", 1, std::nullopt, 0

  };
};

TransactionsTableVerification transactions_table_verification;
TableVerificationMap transactions_table_verification_map = {
    {"id", transactions_table_verification.id},
    {"account_id", transactions_table_verification.account_id},
    {"category_id", transactions_table_verification.category_id},
    {"amount", transactions_table_verification.amount},
    {"type", transactions_table_verification.type},
    {"note", transactions_table_verification.note},
    {"date", transactions_table_verification.date},
};

std::unordered_map<std::string, TableVerificationMap> schema_verification_map = {
    {"accounts", accounts_table_verification_map},
    {"categories", categories_table_verification_map},
    {"transactions", transactions_table_verification_map}};

bool compare_table_verification_row(TableVerificationRow& a, TableVerificationRow& b) {
  return !(a.name != b.name || a.type != b.type || a.not_null != b.not_null || a.default_value != b.default_value ||
           a.primary_key != b.primary_key);
}

struct TableForeignKeyVerificationRow {
  int seq;
  std::string table;
  std::string from;
  std::string to;
  std::string on_update;
  std::string on_delete;
  std::string match;
};

struct TransactionsTableForeignKeyVerification {
  TableForeignKeyVerificationRow categories{0, "categories", "category_id", "id", "NO ACTION", "SET NULL", "NONE"};
  TableForeignKeyVerificationRow accounts{0, "accounts", "account_id", "id", "NO ACTION", "SET NULL", "NONE"};
};

using TableForeignKeyVerificationMap = std::unordered_map<std::string, TableForeignKeyVerificationRow>;

TransactionsTableForeignKeyVerification transactions_table_foreign_key_verification;
TableForeignKeyVerificationMap transactions_table_foreign_key_verification_map = {
    {"categories", transactions_table_foreign_key_verification.categories},
    {"accounts", transactions_table_foreign_key_verification.accounts},
};

bool compare_table_foreign_key_verification_row(TableForeignKeyVerificationRow& a, TableForeignKeyVerificationRow& b) {
  return !(a.seq != b.seq || a.table != b.table || a.from != b.from || a.to != b.to || a.on_update != b.on_update ||
           a.on_delete != b.on_delete || a.match != b.match);
}

std::unordered_map<std::string, TableForeignKeyVerificationMap> foreign_key_verification_map = {
    {"transactions", transactions_table_foreign_key_verification_map}};

bool run_database_migration(SQLite::Database& db) {
  std::ifstream file(INIT_MIGRATION);

  if (!file) {
    std::cerr << "Cannot find migration file: " << INIT_MIGRATION << '\n';
    return false;
  }

  std::string init_query((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  try {
    SQLite::Transaction transaction(db);

    db.exec(init_query);

    transaction.commit();

    std::cout << "Migration completed successfully\n";
    return true;
  } catch (const SQLite::Exception& e) {
    std::cerr << "Migration failed: " << e.what() << '\n';
    return false;
  }
}

bool verify_table(SQLite::Database& db, std::string table_name) {
  SQLite::Statement verify(db, "PRAGMA table_info(" + table_name + ");");
  std::cout << "Verifying " << table_name << "\n";

  if (!schema_verification_map.contains(table_name)) {
    std::cerr << "Failed Verifying " << table_name << "\n";
    return false;
  }

  TableVerificationMap expected_table_map = schema_verification_map[table_name];

  TableVerificationRow table_verification;
  while (verify.executeStep()) {
    table_verification.name = verify.getColumn(1).getString();
    table_verification.type = verify.getColumn(2).getString();
    table_verification.not_null = verify.getColumn(3);
    if (verify.getColumn(4).isNull()) {
      table_verification.default_value = std::nullopt;
    } else {
      table_verification.default_value = verify.getColumn(4).getString();
    }
    table_verification.primary_key = verify.getColumn(5);

    if (!expected_table_map.contains(table_verification.name)) {
      std::cerr << "Failed Verifying " << table_name << "\n";
      return false;
    }

    if (!compare_table_verification_row(expected_table_map[table_verification.name], table_verification)) {
      std::cerr << "Failed Verifying " << table_name << "\n";
      return false;
    }
  }

  return true;
}

bool verify_foreign_key_table(SQLite::Database& db, std::string table_name) {
  SQLite::Statement verify_transactions_foreign_keys(db, "PRAGMA foreign_key_list(" + table_name + ");");
  std::cout << "Verifying " << table_name << " foreign key\n";

  if (!foreign_key_verification_map.contains(table_name)) {
    std::cerr << "Failed Verifying " << table_name << " foreign key\n";
    return false;
  }

  TableForeignKeyVerificationMap expected_table_map = foreign_key_verification_map[table_name];

  TableForeignKeyVerificationRow table_verification;
  while (verify_transactions_foreign_keys.executeStep()) {
    table_verification.seq = verify_transactions_foreign_keys.getColumn(1);
    table_verification.table = verify_transactions_foreign_keys.getColumn(2).getString();
    table_verification.from = verify_transactions_foreign_keys.getColumn(3).getString();
    table_verification.to = verify_transactions_foreign_keys.getColumn(4).getString();
    table_verification.on_update = verify_transactions_foreign_keys.getColumn(5).getString();
    table_verification.on_delete = verify_transactions_foreign_keys.getColumn(6).getString();
    table_verification.match = verify_transactions_foreign_keys.getColumn(7).getString();

    if (!expected_table_map.contains(table_verification.table)) {
      std::cerr << "Failed Verifying " << table_name << " foreign key\n";
      return false;
    }

    if (!compare_table_foreign_key_verification_row(expected_table_map[table_verification.table], table_verification)) {
      std::cerr << "Failed Verifying " << table_name << " foreign key\n";
      return false;
    }
  }

  return true;
}

bool run_database_verification(SQLite::Database& db) {
  try {
    return verify_table(db, "accounts") && verify_table(db, "categories") && verify_table(db, "transactions") &&
           verify_foreign_key_table(db, "transactions");
  } catch (const SQLite::Exception& e) {
    std::cerr << "Migration failed: " << e.what() << '\n';
    return false;
  }
}

int main(int argc, char** argv) {
  CLI::App app;

  std::string database_path;
  app.add_option("--records", database_path, "This is the path to save the financial records");
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
    SQLite::Database db(database_path, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

    if (init_migration and !run_database_migration(db)) return 1;

    if (!run_database_verification(db)) return 1;

  } catch (const std::exception& exp) {
    std::cerr << "Exception: " << exp.what() << '\n';
    return 1;
  }

  return 0;
}
