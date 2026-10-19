#include "Funds.h"
#include <iostream>

Funds::Funds(Transaction_Writer& transaction) : transaction(transaction) {}

void Funds::add_credit(const std::string& user) {
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

void Funds::refund(const std::string& owner, const std::string& seller, double credit) {
    transaction.record_refund(owner, seller, credit);
}
