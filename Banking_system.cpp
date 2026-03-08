#include <iostream>
#include <vector>
#include <fstream>
using namespace std;

// Permission flags
//use bitwise flags because it lets us store multiple permissions in one integer efficiently
const unsigned int CAN_WITHDRAW = 1;
const unsigned int CAN_DEPOSIT = 2;
const unsigned int CAN_TRANSFER = 4;
const unsigned int VIP = 8;

// ABSTRACT BASE CLASS
class Account {
protected:
    int accountId;
    string name;
    double balance;
    unsigned int permissions;
    vector<double> transactions;

public:
    Account(int id, const string& n, double b, unsigned int p)
        : accountId(id), name(n), balance(b), permissions(p) {}

    virtual void deposit(double amount) = 0;
    virtual void withdraw(double amount) = 0;
    virtual void showAccount() = 0;
    virtual void saveToFile(ofstream &file) = 0;

    int getId() const { return accountId; }
    double getBalance() const { return balance; }
    const vector<double>& getTransactions() const { return transactions; }

    virtual ~Account() {}
};

// SAVINGS ACCOUNT
//derived class , inherits from accounts
//checks permission and balance
class SavingsAccount : public Account {
public:
    SavingsAccount(int id, const string& n, double b, unsigned int p)
        : Account(id, n, b, p) {}

    void deposit(double amount) override {
        if (permissions & CAN_DEPOSIT) {
            balance += amount;
            transactions.push_back(amount);
            cout << "Deposit successful\n";
        } else cout << "Deposit not allowed\n";
    }

    void withdraw(double amount) override {
        if (permissions & CAN_WITHDRAW) {
            if (balance >= amount) {
                balance -= amount;
                transactions.push_back(-amount);
                cout << "Withdraw successful\n";
            } else cout << "Insufficient balance\n";
        } else cout << "Withdraw not allowed\n";
    }

    void showAccount() override {
        cout << "\nSavings Account\n";
        cout << "ID: " << accountId << "\nName: " << name << "\nBalance: " << balance << "\n";
        cout << "Transactions:\n";
        for (size_t i = 0; i < transactions.size(); i++)
            cout << transactions[i] << "\n";
    }

    void saveToFile(ofstream &file) override {
        file << "ACCOUNT Savings\n" << accountId << " " << name << " " << balance << " " << permissions << "\n";
        file << "TRANSACTIONS\n";
        for (size_t i = 0; i < transactions.size(); i++)
            file << transactions[i] << "\n";
        file << "END\n";
    }
};

// CURRENT ACCOUNT
//derived class , inherits from accounts
//checks permission and balance
class CurrentAccount : public Account {
public:
    CurrentAccount(int id, const string& n, double b, unsigned int p)
        : Account(id, n, b, p) {}

    void deposit(double amount) override {
        if (permissions & CAN_DEPOSIT) {
            balance += amount;
            transactions.push_back(amount);
            cout << "Deposit successful\n";
        } else cout << "Deposit not allowed\n";
    }

    void withdraw(double amount) override {
        if (permissions & CAN_WITHDRAW) {
            if (balance >= amount) {
                balance -= amount;
                transactions.push_back(-amount);
                cout << "Withdraw successful\n";
            } else cout << "Insufficient balance\n";
        } else cout << "Withdraw not allowed\n";
    }

    void showAccount() override {
        cout << "\nCurrent Account\n";
        cout << "ID: " << accountId << "\nName: " << name << "\nBalance: " << balance << "\n";
        cout << "Transactions:\n";
        for (size_t i = 0; i < transactions.size(); i++)
            cout << transactions[i] << "\n";
    }

    void saveToFile(ofstream &file) override {
        file << "ACCOUNT Current\n" << accountId << " " << name << " " << balance << " " << permissions << "\n";
        file << "TRANSACTIONS\n";
        for (size_t i = 0; i < transactions.size(); i++)
            file << transactions[i] << "\n";
        file << "END\n";
    }
};

// FIND ACCOUNT
Account* findAccount(vector<Account*>& accounts, int id) {
    for (size_t i = 0; i < accounts.size(); i++)
        if (accounts[i]->getId() == id)
            return accounts[i];
    return NULL; 
}

// SAVE ALL ACCOUNTS
//SAVEALL() writes all accounts and transactions to bank.txt.
//accounts are stored in vector<Account*>
void saveAll(vector<Account*>& accounts) {
    ofstream file("bank.txt");
    if (!file) {
        cout << "Error opening file!\n";
        return;
    }
    for (size_t i = 0; i < accounts.size(); i++)
        accounts[i]->saveToFile(file);

    cout << "Accounts saved to file\n";
}

// MONTHLY SUMMARY (distribute transactions over months)
//MONTHLY SUMMARY() loops through all transactions and sums them for 12 months.
void monthlySummary(vector<Account*>& accounts) {
    double monthlyTotals[12] = {0};
    for (size_t i = 0; i < accounts.size(); i++) {
        const vector<double>& trans = accounts[i]->getTransactions();
        for (size_t j = 0; j < trans.size(); j++)
            monthlyTotals[j % 12] += trans[j]; // simple distribution
    }
    cout << "\nMonthly Summary:\n";
    for (int i = 0; i < 12; i++)
        cout << "Month " << i + 1 << " : " << monthlyTotals[i] << "\n";
}

// MAIN
int main() {
    vector<Account*> accounts;
    int choice;

    while (true) {
        cout << "\n===== BANK SYSTEM =====\n";
        cout << "1 Create Savings Account\n2 Create Current Account\n3 Deposit\n4 Withdraw\n";
        cout << "5 Show Account\n6 Save to File\n7 Monthly Summary\n8 Exit\n";
        cout << "Choice: ";
        cin >> choice;

        if (choice == 1 || choice == 2) {
            int id;
            string name;
            double balance;

            cout << "Enter ID: ";
            cin >> id;
            cin.ignore(); // ignore leftover newline
            cout << "Enter Name: ";
            getline(cin, name);
            cout << "Enter Initial Balance: ";
            cin >> balance;

            unsigned int permissions = (choice == 1) ? (CAN_WITHDRAW | CAN_DEPOSIT | CAN_TRANSFER) : (CAN_WITHDRAW | CAN_DEPOSIT);

            if (choice == 1)
                accounts.push_back(new SavingsAccount(id, name, balance, permissions));
            else
                accounts.push_back(new CurrentAccount(id, name, balance, permissions));
        }
        else if (choice == 3 || choice == 4) {
            int id;
            double amount;
            cout << "Enter Account ID: ";
            cin >> id;
            cout << "Enter Amount: ";
            cin >> amount;

            Account* acc = findAccount(accounts, id);
            if (acc) {
                if (choice == 3) acc->deposit(amount);
                else acc->withdraw(amount);
            } else cout << "Account not found\n";
        }
        else if (choice == 5) {
            int id;
            cout << "Enter Account ID: ";
            cin >> id;
            Account* acc = findAccount(accounts, id);
            if (acc) acc->showAccount();
            else cout << "Account not found\n";
            
        }
        else if (choice == 6) saveAll(accounts);
        else if (choice == 7) monthlySummary(accounts);
        else if (choice == 8) break;
        else cout << "Invalid choice, try again.\n";
    }

    // free memory
    for (size_t i = 0; i < accounts.size(); i++)
        delete accounts[i];

    return 0;
}

