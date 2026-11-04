#include "Funds.h"
#include <iomanip>
#include <iostream>

Funds::Funds(Session& session, Transaction_Writer& transaction)
    : transaction(transaction), session(session) {}

void Funds::add_credit(const std::string& user) {
    static const double SESSION_LIMIT = 1000.00;
    std::string amount_text;
    std::cout << "Enter amount to add:\n";
    std::getline(std::cin, amount_text);
    double amount = 0.0;
    size_t parsed = 0;
    try { amount = std::stod(amount_text, &parsed); } catch (...) {
        std::cout << "ERROR: Invalid amount format.\n";
        return;
    }
    if (parsed != amount_text.size() || amount <= 0.0) {
        std::cout << "ERROR: Invalid amount format.\n";
        return;
    }

    // Admins credit another account; everyone else credits their own.
    std::string target = user;
    if (session.is_admin()) {
        std::cout << "Enter username to credit:\n";
        std::getline(std::cin, target);
    }
    User target_user = session.find_user(target);
    if (target_user.get_username().empty()) {
        std::cout << "ERROR: Username not found.\n";
        return;
    }
    if (!session.is_admin() && target_user.get_credit_in_session() + amount > SESSION_LIMIT) {
        std::cout << "ERROR: Cannot add more than $1000.00 in credit per session.\n";
        return;
    }

    session.add_credit(target, amount);
    User updated = session.find_user(target);
    transaction.record_user_transaction("06", target, updated.get_user_type(), updated.get_total_credit());
    std::cout << "Added $" << std::fixed << std::setprecision(2) << amount
              << " credit to " << target << ".\n";
}

void Funds::refund(const std::string& buyer, const std::string& seller, double credit) {
    transaction.record_refund(buyer, seller, credit);
}
