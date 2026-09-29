# Bank Management Application

A console-based Bank Management System written in C++ using object-oriented programming and file handling.

> **Note:** This is an educational banking simulation. It is not a real banking system and does not provide real-world security (there is no login, PIN or encryption).

## Objective

To simulate basic banking operations (creating accounts, deposits, withdrawals and balance checks) and keep the account records saved in a file so they are available the next time the program runs.

## Features

- Create an account (customer name, account number, account type, initial deposit)
- Deposit money
- Withdraw money (overdrawing is not allowed)
- Check balance
- Display account details
- Data is saved to a file and loaded again when the program starts
- Validation for invalid menu choices, invalid names, duplicate account numbers, accounts that do not exist, zero/negative/invalid amounts and insufficient balance

## Technologies Used

- C++ (C++17)
- Standard library only (`iostream`, `fstream`, `sstream`, `iomanip`, `string`, `vector`)

## OOP Concepts Used

- **Classes and objects:** `BankAccount` (one account) and `Bank` (manages all accounts)
- **Encapsulation:** account data is private and accessed through public functions
- **Constructors:** used to create account objects
- **Member functions:** `deposit()`, `withdraw()`, `display()` and others

## File Handling Concepts Used

- `ifstream` to read saved accounts when the program starts
- `ofstream` to write all accounts back to the file after every change
- Reading the file line by line with `getline`, and splitting each line at the `|` character
- Checking that a file opened successfully

## How Data Persistence Works

- All accounts are stored in a text file named `accounts.txt`, one account per line:

```
101|John Smith|Savings|750.00
102|Asha Rao|Current|1005.00
```

  The fields are: account number, customer name, account type, balance.
- When the program starts, it reads `accounts.txt` and loads every account into memory (a `vector` of `BankAccount` objects).
- After every change (new account, deposit or withdrawal), the whole file is rewritten from the vector, so the file always matches the current data.
- `accounts.txt` is **created automatically** if it does not exist. You do not need to create it yourself. It is listed in `.gitignore` so account data is not uploaded to GitHub.
- The file is created in the folder you run the program from, so run the program from the project folder.
- Lines in the file that are invalid or duplicated are ignored, and a warning is shown.

## How to Compile

```bash
g++ -std=c++17 bank_management.cpp -o bank_management
```

## How to Run

```bash
./bank_management
```

On Windows:

```bash
bank_management.exe
```

## How to Use the Menu

1. **Create Account:** enter the customer name (letters and spaces only), a new account number (1-999999), the account type (Savings or Current) and an initial deposit greater than 0.
2. **Deposit Money:** enter the account number and an amount greater than 0.
3. **Withdraw Money:** enter the account number and an amount greater than 0. The amount cannot be more than the current balance.
4. **Check Balance:** enter the account number to see the customer name and current balance.
5. **Display Account Details:** enter the account number to see the account number, name, type and balance.
6. **Exit:** closes the program. Data is already saved.

Amounts can have up to 2 decimal places, and the maximum amount for one transaction is 10,000,000.

## Expected Behavior

- Invalid input shows an error message and asks again.
- Using an account number that does not exist shows "not found".
- If no accounts exist yet, the operations ask you to create an account first.
- If you close and reopen the program, your accounts and balances are still there.
- If the data file cannot be saved, an error message is shown.
