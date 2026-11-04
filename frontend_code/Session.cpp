#include "Session.h"
#include <iostream>

// Human-readable name for a two-letter user type code.
static std::string type_name(const std::string& code) {
    if (code == "AA") return "admin";
    if (code == "FS") return "full-standard";
    if (code == "BS") return "buy-standard";
    if (code == "SS") return "sell-standard";
    return code;
}

Session::Session(Transaction_Writer& transaction, const std::vector<User>& all_users)
    : current_user(), all_users(all_users), transaction(transaction), logged_in(false) {}

void Session::login(const std::string& user) {
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
    std::cout << "Login successful, welcome " << found.get_username()
              << " (" << type_name(found.get_user_type()) << ").\n";
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
    std::cout << "Logout successful.\n";
}

User Session::find_user(const std::string& user) {
    for (const User& u : all_users) {
        if (u.get_username() == user) return u;
    }
    return User();
}

bool Session::is_logged_in() const { return logged_in; }
bool Session::is_admin() const { return logged_in && current_user.get_user_type() == "AA"; }
User Session::get_current_user() const { return current_user; }
std::vector<User>& Session::get_all_users() { return all_users; }

void Session::add_credit(const std::string& user, double amount) {
    for (User& u : all_users) {
        if (u.get_username() == user) {
            u.set_total_credit(u.get_total_credit() + amount);
            u.set_credit_in_session(u.get_credit_in_session() + amount);
            if (logged_in && current_user.get_username() == user) current_user = u;
            return;
        }
    }
}
