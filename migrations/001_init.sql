-- Drop table if exist to have clean creation
DROP TABLE IF EXISTS transactions;
DROP TABLE IF EXISTS categories;
DROP TABLE IF EXISTS accounts;

-- Create tables which will be used in the app
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
    date TEXT NOT NULL,

    FOREIGN KEY (account_id)
        REFERENCES accounts(id)
        ON DELETE SET NULL,

    FOREIGN KEY (category_id)
        REFERENCES categories(id)
        ON DELETE SET NULL
) STRICT;

-- Create seed data for user to interact with
INSERT INTO accounts (
  name, type, currency
) VALUES ( 'Wallet', 'cash', 'USD' );
INSERT INTO accounts (
  name, type, currency
) VALUES ( 'Visa Card', 'credit_card', 'USD' );
INSERT INTO accounts (
  name, type, currency
) VALUES ( 'Master Card', 'credit_card', 'USD' );
INSERT INTO accounts (
  name, type, currency
) VALUES ( 'Bank Account', 'bank', 'USD' );

INSERT INTO categories (
  name, type
) VALUES ( 'Salary', 'income' );
INSERT INTO categories (
  name, type
) VALUES ( 'Groceries', 'expense' );
INSERT INTO categories (
  name, type
) VALUES ( 'Food', 'expense' );
INSERT INTO categories (
  name, type
) VALUES ( 'Dining', 'expense' );
INSERT INTO categories (
  name, type
) VALUES ( 'Rent', 'expense' );
INSERT INTO categories (
  name, type
) VALUES ( 'Utilities', 'expense' );
INSERT INTO categories (
  name, type
) VALUES ( 'Shopping', 'expense' );
INSERT INTO categories (
  name, type
) VALUES ( 'Travel', 'expense' );

