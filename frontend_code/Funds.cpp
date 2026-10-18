#pragma once
#include <iostream>
#include <string>
#include "Transaction_Writer.cpp"

// Handles adding credit and refunds between users.
class Funds {
private:
    // Shared writer, held by reference so every class logs to the same file.
    Transaction_Writer& transaction;

public:
    // Initializes with the shared writer.
    Funds(Transaction_Writer& transaction) : transaction(transaction) {}

    // Prompts for an amount and records it as credit added to `user`.
    void add_credit(const std::string& user) {
        std::string amount_text;
        std::cout << "Amount: ";
        std::getline(std::cin, amount_text);
        double amount = 0.0;
        try { amount = std::stod(amount_text); } catch (...) {
            std::cout << "Error: invalid amount.\n";
            return;
        }
        if (amount <= 0.0) {
            std::cout << "Error: amount must be positive.\n";
            return;
        }
        transaction.record_user_transaction("06", user, "", amount);
    }

    // Records a refund of `credit` from `seller` to `owner`.
    void refund(const std::string& owner, const std::string& seller, double credit) {
        transaction.record_refund(owner, seller, credit);
    }
};
