DROP TABLE IF EXISTS transactions;
DROP TABLE IF EXISTS categories;
DROP TABLE IF EXISTS accounts;

CREATE TABLE accounts (
    id INTEGER PRIMARY KEY,
    name TEXT NOT NULL,
    type TEXT NOT NULL,
    currency TEXT NOT NULL
) STRICT;

CREATE TABLE categories (
    id INTEGER PRIMARY KEY,
    name TEXT NOT NULL UNIQUE,
    type TEXT NOT NULL
) STRICT;

CREATE TABLE transactions (
    id INTEGER PRIMARY KEY,
    account_id INTEGER,
    category_id INTEGER,
    amount REAL NOT NULL,
    type TEXT NOT NULL,
    note TEXT,
    transaction_date TEXT NOT NULL,

    FOREIGN KEY (account_id)
        REFERENCES accounts(id)
        ON DELETE SET NULL,

    FOREIGN KEY (category_id)
        REFERENCES categories(id)
        ON DELETE SET NULL
) STRICT;
