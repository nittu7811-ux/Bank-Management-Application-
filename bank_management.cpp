// Bank Management Application
// A console-based C++ program that simulates basic banking operations.
// Account data is saved in a text file (accounts.txt) so it is kept after the program closes.
// NOTE: This is an educational simulation and not a real banking system.

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

const string DATA_FILE = "accounts.txt";
const double MAX_AMOUNT = 10000000.0;  // largest amount allowed in one transaction

// ---------------------------------------------------------------
// Input helpers: keep asking until the user enters valid input
// ---------------------------------------------------------------

// Reads one line from the user. Exits cleanly if input ends (Ctrl+D / Ctrl+Z).
string readLine(const string &prompt) {
    string line;
    cout << prompt;
    if (!getline(cin, line)) {
        cout << "\nInput ended. Exiting program.\n";
        exit(0);
    }
    return line;
}

// Reads a whole number between min and max (inclusive).
int readNumber(const string &prompt, int min, int max) {
    while (true) {
        string input = readLine(prompt);
        bool valid = !input.empty() && input.length() <= 9;
        for (char ch : input) {
            if (ch < '0' || ch > '9') {
                valid = false;
            }
        }
        if (valid) {
            int number = stoi(input);
            if (number >= min && number <= max) {
                return number;
            }
        }
        cout << "Invalid input. Please enter a number between " << min << " and "
             << max << ".\n";
    }
}

