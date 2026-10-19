#include "Session.h"
#include <iostream>

Session::Session(Transaction_Writer& transaction, const std::vector<User>& all_users)
    : current_user(), all_users(all_users), transaction(transaction), logged_in(false) {}

void Session::login(const std::string& user, const std::string& pw) {
    (void)pw;
    if (logged_in) {
        std::cout << "Error: a user is already logged in.\n";
        return;
    }
    User found = find_user(user);
    if (found.get_username().empty()) {
        std::cout << "Error: unknown user.\n";
        return;
    }
    current_user = found;
    logged_in = true;
}

void Session::process_transactions() {
    if (!logged_in) {
        std::cout << "Error: no user is logged in.\n";
        return;
    }
    std::cout << "Processing transactions for " << current_user.get_username() << "\n";
}

void Session::logout(const std::string& user) {
    if (!logged_in || current_user.get_username() != user) {
        std::cout << "Error: user is not logged in.\n";
        return;
    }
    transaction.record_user_transaction("00", current_user.get_username(),
                                        current_user.get_user_type(),
                                        current_user.get_total_credit());
    logged_in = false;
    current_user = User();
}

User Session::find_user(const std::string& user) {
    for (const User& u : all_users) {
        if (u.get_username() == user) return u;
    }
    return User();
}

bool Session::is_logged_in() const { return logged_in; }
User Session::get_current_user() const { return current_user; }
std::vector<User>& Session::get_all_users() { return all_users; }
