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
    // The $1000 limit is per account per session, whoever adds the credit (admin included).
    if (target_user.get_credit_in_session() + amount > SESSION_LIMIT) {
        std::cout << "ERROR: Cannot add more than $1000.00 in credit per session.\n";
        return;
    }

    session.add_credit(target, amount);
    User updated = session.find_user(target);
    transaction.record_user_transaction("06", target, updated.get_user_type(), updated.get_total_credit());
    std::cout << "Added $" << std::fixed << std::setprecision(2) << amount
              << " credit to " << target << ".\n";
}

// Each field is checked right after it is entered, so an error ends the command
// before the next prompt: buyer (blank / not found), seller (blank / not found),
// then the amount format.
void Funds::refund() {
    if (!session.is_admin()) {
        std::cout << "ERROR: Permission denied, refund requires admin privileges.\n";
        return;
    }
    std::cout << "Enter buyer username:\n";
    std::string buyer;
    std::getline(std::cin, buyer);
    if (buyer.empty()) {
        std::cout << "ERROR: Buyer username cannot be blank.\n";
        return;
    }
    if (session.find_user(buyer).get_username().empty()) {
        std::cout << "ERROR: Buyer not found.\n";
        return;
    }
    std::cout << "Enter seller username:\n";
    std::string seller;
    std::getline(std::cin, seller);
    if (seller.empty()) {
        std::cout << "ERROR: Seller username cannot be blank.\n";
        return;
    }
    if (session.find_user(seller).get_username().empty()) {
        std::cout << "ERROR: Seller not found.\n";
        return;
    }
    std::cout << "Enter refund amount:\n";
    std::string amount_text;
    std::getline(std::cin, amount_text);
    double amount = 0.0;
    size_t parsed = 0;
    try { amount = std::stod(amount_text, &parsed); } catch (...) { parsed = 0; }
    if (parsed == 0 || parsed != amount_text.size() || amount <= 0.0) {
        std::cout << "ERROR: Invalid amount format.\n";
        return;
    }

    session.adjust_credit(buyer, amount);
    session.adjust_credit(seller, -amount);
    transaction.record_refund(buyer, seller, amount);
    std::cout << "Refunded $" << std::fixed << std::setprecision(2) << amount
              << " from " << seller << " to " << buyer << ".\n";
}