// Reads a customer name. Only letters and spaces are allowed.
string readName(const string &prompt) {
    while (true) {
        string name = readLine(prompt);
        bool hasLetter = false;
        bool valid = true;
        for (char ch : name) {
            bool isLetter = (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
            if (isLetter) {
                hasLetter = true;
            } else if (ch != ' ') {
                valid = false;
            }
        }
        if (valid && hasLetter && name.length() <= 40) {
            return name;
        }
        cout << "Invalid name. Use only letters and spaces (maximum 40 characters).\n";
    }
}

// Reads a money amount such as 500 or 250.75 (at most 2 decimal places).
// The amount must be greater than 0 and not more than MAX_AMOUNT.
double readAmount(const string &prompt) {
    while (true) {
        string input = readLine(prompt);
        bool valid = !input.empty() && input.length() <= 12;
        int dots = 0;
        int decimals = 0;
        for (char ch : input) {
            if (ch == '.') {
                dots++;
            } else if (ch < '0' || ch > '9') {
                valid = false;
            } else if (dots == 1) {
                decimals++;
            }
        }
        if (dots > 1 || decimals > 2) {
            valid = false;
        }
        if (valid && input != ".") {
            double amount = stod(input);
            if (amount > 0 && amount <= MAX_AMOUNT) {
                return amount;
            }
        }
        cout << "Invalid amount. Enter a positive amount (up to 2 decimal places, "
             << "maximum " << static_cast<long long>(MAX_AMOUNT) << ").\n";
    }
}

// ---------------------------------------------------------------
// BankAccount class: stores the details of one account
// ---------------------------------------------------------------
class BankAccount {
private:
    int accountNumber;
    string holderName;
    string accountType;  // "Savings" or "Current"
    double balance;

public:
    BankAccount() {
        accountNumber = 0;
        balance = 0;
    }

    BankAccount(int number, string name, string type, double startBalance) {
        accountNumber = number;
        holderName = name;
        accountType = type;
        balance = startBalance;
    }

    int getAccountNumber() const { return accountNumber; }
    string getHolderName() const { return holderName; }
    string getAccountType() const { return accountType; }
    double getBalance() const { return balance; }

    void deposit(double amount) {
        balance = balance + amount;
    }

    // Returns false (and changes nothing) if the balance is not enough.
    bool withdraw(double amount) {
        if (amount > balance) {
            return false;
        }
        balance = balance - amount;
        return true;
    }

    // One account is stored as one line: number|name|type|balance
    string toFileLine() const {
        ostringstream line;
        line << accountNumber << "|" << holderName << "|" << accountType << "|"
             << fixed << setprecision(2) << balance;
        return line.str();
    }

    // Builds an account from a line of the file. Returns false if the line is invalid.
    bool fromFileLine(const string &line) {
        istringstream input(line);
        string numberText, name, type, balanceText;
        if (!getline(input, numberText, '|') || !getline(input, name, '|') ||
            !getline(input, type, '|') || !getline(input, balanceText)) {
            return false;
        }
        try {
            accountNumber = stoi(numberText);
            balance = stod(balanceText);
        } catch (...) {
            return false;
        }
        holderName = name;
        accountType = type;
        return accountNumber > 0 && !holderName.empty() && balance >= 0;
    }

    void display() const {
        cout << "Account Number : " << accountNumber << "\n";
        cout << "Customer Name  : " << holderName << "\n";
        cout << "Account Type   : " << accountType << "\n";
        cout << "Balance        : " << fixed << setprecision(2) << balance << "\n";
        cout << "----------------------------------------\n";
    }
};

// ---------------------------------------------------------------
// Bank class: keeps all accounts and saves/loads them from the file
// ---------------------------------------------------------------
class Bank {
private:
    vector<BankAccount> accounts;

    // Returns the position of the account in the vector, or -1 if not found.
    int findAccountIndex(int accountNumber) const {
        for (size_t i = 0; i < accounts.size(); i++) {
            if (accounts[i].getAccountNumber() == accountNumber) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    // Writes all accounts to the file (the old file content is replaced).
    bool saveToFile() const {
        ofstream file(DATA_FILE);
        if (!file) {
            cout << "Error: Could not save data to " << DATA_FILE << ".\n";
            return false;
        }
        for (size_t i = 0; i < accounts.size(); i++) {
            file << accounts[i].toFileLine() << "\n";
        }
        return true;
    }

    // Asks for an account number and returns its position, or -1 if it does not exist.
    int askForAccount() const {
        if (accounts.empty()) {
            cout << "No accounts exist yet. Please create an account first.\n";
            return -1;
        }
        int number = readNumber("Enter account number: ", 1, 999999);
        int index = findAccountIndex(number);
        if (index == -1) {
            cout << "Error: Account " << number << " not found.\n";
        }
        return index;
    }

public:
    // Reads all saved accounts from the file. Creates the file if it does not exist.
    void loadFromFile() {
        ifstream file(DATA_FILE);
        if (!file) {
            ofstream newFile(DATA_FILE);  // create an empty data file
            if (!newFile) {
                cout << "Warning: Could not create " << DATA_FILE
                     << ". Data will not be saved.\n";
            } else {
                cout << "No saved data found. A new file (" << DATA_FILE
                     << ") has been created.\n";
            }
            return;
        }

        string line;
        int skipped = 0;
        while (getline(file, line)) {
            if (line.empty()) {
                continue;
            }
            BankAccount account;
            if (account.fromFileLine(line) &&
                findAccountIndex(account.getAccountNumber()) == -1) {
                accounts.push_back(account);
            } else {
                skipped++;
            }
        }
        cout << "Loaded " << accounts.size() << " account(s) from " << DATA_FILE << ".\n";
        if (skipped > 0) {
            cout << "Warning: " << skipped << " invalid line(s) in the file were ignored.\n";
        }
    }

    void createAccount() {
        string name = readName("Enter customer name: ");
        int number = readNumber("Enter new account number (1-999999): ", 1, 999999);
        if (findAccountIndex(number) != -1) {
            cout << "Error: Account number " << number << " already exists.\n";
            return;
        }

        cout << "Account type:\n1. Savings\n2. Current\n";
        int typeChoice = readNumber("Select account type (1-2): ", 1, 2);
        string type = (typeChoice == 1) ? "Savings" : "Current";

        double deposit = readAmount("Enter initial deposit: ");

        accounts.push_back(BankAccount(number, name, type, deposit));
        if (saveToFile()) {
            cout << "Account created successfully. Account number: " << number << "\n";
        }
    }

    void depositMoney() {
        int index = askForAccount();
        if (index == -1) {
            return;
        }
        double amount = readAmount("Enter deposit amount: ");
        accounts[index].deposit(amount);
        if (saveToFile()) {
            cout << "Deposit successful. New balance: " << fixed << setprecision(2)
                 << accounts[index].getBalance() << "\n";
        }
    }

    void withdrawMoney() {
        int index = askForAccount();
        if (index == -1) {
            return;
        }
        double amount = readAmount("Enter withdrawal amount: ");
        if (!accounts[index].withdraw(amount)) {
            cout << "Error: Insufficient balance. Current balance: " << fixed
                 << setprecision(2) << accounts[index].getBalance() << "\n";
            return;
        }
        if (saveToFile()) {
            cout << "Withdrawal successful. New balance: " << fixed << setprecision(2)
                 << accounts[index].getBalance() << "\n";
        }
    }

    void checkBalance() const {
        int index = askForAccount();
        if (index == -1) {
            return;
        }
        cout << "\nAccount Number : " << accounts[index].getAccountNumber() << "\n";
        cout << "Customer Name  : " << accounts[index].getHolderName() << "\n";
        cout << "Current Balance: " << fixed << setprecision(2)
             << accounts[index].getBalance() << "\n";
    }

    void displayAccountDetails() const {
        int index = askForAccount();
        if (index == -1) {
            return;
        }
        cout << "\n----------------------------------------\n";
        accounts[index].display();
    }
};

// ---------------------------------------------------------------
// Menu
// ---------------------------------------------------------------
void showMenu() {
    cout << "\n========================================\n";
    cout << "         BANK MANAGEMENT SYSTEM\n";
    cout << "========================================\n";
    cout << "1. Create Account\n";
    cout << "2. Deposit Money\n";
    cout << "3. Withdraw Money\n";
    cout << "4. Check Balance\n";
    cout << "5. Display Account Details\n";
    cout << "6. Exit\n";
    cout << "----------------------------------------\n";
}

int main() {
    Bank bank;
    bank.loadFromFile();
    int choice = 0;

    while (choice != 6) {
        showMenu();
        choice = readNumber("Enter your choice (1-6): ", 1, 6);
        cout << "\n";

        switch (choice) {
            case 1: bank.createAccount(); break;
            case 2: bank.depositMoney(); break;
            case 3: bank.withdrawMoney(); break;
            case 4: bank.checkBalance(); break;
            case 5: bank.displayAccountDetails(); break;
            case 6: cout << "Thank you for using the Bank Management System.\n"; break;
        }
    }
    return 0;
}
